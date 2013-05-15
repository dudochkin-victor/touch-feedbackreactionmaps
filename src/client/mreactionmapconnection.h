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

#ifndef MREACTIONMAPCONNECTION_H
#define MREACTIONMAPCONNECTION_H

#include <QObject>
#include <QLocalSocket>
#include <QMap>

#include <mfcommondata.h>

class MReactionMapPrivate;

//! \internal

/*!
 * @class MReactionMapConnection
 *
 * @state Stable
 * @copyright Nokia Corporation
 * @license Nokia Open Source
 * @osslibrary Qt 4.6
 * @scope Private
 */
class MReactionMapConnection : public QObject
{
    Q_OBJECT

    // Only MReactionMap can create and delete MReactionMapConnection
    explicit MReactionMapConnection(qint64 pid, QObject *parent = 0);
    virtual ~MReactionMapConnection();

    static MReactionMapConnection *instance();

    friend class MReactionMapPrivate;

public:
    void addReactionMap(MReactionMapPrivate *reactionMap);
    void removeReactionMap(MReactionMapPrivate *reactionMap);

    void updateReactionMapWindowId(MReactionMapPrivate *reactionMap);

signals:
    void disconnected();
    void connectionFailed();

public slots:
    void onConnected();
    void onSocketError(QLocalSocket::LocalSocketError socketError);
    void onSocketReadyRead();
    void connectToMeegoFeedbackd();

protected:
    void tryReconnect();
    void sendReactionMapData(MReactionMapPrivate *reactionMap,
                             quint8 requestType);
    void clearReactionMapData();
    void getShmInfo();

    enum ReactionMapConnectionState {
        // Initial state. Trying to establish socket connection
        // with meegofeedbackd.
        StateConnecting,

        // Socket connection is established.
        StateConnected,

        // Some unrecoverable error has happened. There's no way out of this
        // state.
        StateFailed
    } state;

    qint64 pid;
    QLocalSocket socket;

    // Number of reconnection attempts that have consecutively failed.
    // This value is zeroed when a connection is successfully established.
    int failedReconnections;

    // Contains the intervals, in milliseconds, to wait before attempting
    // to reconnect to meegofeedbackd daemon.
    // list[0] is the time to wait before trying to reconnect for the first time.
    // If the first attempt fails then it waits list[1] milliseconds before attempting
    // to reconnect for the second time and so on.
    // It gives up trying to reconnect when failedReconnections == list.size()
    QList<int> reconnectionIntervalsList;

    quint32 identifierCount;
    QMap<MReactionMapPrivate *, quint32> reactionMaps;

    static MReactionMapConnection *reactionMapConnection;
};

//! \internal_end

#endif
