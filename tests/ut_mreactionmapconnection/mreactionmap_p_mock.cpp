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

#include "mreactionmap_p_mock.h"

MReactionMapPrivate::MReactionMapPrivate(QWidget *topLevelWidget,
                                             const QString& applicationName,
                                             QObject *parent)
    : QObject(parent), topLevelWidgetWinId(0xf00baa), applicationName(applicationName)
{
    Q_UNUSED(topLevelWidget);

    atConnectionFailedState = false;

    reactionMapConnection = MReactionMapConnection::instance();

    connect(reactionMapConnection, SIGNAL(disconnected()), SLOT(onDisconnected()));
    connect(reactionMapConnection, SIGNAL(connectionFailed()), SLOT(onConnectionFailed()));

    reactionMapConnection->addReactionMap(this);
}

MReactionMapPrivate::~MReactionMapPrivate()
{
    reactionMapConnection->removeReactionMap(this);
}

void MReactionMapPrivate::onConnectionFailed()
{
    atConnectionFailedState = true;
}

void MReactionMapPrivate::onDisconnected()
{
    reactionMapConnection->addReactionMap(this);
}

void MReactionMapPrivate::setShmInfo(QString shmInfo)
{
    receivedShmInfo = shmInfo;
}

quint32 MReactionMapPrivate::identifier()
{
    quint32 ret = 0;

    if (reactionMapConnection->reactionMaps.contains(this) == true) {
        ret  = reactionMapConnection->reactionMaps[this];
    }

    return ret;
}

QString MReactionMapPrivate::shmInfo() const
{
    return receivedShmInfo;
}

void MReactionMapPrivate::setTopLevelWinId(quint32 topLevelWidgetWinId)
{
    this->topLevelWidgetWinId = topLevelWidgetWinId;

    reactionMapConnection->updateReactionMapWindowId(this);
}
