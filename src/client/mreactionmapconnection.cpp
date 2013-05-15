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

#include "mreactionmapconnection.h"
#include "mreactionmap_p.h"

#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

static const QString gSocketFilePath("/tmp/mfeedbackd/reactionmaps.sock");

MReactionMapConnection *MReactionMapConnection::reactionMapConnection = 0;

MReactionMapConnection::MReactionMapConnection(qint64 pid, QObject *parent)
    : QObject(parent), pid(pid)
{
    if (reactionMapConnection != 0) {
        qFatal("Trying to create a second instance of MReactionMapConnection!");
    }

    connect(&socket, SIGNAL(connected()), SLOT(onConnected()));
    connect(&socket, SIGNAL(error(QLocalSocket::LocalSocketError)),
            SLOT(onSocketError(QLocalSocket::LocalSocketError)));
    connect(&socket, SIGNAL(readyRead()), SLOT(onSocketReadyRead()));

    identifierCount = 0;
    failedReconnections = 0;
    state = StateConnecting;

    // TODO: Load from a config file?
    reconnectionIntervalsList << 600;
    reconnectionIntervalsList << 1000;
    reconnectionIntervalsList << 2000;
    reconnectionIntervalsList << 2000;
    reconnectionIntervalsList << 5000;
    reconnectionIntervalsList << 5000;
    reconnectionIntervalsList << 10000;
    reconnectionIntervalsList << 30000;
    reconnectionIntervalsList << 300000; // 5*60*1000 (five minutes)

    connectToMeegoFeedbackd();
}

MReactionMapConnection::~MReactionMapConnection()
{
}


void MReactionMapConnection::connectToMeegoFeedbackd()
{
    Q_ASSERT(socket.state() == QLocalSocket::UnconnectedState);

    qDebug("MReactionMap: Connecting to meegofeedback-reactionmaps...");
    socket.connectToServer(gSocketFilePath);
}

void MReactionMapConnection::onConnected()
{
    QDataStream socketStream(&socket);

    failedReconnections = 0;

    state = StateConnected;

    // Initialise session by telling server our pid
    socketStream << pid;
    socket.flush();

    // Send all reaction map information to meegofeedbackd
    QMap<MReactionMapPrivate *, quint32>::const_iterator i;
    for (i = reactionMaps.begin(); i != reactionMaps.end(); ++i) {
        sendReactionMapData(i.key(), MF_REQUEST_ADD);
    }
}

void MReactionMapConnection::clearReactionMapData()
{
    // Remove all added reaction maps.
    reactionMaps.clear();

    // Make sure identifiers are correct for possible new connection
    identifierCount = 0;
}

void MReactionMapConnection::onSocketError(QLocalSocket::LocalSocketError socketError)
{
    Q_UNUSED(socketError);

    qDebug("MReactionMapConnection: Socket error");

    switch (state) {
        case StateConnected:
        {
            // Connection suddenly dropped, need to start over.
            state = StateConnecting;

            // Clear all reaction map data.
            clearReactionMapData();

            // Inform reaction maps
            emit disconnected();

            tryReconnect();
            break;
        }

        case StateConnecting:
            // Initial connection failed, simply try again after a while
            failedReconnections++;
            if (failedReconnections < reconnectionIntervalsList.size()) {
                tryReconnect();
            } else {
                // Give up
                state = StateFailed;

                // Clear all reaction map data.
                clearReactionMapData();

                // Inform reaction maps
                emit connectionFailed();
            }
            break;

        case StateFailed:
        default:
            // Do nothing
            break;
    };
}

void MReactionMapConnection::getShmInfo()
{
    if (state == StateConnected) {
        QDataStream socketStream(&socket);
        quint32 identifier;
        QString shmInfo;

        socketStream >> identifier;
        socketStream >> shmInfo;

        if (socketStream.status() != QDataStream::Ok) {
            qCritical() << "MReactionMapConnection: Error reading from local socket, disconnect.";
            // If there was an error with the communication, disconnect from server
            socket.disconnectFromServer();
            state = StateConnecting;

            // Clear all reaction map data.
            clearReactionMapData();

            // Inform reaction maps
            emit disconnected();

            tryReconnect();
            return;
        }

        // Set shared memory information to reaction map in question
        QMap<MReactionMapPrivate *, quint32>::iterator i;
        for (i = reactionMaps.begin(); i != reactionMaps.end(); ++i) {
            if (i.value() == identifier) {
                i.key()->setShmInfo(shmInfo);
                break;
            }
        }
    } else {
        qFatal("MReactionMapConnection: Got data from server unexpectedly!");
    }
}

void MReactionMapConnection::onSocketReadyRead()
{
    while (socket.bytesAvailable() > 0) {
        getShmInfo();
    }
}

void MReactionMapConnection::tryReconnect()
{
    QTimer::singleShot(reconnectionIntervalsList[failedReconnections],
                       this, SLOT(connectToMeegoFeedbackd()));
}

MReactionMapConnection* MReactionMapConnection::instance()
{
    if (reactionMapConnection == 0) {
        reactionMapConnection = new MReactionMapConnection(QCoreApplication::applicationPid ());
    }

    return reactionMapConnection;
}

void MReactionMapConnection::sendReactionMapData(MReactionMapPrivate *reactionMap,
                                                   quint8 requestType)
{
    Q_ASSERT(state == StateConnected);
    QDataStream socketStream(&socket);

    switch (requestType) {
        case MF_REQUEST_ADD:
            // Send all data when requesting a new reaction map.
            socketStream << requestType;
            socketStream << reactionMaps[reactionMap];
            socketStream << reactionMap->applicationName;
            socketStream << reactionMap->topLevelWidgetWinId;
            break;

        case MF_REQUEST_UPDATE:
            // Send only identifier and window ID when updating window ID
            // information.
            socketStream << requestType;
            socketStream << reactionMaps[reactionMap];
            socketStream << reactionMap->topLevelWidgetWinId;
            break;

        case MF_REQUEST_DELETE:
            // Send only identifier when removing reaction map.
            socketStream << requestType;
            socketStream << reactionMaps[reactionMap];
            break;

        default:
            break;
    }

    socket.flush();
}

void MReactionMapConnection::addReactionMap(MReactionMapPrivate *reactionMap)
{
    if (reactionMaps.contains(reactionMap) == 0) {
        // Insert reaction map to reaction map map
        reactionMaps.insert(reactionMap, identifierCount++);

        // Send data to server if connection is established. If connection
        // is not established, data will be sent later.
        if (state == StateConnected) {
            sendReactionMapData(reactionMap, MF_REQUEST_ADD);
        }
    } else {
        qFatal("Trying to add the same reaction map second time!");
    }
}

void MReactionMapConnection::removeReactionMap(MReactionMapPrivate *reactionMap)
{
    if (reactionMaps.contains(reactionMap) != 0 && state == StateConnected) {
        sendReactionMapData(reactionMap, MF_REQUEST_DELETE);
    }
    reactionMaps.remove(reactionMap);
}

void MReactionMapConnection::updateReactionMapWindowId(MReactionMapPrivate *reactionMap)
{
    if (reactionMaps.contains(reactionMap) != 0 && state == StateConnected) {
        sendReactionMapData(reactionMap, MF_REQUEST_UPDATE);
    }
}

