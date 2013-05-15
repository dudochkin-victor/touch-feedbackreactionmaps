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

#include "mfsettings.h"
#include <sys/shm.h>
#include <fcntl.h>

#define SHAREDMEMORYPATH "/tmp/ut_mreactionmap_"

MReactionMapConnection *MReactionMapConnection::reactionMapConnection = 0;

MReactionMapConnection::MReactionMapConnection(qint64 pid, QObject *parent)
    : QObject(parent), pid(pid)
{
    if (reactionMapConnection != 0) {
        qFatal("Trying to create a second instance of MReactionMapConnection!");
    }
}

MReactionMapConnection::~MReactionMapConnection()
{
    QMap<MReactionMapPrivate *, struct mapData>::iterator i = reactionMaps.begin();
    while (i != reactionMaps.end()) {
        if (i.value().shmPath.isEmpty() == false) {
            deleteSharedMemory(i.key());
        }
         i = reactionMaps.erase(i);
    }
}

MReactionMapConnection* MReactionMapConnection::instance()
{
    if (reactionMapConnection == 0) {
        reactionMapConnection = new MReactionMapConnection(QCoreApplication::applicationPid ());
    }

    return reactionMapConnection;
}

bool MReactionMapConnection::generateSharedMemory(MReactionMapPrivate *reactionMap)
{
    int sharedMemoryId;
    void *sharedMemoryAddress;
    uchar *image;
    MfPaletteEntry *palette;
    key_t key;
    char proc_id = 'C';
    int tempFileFd;
    int size = MfSettings::reactionMapWidth()*MfSettings::reactionMapHeight()+256*sizeof(MfPaletteEntry); // Each pixel takes 8 bits+reaction palette
    QString shmPath;

    int flags = S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | IPC_CREAT;

    shmPath.append(SHAREDMEMORYPATH);
    shmPath.append(QString::number(shmCount));
    shmCount++;

    tempFileFd = creat(shmPath.toAscii().constData(),
               S_IRUSR | S_IWUSR | S_IROTH);
    if (tempFileFd == -1) {
        qCritical() << "MReactionMapConnection: Unable to create file for fake reaction map"
                    << shmPath;
        return false;
    }

    key = ftok(shmPath.toAscii().constData(), proc_id);
    sharedMemoryId = shmget(key, size, flags);

    if (sharedMemoryId == -1) {
        qCritical() << "MReactionMapConnection: Unable to create shared memory for fake reaction map"
                    << shmPath;
        return false;
    }

    sharedMemoryAddress = shmat(sharedMemoryId, 0, 0);
    if ((qptrdiff)sharedMemoryAddress == -1) {
        qCritical() << "MReactionMapConnection: Unable to get shared memory address for"
                    << shmPath;
        return false;
    }

    // Make our image for that shared memory
    image = static_cast<uchar*>(sharedMemoryAddress);
    memset(image, 0, MfSettings::reactionMapWidth()*MfSettings::reactionMapHeight());
    // An index with feedback codes
    palette = reinterpret_cast<MfPaletteEntry*>(&image[MfSettings::reactionMapWidth()*MfSettings::reactionMapHeight()]);
    memset(palette, 0, 256*sizeof(MfPaletteEntry));

    reactionMaps[reactionMap].shmPath = shmPath;
    reactionMaps[reactionMap].image = image;
    reactionMaps[reactionMap].sharedMemoryId = sharedMemoryId;
    reactionMaps[reactionMap].palette = palette;
    reactionMaps[reactionMap].sharedMemoryAddress = sharedMemoryAddress;

    return true;
}

void MReactionMapConnection::deleteSharedMemory(MReactionMapPrivate *reactionMap) {
    int result;
    void *sharedMemoryAddress;
    int sharedMemoryId;
    QString shmPath;

    sharedMemoryAddress = reactionMaps[reactionMap].sharedMemoryAddress;
    sharedMemoryId = reactionMaps[reactionMap].sharedMemoryId;
    shmPath = reactionMaps[reactionMap].shmPath;

    if (sharedMemoryAddress) {
        result = shmdt(sharedMemoryAddress);
        if (result == -1) {
            qWarning("MReactionMapConnection: Failed to detach shared memory segment.");
        } else {
            sharedMemoryAddress = 0;
        }
    }

    if (sharedMemoryId != -1) {
        shmid_ds buf;

        result = shmctl(sharedMemoryId, IPC_STAT, &buf);
        if (result == -1) {
            qWarning("MReactionMapConnection: Failed to get shm info.");
        } else if (buf.shm_nattch > 0) {
            qWarning() << "MReactionMapConnection:" << buf.shm_nattch
                       << "processes still attached to shm.";
        }

        result = shmctl(sharedMemoryId, IPC_RMID, NULL);
        if (result == -1) {
            qCritical("MReactionMapConnection: Failed to remove shared memory segment.");
        } else {
            // We're only ok to remove the temporary file if we successfully
            // destroy its corresponding shared memory. Otherwise it should
            // stay there as a reminder that the shared memory is still hanging around.
            if (!QFile::remove(shmPath)) {
                qWarning() << "MReactionMapConnection: failed to remove" << shmPath;
            }
        }

        sharedMemoryId = -1;
    }
}

void MReactionMapConnection::setSharedMemoryData(MReactionMapPrivate *reactionMap)
{
    if (reactionMaps.contains(reactionMap) == true &&
        reactionMaps[reactionMap].shmPath.isEmpty() == false) {
        reactionMap->setShmInfo(reactionMaps[reactionMap].shmPath);
    }
}

void MReactionMapConnection::addReactionMap(MReactionMapPrivate *reactionMap)
{
    if (reactionMaps.contains(reactionMap) == false) {
        struct mapData data;

        data.topLevelWidgetWinId = reactionMap->topLevelWidgetWinId;

        // Insert reaction map to reaction map map
        reactionMaps.insert(reactionMap, data);

        // Create shared memory for this reaction map
        generateSharedMemory(reactionMap);
    } else {
        qFatal("Trying to add the same reaction map second time!");
    }
}

void MReactionMapConnection::removeReactionMap(MReactionMapPrivate *reactionMap)
{
    if (reactionMaps.contains(reactionMap) == true &&
        reactionMaps[reactionMap].shmPath.isEmpty() == false) {
        deleteSharedMemory(reactionMap);
    }
    reactionMaps.remove(reactionMap);
}

void MReactionMapConnection::updateReactionMapWindowId(MReactionMapPrivate *reactionMap)
{
    if (reactionMaps.contains(reactionMap) == true) {
        reactionMaps[reactionMap].topLevelWidgetWinId = reactionMap->topLevelWidgetWinId;
    }
}

void MReactionMapConnection::disconnect(bool failedConnection)
{
    // Clear all reaction map data.
    QMap<MReactionMapPrivate *, struct mapData>::iterator i = reactionMaps.begin();
    while (i != reactionMaps.end()) {
        if (i.value().shmPath.isEmpty() == false) {
            deleteSharedMemory(i.key());
        }
         i = reactionMaps.erase(i);
    }

    emit disconnected();

    // Send failed connection if so requested
    if (failedConnection == true) {
        emit connectionFailed();
    }
}

