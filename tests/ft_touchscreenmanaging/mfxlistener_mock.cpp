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

#include "mfxlistener_mock.h"
#include <mfxlistener.h>
#include <QDebug>

MfXListenerMock stackEmitter(NULL);

void MfXListenerMock::emitWindowStackChanged(QList<quint32> newWindowStack)
{
    emit windowStackChanged(newWindowStack);
}

MfXListener::MfXListener(MfReactionMapStack *reactionMapStack, QObject *parent)
    : QThread(parent)
{
    Q_UNUSED(reactionMapStack);
}

MfXListener::~MfXListener()
{
    stopListening();
}

bool MfXListener::startListening()
{
    bool ok;
    ok = connect(&stackEmitter, SIGNAL(windowStackChanged(QList<quint32>)),
                 SIGNAL(windowStackChanged(QList<quint32>)));
    if (!ok) {
        qCritical("Failed to connect MfXListener signal");
    }

    start(QThread::NormalPriority);

    return true;
}

bool MfXListener::stopListening()
{
    if (!isRunning()) {
        // Nothing to be done
        return true;
    }

    exit();

    if (!wait(500)) {
        // Ok, let's use brute force
        qCritical() << "Terminating by force";
        terminate();
        wait();
    }

    return true;
}

void MfXListener::run()
{
    exec();
}
