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

#ifndef MREACTIONMAPPRIVATEMOCK_H
#define MREACTIONMAPPRIVATEMOCK_H

#include <QTransform>
#include <QObject>

#include "mreactionmapconnection.h"

class MReactionMapPrivate : public QObject
{
    Q_OBJECT
public:
    MReactionMapPrivate(QWidget *topLevelWidget, const QString& applicationName,
                          QObject *parent);
    virtual ~MReactionMapPrivate();

    void setShmInfo(QString shmInfo);
    QString shmInfo() const;

    void setTopLevelWinId(quint32 topLevelWidgetWinId);
    quint32 identifier();

public slots:
    void onDisconnected();
    void onConnectionFailed();

public:
    quint32 topLevelWidgetWinId;
    QString applicationName;
    QString receivedShmInfo;
    bool atConnectionFailedState;

    MReactionMapConnection *reactionMapConnection;
};

#endif
