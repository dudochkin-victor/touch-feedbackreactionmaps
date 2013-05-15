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

#include "mftouchscreenlistener.h"
#include "mfkernelthread.h"

#include <sys/socket.h>

class MfTouchScreenListenerPriv
{
    public:
    MfTouchScreenListenerPriv(MfReactionMapStack *reactionMapStack);

    MfKernelThread kernelThread;

    // 0 - Used by MfTouchScreenListener on main thread
    // 1 - Used by MfKernelThread on listener thread
    int communicationSocketFdPair[2];

    bool isActive;
};

MfTouchScreenListenerPriv::MfTouchScreenListenerPriv(MfReactionMapStack *reactionMapStack)
    : kernelThread(reactionMapStack), isActive(true)
{
    communicationSocketFdPair[0] = -1;
    communicationSocketFdPair[1] = -1;
}

MfTouchScreenListener::MfTouchScreenListener(MfReactionMapStack *reactionMapStack,
                                             QObject *parent)
    : QObject(parent)
{
    d = new MfTouchScreenListenerPriv(reactionMapStack);
}

MfTouchScreenListener::~MfTouchScreenListener()
{
    if (!d) {
        return;
    }

    if (d->kernelThread.isRunning()) {
        stopListening();
    }

    delete d;
    d = NULL;
}

bool MfTouchScreenListener::startListening()
{
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, d->communicationSocketFdPair)) {
        qCritical("MfTouchScreenListener: Failed to create socket pair");
        return false;
    }

    d->kernelThread.communicationSocketFd = d->communicationSocketFdPair[1];

    // Set the stack size quite low to avoid too much memory mappings
    d->kernelThread.setStackSize(512*1024);
    d->kernelThread.start();
    return true;
}

bool MfTouchScreenListener::stopListening()
{
    char msg = MF_KERNEL_THREAD_STOP;

    if (!d->kernelThread.isRunning()) {
        // Nothing to be done
        return true;
    }

    // Send MF_KERNEL_THREAD_STOP to thread
    if (write(d->communicationSocketFdPair[0], &msg, sizeof(char)) != sizeof(char)) {
        qWarning("MfTouchScreenListener: Failed to send thread stop msg.");
    }

    if (!d->kernelThread.wait(1000)) {
        // Ok, let's use brute force
        d->kernelThread.terminate();
        d->kernelThread.wait();
    }

    if (close(d->communicationSocketFdPair[0]) == -1) {
        qWarning("MfTouchScreenListener: Failed to close stopListening socket");
    }
    d->communicationSocketFdPair[0] = -1;

    if (close(d->communicationSocketFdPair[1]) == -1) {
        qWarning("MfTouchScreenListener: Failed to close stopListening socket");
    }
    d->communicationSocketFdPair[1] = -1;

    qDebug("MfTouchScreenListener: Stopped nicely");
    return true;
}

bool MfTouchScreenListener::isActive()
{
    return d->isActive;
}

void MfTouchScreenListener::setActive(bool active)
{
    if (active != d->isActive) {
        char msg;
        if (active) {
            msg = MF_KERNEL_THREAD_RESUME;
        } else {
            msg = MF_KERNEL_THREAD_PAUSE;
        }

        if (write(d->communicationSocketFdPair[0], &msg, sizeof(char)) != sizeof(char)) {
            qWarning("MfTouchScreenListener: Failed to send control message to MfKernelThread.");
        }

        d->isActive = active;
    }
}
