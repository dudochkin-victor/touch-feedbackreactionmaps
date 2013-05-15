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

#include "mfxlistener.h"
#include "mfxlistener_p.h"

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>

#include <QDebug>

MfXListener::MfXListener(MfReactionMapStack *reactionMapStack, QObject *parent)
    : QThread(parent), reactionMapStack(reactionMapStack)
{
    qRegisterMetaType< QList<quint32> >("QList<quint32>");
}

MfXListener::~MfXListener()
{
    stopListening();

    if (!d.isNull()) {
        delete d;
        d = 0;
    }
}

bool MfXListener::startListening()
{
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, stopListeningSocketFdPair)) {
        qCritical("MfTouchScreenListener: Failed to create socket pair");
        return false;
    }

    start(QThread::NormalPriority);

    return true;
}

bool MfXListener::stopListening()
{
    char msg = 1;

    if (!isRunning()) {
        // Nothing to be done
        return true;
    }

    // Just send something
    if (write(stopListeningSocketFdPair[0], &msg, sizeof(char)) != sizeof(char)) {
        qWarning() << __PRETTY_FUNCTION__ << "Failed to send thread stop msg.";
    }

    if (!wait(500)) {
        // Ok, let's use brute force
        terminate();
        wait();
    }

    if (close(stopListeningSocketFdPair[0]) == -1) {
        qWarning() << __PRETTY_FUNCTION__ << "Failed to close stopListening socket";
    }
    stopListeningSocketFdPair[0] = -1;

    if (close(stopListeningSocketFdPair[1]) == -1) {
        qWarning() << __PRETTY_FUNCTION__ << "Failed to close stopListening socket";
    }
    stopListeningSocketFdPair[1] = -1;

    return true;
}

void MfXListener::run()
{
    bool ok = true;
    bool terminate = false;
    int x11Fd;
    int nfds;
    fd_set readFdSet;
    int ret = 0;

    // MfXListenerPriv object belongs to the listener thread
    d = new MfXListenerPriv(reactionMapStack);

    ok = QObject::connect(d, SIGNAL(windowStackChanged(QList<quint32>)),
                          this, SIGNAL(windowStackChanged(QList<quint32>)));
    if (!ok) {
        qCritical("Failed to connect MfXListener signal");
    }

    if (!d->init()) {
        // Cannot work without connection to X server
        kill(getpid(), SIGTERM);
        return;
    }

    x11Fd = xcb_get_file_descriptor(d->getConnection());

    if (x11Fd > stopListeningSocketFdPair[1]) {
        nfds = x11Fd + 1;
    } else {
        nfds = stopListeningSocketFdPair[1] + 1;
    }

    while (ok) {
        // Build the file descriptors set
        FD_ZERO(&readFdSet);
        FD_SET(x11Fd, &readFdSet);
        FD_SET(stopListeningSocketFdPair[1], &readFdSet);

        xcb_flush(d->getConnection());

        ret = select(nfds, &readFdSet, NULL/*writefds*/, NULL/*exceptfds*/, NULL);
        if (ret == -1) {
            qCritical() << __PRETTY_FUNCTION__ << "Error in select()";
            ok = false;
            terminate = true;
        } else if (FD_ISSET(stopListeningSocketFdPair[1], &readFdSet)) {
            qDebug() << __PRETTY_FUNCTION__ << "Stopping nicely...";
            // It's time to exit
            ok = false;
        } else if (FD_ISSET(x11Fd, &readFdSet)) {
            ok = d->processXEvents();
            terminate = true;
        }
    }

    delete d;
    d = 0;

    if (terminate) {
        // Terminate program if stopping the thread was not
        // intentional.
        kill(getpid(), SIGTERM);
    }
}
