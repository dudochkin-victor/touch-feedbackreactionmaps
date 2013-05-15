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

#ifndef MFKERNELTHREAD_H
#define MFKERNELTHREAD_H

#include <linux/input.h>

#include <QPoint>
#include <QThread>
#include <QVector>

// Define communication protocol between MfKernelThread
// and MfTouchScreenListener (kernel implementation)
#define MF_KERNEL_THREAD_STOP    0
#define MF_KERNEL_THREAD_PAUSE   1
#define MF_KERNEL_THREAD_RESUME  2

class MfReactionMapStack;
class MfTouchScreenTranslator;

class MfTouchDevice : public QObject {
    Q_OBJECT
public:
    MfTouchDevice();
    ~MfTouchDevice();

    void processEvents();
    int readEvents(struct input_event *ev_list, int max_size);
    void fetchCaps();
    static int queryAbsValues(int fd, int abs, struct input_absinfo *info);
    QPoint calcPixelCoords(const QPoint& tsCoords);

public slots:
    void pressEvent(int fingerId, const QPoint& position);
    void releaseEvent(int fingerId, const QPoint& position);
public:
    // file descriptor for /dev/input/ts
    int fd;
    MfReactionMapStack *reactionMapStack;
    MfTouchScreenTranslator* translator;
private:
    // Touch screen capabilities
    struct TSCaps {
        struct input_absinfo abs_x;
        struct input_absinfo abs_y;
        int rangeX;
        int rangeY;
    };
    TSCaps tsCaps;
};

class MfKernelThread : public QThread
{
    Q_OBJECT
public:
    explicit MfKernelThread(MfReactionMapStack *reactionMapStack,
                            QObject *parent = 0);
    ~MfKernelThread();

    int communicationSocketFd;


protected:
    void run();

private:
    bool setup();
    bool listen();

    bool readHostMessage();
    bool scanInputDevices();
    int checkInputDevice(const char *name);
    bool waitForEvent();

    int findHighestTouchPointFd();

    // Compute nfds parameter used in select() function call
    int computeNFDS();

    QVector<MfTouchDevice*> touchDevicesVector;
    MfReactionMapStack *reactionMapStack;
    int nfds;
    bool isActive;
};

#endif
