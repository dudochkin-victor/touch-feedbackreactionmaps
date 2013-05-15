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

#include "qlocalsocket_mock.h"

int QLocalSocket::failConnectCount = 0;
QLocalSocket* QLocalSocket::self = 0;

QLocalSocket::QLocalSocket(QObject *parent) : QIODevice(parent)
{
    socketState = UnconnectedState;
    attemptedConnectCount = 0;
    self = this;
}

QLocalSocket::~QLocalSocket()
{
}

qint64 QLocalSocket::bytesAvailable() const
{
    return bytesToBeRead.size() + QIODevice::bytesAvailable();
}

void QLocalSocket::connectToServer(const QString &name, OpenMode openMode)
{
    Q_UNUSED(name);
    Q_UNUSED(openMode);

    attemptedConnectCount++;

    if (failConnectCount > 0) {
        failConnectCount--;
        emit error(ServerNotFoundError);
    } else {
        this->open(QIODevice::ReadWrite);
        socketState = ConnectedState;
        emit connected();
    }
}

void QLocalSocket::sendSharedMemoryInfo(quint32 identifier, QString shmInfo)
{
    QBuffer readBuffer(&bytesToBeRead);
    readBuffer.open(QIODevice::WriteOnly);
    QDataStream readStream(&readBuffer);

    readStream << identifier;
    readStream << shmInfo;

    emit readyRead();
}

void QLocalSocket::suddenDisconnect()
{
    socketState = UnconnectedState;
    bytesWritten.clear();
    bytesToBeRead.clear();
    this->close();
    attemptedConnectCount = 0;
    emit error(UnknownSocketError);
}

void QLocalSocket::clearReceivedData()
{
    bytesWritten.clear();
}

void QLocalSocket::disconnectFromServer()
{
}

bool QLocalSocket::flush()
{
    return 0;
}

QLocalSocket::LocalSocketState QLocalSocket::state() const
{
    return socketState;
}

qint64 QLocalSocket::readData(char* data, qint64 maxSize)
{
    qint64 readBytes;
    QBuffer readBuffer(&bytesToBeRead);
    readBuffer.open(QIODevice::ReadOnly);
    QDataStream readStream(&readBuffer);

    readBytes = readStream.readRawData(data, maxSize);

    // Remove read bytes from buffer so they won't be read again
    // in case of short read
    bytesToBeRead = bytesToBeRead.remove(0, readBytes);

    return readBytes;
}

qint64 QLocalSocket::writeData(const char* data, qint64 maxSize)
{
    bytesWritten.append(data, maxSize);

    return 0;
}

QByteArray* QLocalSocket::getWrittenData()
{
    return &bytesWritten;
}

int QLocalSocket::getAttemptedConnectCount()
{
    return attemptedConnectCount;
}

void QLocalSocket::setFailConnectCount(int count)
{
    failConnectCount = count;
}


QLocalSocket* QLocalSocket::instance()
{
    return self;
}


