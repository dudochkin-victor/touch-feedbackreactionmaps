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

#include "mfserver.h"
#include "mfconnection.h"
#include <mfutil.h>

#include <sys/stat.h>
#include <sys/shm.h>

#include <QDebug>
#include <QFile>
#include <QDir>


MfServer::MfServer(MfReactionMapStack *stack, QObject *parent)
    : QObject(parent), stack(stack)
{
}

bool MfServer::init()
{
    bool ok;
    QString tempDir = mfTempDir();
    QString socketFilePath;

    if (tempDir.size() == 0) {
        return false;
    }

    removePreExistingSharedMemory();

    socketFilePath = tempDir;
    socketFilePath.append("/reactionmaps.sock");

    ok = connect(&socketServer, SIGNAL(newConnection()), SLOT(onNewConnection()));
    if (!ok) return false;

    ok = socketServer.listen(socketFilePath);
    if (!ok) {
        // Remove temp file and try again.
        if (QFile::remove(socketFilePath)) {
            ok = socketServer.listen(socketFilePath);
        }
    }
    if (!ok) {
        qCritical() << "MfServer: Unable to listen for connections on"
                    << socketFilePath;
        return false;
    }

    // Enable other users to connect to this socket server
    if (chmod(socketFilePath.toAscii().constData(),
              S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH) == -1) {
        qCritical() << "MfServer: Unable to set socket file permissions";
        return false;
    }

    return true;
}

void MfServer::removePreExistingSharedMemory()
{
    int result;
    char proc_id = 'C';
    QDir oldTempFiles(mfTempDir());
    QStringList nameFilter;

    nameFilter << "rm*";
    oldTempFiles.setNameFilters(nameFilter);
    oldTempFiles.setFilter(QDir::Files);

    foreach(const QString &tempFile, oldTempFiles.entryList()) {
        QString fullPath;
        key_t key;
        int shm_id;

        fullPath = oldTempFiles.filePath(tempFile);

        key = ftok(fullPath.toAscii().constData(), proc_id);
        shm_id = shmget(key, 0, S_IRUSR | S_IWUSR);

        if (shm_id == -1) {
            qWarning() << "MfServer: Unable to remove shared memory segment indicated by file:"
                       << fullPath << "(no ID)";
        } else {
            result = shmctl(shm_id, IPC_RMID, NULL);

            if (result == -1) {
                qCritical() << "MfServer: Failed to remove shared memory segment. ID:" << hex << shm_id;
            } else {
                // We're only ok to remove the temporary file if we successfully
                // destroy its corresponding shared memory. Otherwise it should
                // stay there as a reminder that the shared memory is still hanging around.
                if (!QFile::remove(fullPath)) {
                    qCritical() << "MfServer: failed to remove" << fullPath;
                }
            }
        }
    }
}

void MfServer::onNewConnection()
{
    QLocalSocket *socket;
    qDebug("MfServer: Connection arrived.");

    socket = socketServer.nextPendingConnection();

    new MfConnection(socket, stack, this);
}
