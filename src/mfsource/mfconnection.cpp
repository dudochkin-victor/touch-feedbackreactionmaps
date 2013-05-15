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

#include "mfconnection.h"
#include "mfsession.h"

#include <mfcommondata.h>

#include <QCoreApplication>
#include <QDebug>
#include <QLocalSocket>

#include "mfreactionmap.h"
#include "mfreactionmapstack.h"

MfConnection::MfConnection(QLocalSocket *connection, MfReactionMapStack *stack, QObject *parent)
    : QObject(parent), state(StateClientInit), socket(connection),
      stack(stack), clientPid(0)
{
    // Reparent QLocalSocket to have it deleted once MfConnection gets deleted
    socket->setParent(this);

    connect(socket, SIGNAL(readyRead()), SLOT(readSocketData()));
    connect(socket, SIGNAL(disconnected()), SLOT(deleteLater()));

    reactionMaps.clear();

    if (socket->bytesAvailable() > 0) {
        readSocketData();
    }
}

MfConnection::~MfConnection()
{
    if (clientPid != 0) {
        qDebug() << "MfConnection: Session with client pid"
                 << clientPid << "has ended.";

        // Remove all existing reaction maps
        QMap<quint32, MfReactionMap *>::iterator i = reactionMaps.begin();
        while (i != reactionMaps.end()) {
            stack->remove(i.value());
            delete i.value();
            i = reactionMaps.erase(i);
        }
    } else {
        qDebug() << "MfConnection: Destroyed. Session creation didn't happen.";
    }
}

void MfConnection::readSocketData()
{
    while (socket->bytesAvailable() > 0) {
        if (state == StateClientInit) {
            init();
        } else if (state == StateListening) {
            readRequest();
        } else {
            // Ignore everything
            socket->readAll();
        }
    }
}

void MfConnection::sessionSetupReady()
{
    MfReactionMap *newReactionMap = new MfReactionMap(sessions[0].session, sessions[0].windowId, clientPid, this);

    if (reactionMaps.contains(sessions[0].id) == false &&
        newReactionMap->init() == true) {
        QDataStream socketStream(socket);

        stack->add(newReactionMap);
        reactionMaps.insert(sessions[0].id, newReactionMap);

        // Send back information needed by the client to attach himself to the
        // newly created shared memory
        socketStream << sessions[0].id;
        socketStream << newReactionMap->tempFilePath();

        // Reaction map creation has ended
        qDebug() << "MfConnection: finished adding reaction map for client"
                 << sessions[0].clientName;
        sessions[0].session = NULL;
        sessions.remove(0);
    } else {
        // We are doomed.
        goDoomed();
    }
}

void MfConnection::init()
{
    QDataStream socketStream (socket);

    socketStream >> clientPid;

    state = StateListening;
}

void MfConnection::readRequest()
{
    quint8 requestType;
    QDataStream socketStream(socket);

    socketStream >> requestType;

    switch (requestType) {
        case MF_REQUEST_ADD:
            {
                SessionData newSession;

                // Requested new reaction map
                socketStream >> newSession.id;
                socketStream >> newSession.clientName;
                socketStream >> newSession.windowId;

                newSession.session = new MfSession(newSession.clientName);
                sessions.prepend(newSession);
                connect(newSession.session, SIGNAL(setupReady()), this, SLOT(sessionSetupReady()),
                        Qt::QueuedConnection);
            }
            break;

        case MF_REQUEST_UPDATE:
            {
                quint32 identifier;
                quint32 windowId;

                // Window ID of top level widget has been changed
                socketStream >> identifier;
                socketStream >> windowId;

                if (reactionMaps.contains(identifier) == true) {
                    stack->remove(reactionMaps[identifier]);
                    reactionMaps[identifier]->setWindowId(windowId);
                    stack->add(reactionMaps[identifier]);
                    qDebug() << "MfConnection: finished reconfiguring window ID for reaction map with application name:"
                             << reactionMaps[identifier]->applicationName();
                } else {
                    qWarning("MfConnection: Requested window ID change for reaction map that does not exist.");
                }
            }
            break;

        case MF_REQUEST_DELETE:
            {
                quint32 identifier;

                // Requested to delete a reaction map
                socketStream >> identifier;

                if (reactionMaps.contains(identifier) == true) {
                    stack->remove(reactionMaps[identifier]);
                    qDebug() << "MfConnection: Deleting reaction map with application name:"
                             << reactionMaps[identifier]->applicationName();
                    delete reactionMaps[identifier];
                    reactionMaps.remove(identifier);
                } else {
                    qWarning("MfConnection: Requested deletion for reaction map that does not exist.");
                }
            }
            break;

        default:
            // Unknown communication, we're doomed
            goDoomed();
            break;
    }
}

void MfConnection::goDoomed()
{
    state = StateDoomed;

    deleteLater();
}
