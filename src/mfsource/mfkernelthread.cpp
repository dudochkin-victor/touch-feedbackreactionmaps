/* This file is part of meegofeedback-reactionmaps
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mfkernelthread.h"

#include <asm-generic/errno-base.h>

#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>

#include <QtDebug>
#include <QString>

#include "mfdefaulttranslator.h"
#include "mfreactionmapstack.h"
#include "mfsettings.h"

static QString touchScreenDevicePath("/dev/input/ts");

MfKernelThread::MfKernelThread(MfReactionMapStack *reactionMapStack, QObject *parent) :
    QThread(parent), reactionMapStack(reactionMapStack), isActive(true)
{
}

MfKernelThread::~MfKernelThread()
{
    qDeleteAll(touchDevicesVector);
    touchDevicesVector.clear();
}

void MfKernelThread::run()
{
    if (!setup()) {
        return;
    }

    listen();
}

bool MfKernelThread::setup()
{
    return scanInputDevices();
}

bool MfKernelThread::listen()
{
    bool ok = true;
    int result;

    // We must have at least one touch device
    Q_ASSERT(touchDevicesVector.size() > 0);

    nfds = computeNFDS();

    do {
        ok = waitForEvent();
    } while (ok);

    // Close all open input event file descriptors
    for (int i = 0; i < touchDevicesVector.size(); ++i) {
        if (touchDevicesVector[i]->fd >= 0) {
            result = close(touchDevicesVector[i]->fd);
            touchDevicesVector[i]->fd = -1;
            if (result == -1) {
                qWarning("MfKernelThread: Failed to close input event fd.");
            }
        }
    }

    // Is it a right place to do the clean up?
    qDeleteAll(touchDevicesVector);
    touchDevicesVector.clear();

    qDebug("MfKernelThread: Finished listening");

    return true;
}

bool MfKernelThread::readHostMessage()
{
    bool ret = true;
    int bytes;
    char buf[1];

    bytes = read(communicationSocketFd, buf, 1);
    if (bytes < 1) {
        qWarning("MfKernelThread: Failed to read a message from host at readHostMessage");
        return ret;
    }

    switch (buf[0]) {
        case MF_KERNEL_THREAD_STOP:
            // Return false to break from the loop and end thread
            ret = false;
            break;

        case MF_KERNEL_THREAD_PAUSE:
            // Mark status as inactive and close all kernel interfaces
            isActive = false;
            for (int i = 0; i < touchDevicesVector.size(); ++i) {
                MfTouchDevice *touchDevice = touchDevicesVector[i];
                if (touchDevice->fd >= 0) {
                    close(touchDevice->fd);
                    touchDevice->fd = -1;
                }
            }
            nfds = computeNFDS();
            break;

        case MF_KERNEL_THREAD_RESUME:
            // Mark status as active and reopen all kernel interfaces
            isActive = true;
            for (int i = 0; i < touchDevicesVector.size(); ++i) {
                MfTouchDevice *touchDevice = touchDevicesVector[i];
                if (touchDevice->fd < 0) {
                    if ((touchDevice->fd = open(touchScreenDevicePath.toAscii().constData(), O_RDONLY)) < 0) {
                        touchDevice->fd = -errno;
                        qWarning("MfKernelThread: open error");
                    }
                }
            }
            nfds = computeNFDS();
            break;

        default:
            break;
    }

    return ret;
}

bool MfKernelThread::waitForEvent()
{
    fd_set readFdSet;
    int ret = 0;
    bool ok = true;
    int i;

    // Build the file descriptors set
    FD_ZERO(&readFdSet);
    FD_SET(communicationSocketFd, &readFdSet);

    // Only add touchsceen file descriptors if display is active
    if (isActive) {
        for (i = 0; i < touchDevicesVector.size(); ++i) {
            if (touchDevicesVector[i]->fd >= 0) {
                FD_SET(touchDevicesVector[i]->fd, &readFdSet);
            }
        }
    }

    ret = select(nfds, &readFdSet, NULL/*writefds*/, NULL/*exceptfds*/, NULL);
    if (ret == -1) {
        qCritical("MfKernelThread: Error in select()");
        ok = false;
    } else if (FD_ISSET(communicationSocketFd, &readFdSet)) {
            ok = readHostMessage();
    } else if (isActive) {
        for (i = 0; i < touchDevicesVector.size(); ++i) {
            if (touchDevicesVector[i]->fd >= 0) {
                MfTouchDevice *touchDevice = touchDevicesVector[i];
                if (FD_ISSET(touchDevice->fd, &readFdSet)) {
                    touchDevice->processEvents();
                }
            }
        }
    }

    return ok;
}


bool MfKernelThread::scanInputDevices()
{
    int fd;
    fd = checkInputDevice(touchScreenDevicePath.toAscii().constData());
    if (fd == -ENOENT)
        return false;

    if (fd > 0) {
        MfTouchDevice* touchDevice = new MfTouchDevice();

        touchDevice->fd = fd;
        touchDevice->reactionMapStack = reactionMapStack;
        touchDevice->fetchCaps();
        touchDevice->translator = new MfDefaultTranslator();
        connect(touchDevice->translator, SIGNAL(press(int, const QPoint&)),
                touchDevice, SLOT(pressEvent(int, const QPoint&)));
        connect(touchDevice->translator, SIGNAL(release(int, const QPoint&)),
                touchDevice, SLOT(releaseEvent(int, const QPoint&)));
        touchDevicesVector << touchDevice;
    }

    return touchDevicesVector.size() > 0;
}

