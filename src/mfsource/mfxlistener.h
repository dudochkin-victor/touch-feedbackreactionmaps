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

#ifndef MFXLISTENER_H
#define MFXLISTENER_H

#include <QPointer>
#include <QThread>

#include <QDataStream>
#include <QTextStream>
#include <QMetaType>

class MfXListenerPriv;
class MfReactionMapStack;

/*!
 * Listens for X events in a separate thread, emitting signals when
 * anything interesting pops up.
 *
 * In other words: this class conveniently translates relevant X events
 * into nice Qt signals.
 */
class MfXListener : public QThread {
    Q_OBJECT
public:
    MfXListener(MfReactionMapStack *reactionMapStack, QObject *parent = 0);
    virtual ~MfXListener();

    // Start listening for X events.
    // Use this method instead of QThread::start() since it provides a way
    // to nicely stop the listener thread afterwards via stopListening().
    bool startListening();

    // Stop listening for X events.
    // Listener thread should have been started with startListening().
    bool stopListening();

signals:
    // Emitted when _NET_CLIENT_LIST_STACKING property
    // in the root Window (i.e. window manager) changes
    // @param newWindowStack X Window IDs ordered according to
    //                       new stacking order, bottom-to-top.
    void windowStackChanged(QList<quint32> newWindowStack);

protected:
    void run();

private:
    QPointer<MfXListenerPriv> d;

    // 0 - Used on main thread side
    // 1 - Used on listener thread side
    int stopListeningSocketFdPair[2];

    MfReactionMapStack *reactionMapStack;
};

#endif
