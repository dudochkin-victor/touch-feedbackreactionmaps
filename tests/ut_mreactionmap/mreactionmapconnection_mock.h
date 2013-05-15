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

#ifndef MREACTIONMAPCONNECTIONMOCK_H
#define MREACTIONMAPCONNECTIONMOCK_H

#include <QObject>
#include <QMap>

#include <mfcommondata.h>

class MReactionMapPrivate;

class MReactionMapConnection : public QObject
{
    Q_OBJECT

public:
    MReactionMapConnection(qint64 pid, QObject *parent = 0);
    virtual ~MReactionMapConnection();

    static MReactionMapConnection *instance();

    friend class MReactionMapPrivate;

    void addReactionMap(MReactionMapPrivate *reactionMap);
    void removeReactionMap(MReactionMapPrivate *reactionMap);

    void updateReactionMapWindowId(MReactionMapPrivate *reactionMap);

    bool generateSharedMemory(MReactionMapPrivate *reactionMap);
    void deleteSharedMemory(MReactionMapPrivate *reactionMap);

    void setSharedMemoryData(MReactionMapPrivate *reactionMap);
    void disconnect(bool failedConnection);

    struct mapData {
        QString shmPath;
        int sharedMemoryId;
        void *sharedMemoryAddress;
        uchar *image;
        MfPaletteEntry *palette;
        quint32 topLevelWidgetWinId;
    };

    qint64 pid;
    int shmCount;

    QMap<MReactionMapPrivate *, struct mapData> reactionMaps;

    static MReactionMapConnection *reactionMapConnection;

signals:
    void disconnected();
    void connectionFailed();

};

#endif