int MfKernelThread::checkInputDevice(const char *name)
{
    /* Enough */
    uint8_t evtype_bitmask[20 + 1];
    int fd;
    char devname[256] = "Unknown";

    if ((fd = open(name, O_RDONLY)) < 0)
    {
        fd = -errno;
        qWarning("MfKernelThread: open error");
        return fd;
    }

    ioctl(fd, EVIOCGNAME(sizeof(devname)), devname);
    qDebug("MfKernelThread: Checking %s: name: \"%s\"...", name, devname);

    if (ioctl(fd, EVIOCGBIT(0, sizeof(evtype_bitmask)), evtype_bitmask) < 0) {
        qDebug("MfKernelThread: %s - EVIOCGBIT error", name);
        goto err;
    }

#define test_bit(bit, array)    (array[bit/8] & (1<<(bit%8)))
    if (!test_bit(EV_ABS, evtype_bitmask))
        goto err;

    if (ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(evtype_bitmask)), evtype_bitmask) < 0) {
        qDebug("MfKernelThread: %s - EVIOGBIT EV_ABS error", name);
        goto err;
    }

    if (!test_bit(ABS_X, evtype_bitmask) || !test_bit(ABS_Y, evtype_bitmask))
        goto err;
#undef test_bit

    qDebug("MfKernelThread: OK");

    return fd;
 err:
    qDebug("MfKernelThread: Not a proper touch point.");
    if (fd) {
        close(fd);
        fd = -1;
    }

    return fd;
}

int MfKernelThread::findHighestTouchPointFd()
{
    int highestFd = -1;

    for (int i = 0; i < touchDevicesVector.size(); ++i) {
        MfTouchDevice *touchDevice = touchDevicesVector[i];
        if (touchDevice->fd > highestFd) {
            highestFd = touchDevice->fd;
        }
    }

    return highestFd;
}

int MfKernelThread::computeNFDS()
{
    int highestFd = -1;

    // Only consider touch screen fd if screen is active
    if (isActive) {
        highestFd = findHighestTouchPointFd();
    }

    if (highestFd > communicationSocketFd) {
        return highestFd + 1;
    } else {
        return communicationSocketFd + 1;
    }
}

MfTouchDevice::MfTouchDevice() : translator(0)
{
}

MfTouchDevice::~MfTouchDevice()
{
    delete translator;
    translator = 0;
}

void MfTouchDevice::processEvents()
{
    struct input_event ev[64];
    int events = 0;

    events = readEvents(ev, 64*sizeof(struct input_event));
    for (int i = 0; i < events; ++i) {
        translator->addRawEvent(&ev[i]);
    }
}

int MfTouchDevice::readEvents(struct input_event *ev_list, int max_size)
{
    int bytes;

    bytes = read(fd, ev_list, max_size);
    if (bytes < (int)sizeof(struct input_event))
    {
        qCritical("MfKernelThread::readEvents - short read");
        return -1;
    }

    if (bytes % sizeof(struct input_event))
    {
        qCritical("MfKernelThread::readEvents - stray bytes or unaligned read");
        return -2;
    }

    return bytes/sizeof(struct input_event);
}

void MfTouchDevice::fetchCaps()
{
    queryAbsValues(fd, ABS_X, &tsCaps.abs_x);
    queryAbsValues(fd, ABS_Y, &tsCaps.abs_y);

    tsCaps.rangeX = tsCaps.abs_x.maximum - tsCaps.abs_x.minimum;
    tsCaps.rangeY = tsCaps.abs_y.maximum - tsCaps.abs_y.minimum;

    qDebug("MfTouchDevice: MinX: %d, MaxX: %d, MinY: %d, MaxY: %d",
           tsCaps.abs_x.minimum,
           tsCaps.abs_x.maximum,
           tsCaps.abs_y.minimum,
           tsCaps.abs_y.maximum);
}

int MfTouchDevice::queryAbsValues(int fd, int abs, struct input_absinfo *info)
{
    return ioctl(fd, EVIOCGABS(abs), info);
}

QPoint MfTouchDevice::calcPixelCoords(const QPoint& tsCoords)
{
    int sx, sy;
    QPoint finalCoords;

    // Make sure no out-of-bounds values are resulted
    sx = tsCoords.x();
    if (sx < tsCaps.abs_x.minimum) {
        sx = (int)tsCaps.abs_x.minimum;
    } else if (sx > tsCaps.abs_x.maximum) {
        sx = tsCaps.abs_x.maximum;
    }

    sy = tsCoords.y();
    if (sy < tsCaps.abs_y.minimum) {
        sy = (int)tsCaps.abs_y.minimum;
    } else if (sy > tsCaps.abs_y.maximum) {
        sy = tsCaps.abs_y.maximum;
    }

    // tsCaps.rangeX and tsCaps.rangeY report the same value as
    // tsCaps.abs_x.maximum and tsCaps.abs_y.maximum respectively.
    // This is wrong behaviour since the minimum values for x an y are
    // zero. Thus we add one to the ranges below.
    finalCoords.rx() = ( MfSettings::reactionMapWidth() * (sx - tsCaps.abs_x.minimum) ) /
              (tsCaps.rangeX+1);

    finalCoords.ry() = ( MfSettings::reactionMapHeight() * (sy - tsCaps.abs_y.minimum) ) /
              (tsCaps.rangeY+1);
    return finalCoords;
}

void MfTouchDevice::pressEvent(int fingerId, const QPoint& position)
{
    Q_UNUSED(fingerId);
    QPoint finalCoords = calcPixelCoords(position);
    reactionMapStack->pressed(finalCoords);

}

void MfTouchDevice::releaseEvent(int fingerId, const QPoint& position)
{
    Q_UNUSED(fingerId);
    QPoint finalCoords = calcPixelCoords(position);
    reactionMapStack->released(finalCoords);

}
