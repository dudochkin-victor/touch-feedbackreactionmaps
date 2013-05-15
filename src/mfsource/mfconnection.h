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

#ifndef MFCONNECTION_H
#define MFCONNECTION_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QPointer>

class QLocalSocket;
class MfReactionMapStack;
class MfReactionMap;
class MfSession;

struct SessionData
{
    quint32 id;
    QString clientName;
    quint32 windowId;
    MfSession* session;
};

class MfConnection : public QObject
{
    Q_OBJECT

public:
    MfConnection(QLocalSocket *connection, MfReactionMapStack *stack, QObject *parent = 0);
    virtual ~MfConnection();

private slots:
    void readSocketData();
    void sessionSetupReady();

private:
    void init();
    void readRequest();
    void goDoomed();

    enum {
        StateClientInit,
        StateListening,
        StateDoomed
    } state;

    QPointer<QLocalSocket> socket;
    MfReactionMapStack *stack;
    QMap <quint32, MfReactionMap *> reactionMaps;
    qint64 clientPid;
    QVector<SessionData> sessions;
};

#endif
