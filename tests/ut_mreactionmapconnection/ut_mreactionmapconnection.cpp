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

#include "ut_mreactionmapconnection.h"

#include "mreactionmapconnection.h"
#include "mreactionmap_p.h"

#include <QDesktopWidget>

QTEST_MAIN(Ut_MReactionMapConnection)

QLocalSocket *Socket;

Ut_MReactionMapConnection::Ut_MReactionMapConnection(QObject *parent)
    : QObject(parent)
{
}

void Ut_MReactionMapConnection::init()
{
}

void Ut_MReactionMapConnection::cleanup()
{
    // Can't be done because of these are private functions
//    delete MReactionMapConnection::reactionMapConnection;
//    MReactionMapConnection::reactionMapConnection = NULL;
}

void Ut_MReactionMapConnection::addReactionMap()
{
    MReactionMapPrivate *reactionMap;
    qint64 sentPid;
    quint8 sentRequest;
    quint32 sentIdentifier;
    QString sentApplicationName;
    quint32 sentTopLevelWinId;
    QLocalSocket *testSocket;

    reactionMap = new MReactionMapPrivate(0, "fooMap", 0);

    // Make sure that instance was created
    QVERIFY(reactionMap->reactionMapConnection != 0);

    // Verify the communication between client and server
    testSocket = QLocalSocket::instance();
    QDataStream testStream(*testSocket->getWrittenData());

    testStream >> sentPid;
    testStream >> sentRequest;
    testStream >> sentIdentifier;
    testStream >> sentApplicationName;
    testStream >> sentTopLevelWinId;

    // Compare the values
    QCOMPARE(sentPid, QCoreApplication::applicationPid());
    QCOMPARE(sentRequest, static_cast<quint8>(MF_REQUEST_ADD));
    QCOMPARE(sentIdentifier, reactionMap->identifier());
    QCOMPARE(sentApplicationName, reactionMap->applicationName);
    // Initial window ID of mocked MReactionMapPrivate is always 0xf00baa
    QCOMPARE(sentTopLevelWinId, reactionMap->topLevelWidgetWinId);

    delete reactionMap;
    reactionMap = 0;
}

void Ut_MReactionMapConnection::reactionMapInformation()
{
    MReactionMapPrivate *reactionMap;
    QLocalSocket *testSocket;

    reactionMap = new MReactionMapPrivate(0, "fooMap", 0);

    testSocket = QLocalSocket::instance();

    // Make sure there is no shared memory information yet received
    QCOMPARE(reactionMap->shmInfo(), QString());

    // Have local socket to send shared memory information
    testSocket->sendSharedMemoryInfo(reactionMap->identifier(), QString("/foo/bar/shm"));

    // Make sure there shared memory information is received
    QCOMPARE(reactionMap->shmInfo(), QString("/foo/bar/shm"));

    delete reactionMap;
    reactionMap = 0;
}

void Ut_MReactionMapConnection::updateReactionMapWindowId()
{
    MReactionMapPrivate *reactionMap;
    quint8 sentRequest;
    quint32 sentIdentifier;
    quint32 sentTopLevelWinId;
    QLocalSocket *testSocket;

    reactionMap = new MReactionMapPrivate(0, "fooMap", 0);

    // Make sure that instance was created
    QVERIFY(reactionMap->reactionMapConnection != 0);

    // Verify the communication between client and server
    testSocket = QLocalSocket::instance();

    // Clear all initial communication
    testSocket->clearReceivedData();

    // Update window ID of top level widget
    reactionMap->setTopLevelWinId(0xdeadbeef);

    // Get the data
    QDataStream testStream(*testSocket->getWrittenData());

    // Read the data client sent to server
    testStream >> sentRequest;
    testStream >> sentIdentifier;
    testStream >> sentTopLevelWinId;

    // Make sure the data is correct
    QCOMPARE(sentRequest, static_cast<quint8>(MF_REQUEST_UPDATE));
    QCOMPARE(sentIdentifier, reactionMap->identifier());
    QCOMPARE(sentTopLevelWinId, reactionMap->topLevelWidgetWinId);

    delete reactionMap;
    reactionMap = 0;
}

void Ut_MReactionMapConnection::removeReactionMap()
{
    MReactionMapPrivate *reactionMap;
    quint8 sentRequest;
    quint32 sentIdentifier;
    quint32 expectedIdentifier;
    QLocalSocket *testSocket;

    reactionMap = new MReactionMapPrivate(0, "fooMap", 0);

    // Make sure that instance was created
    QVERIFY(reactionMap->reactionMapConnection != 0);

    // Verify the communication between client and server
    testSocket = QLocalSocket::instance();

    // Clear all initial communication
    testSocket->clearReceivedData();

    // Remove the reaction map
    expectedIdentifier = reactionMap->identifier();
    delete reactionMap;
    reactionMap = 0;

    // Get the data
    QDataStream testStream(*testSocket->getWrittenData());

    // Read the data client sent to server
    testStream >> sentRequest;
    testStream >> sentIdentifier;

    // Make sure the data is correct
    QCOMPARE(sentRequest, static_cast<quint8>(MF_REQUEST_DELETE));
    QCOMPARE(sentIdentifier, expectedIdentifier);
}

void Ut_MReactionMapConnection::reconnect()
{
    MReactionMapPrivate *reactionMap;
    qint64 sentPid;
    quint8 sentRequest;
    quint32 sentIdentifier;
    QString sentApplicationName;
    quint32 sentTopLevelWinId;
    QLocalSocket *testSocket;

    reactionMap = new MReactionMapPrivate(0, "fooMap", 0);

    // Make sure that instance was created
    QVERIFY(reactionMap->reactionMapConnection != 0);

    // Verify the communication between client and server
    testSocket = QLocalSocket::instance();

    // Clear all initial communication
    testSocket->clearReceivedData();

    // Mock a sudden disconnect from server and refuse to connect
    // on the first reconnection try
    testSocket->setFailConnectCount(1);
    testSocket->suddenDisconnect();

    QCOMPARE(testSocket->state(), QLocalSocket::UnconnectedState);

    // Wait for 700ms, MReactionMapConnection should attempt
    // the first reconnection after 600ms
    QTest::qWait(700);

    // Check that socket is still unconnected and there has been a
    // reconnection attempt
    QCOMPARE(testSocket->state(), QLocalSocket::UnconnectedState);
    QCOMPARE(testSocket->getAttemptedConnectCount(), 1);

    // Wait for 1000ms, MReactionMapConnection should attempt
    // the second reconnection after 1600ms
    QTest::qWait(1000);

    // Check that socket is connected again
    QCOMPARE(testSocket->state(), QLocalSocket::ConnectedState);

    // Get written data
    QDataStream testStream(*testSocket->getWrittenData());

    testStream >> sentPid;
    testStream >> sentRequest;
    testStream >> sentIdentifier;
    testStream >> sentApplicationName;
    testStream >> sentTopLevelWinId;

    // Compare the values
    QCOMPARE(sentPid, QCoreApplication::applicationPid());
    QCOMPARE(sentRequest, static_cast<quint8>(MF_REQUEST_ADD));
    QCOMPARE(sentIdentifier, reactionMap->identifier());
    QCOMPARE(sentApplicationName, reactionMap->applicationName);
    // Initial window ID of mocked MReactionMapPrivate is always 0xf00baa
    QCOMPARE(sentTopLevelWinId, reactionMap->topLevelWidgetWinId);

    delete reactionMap;
    reactionMap = 0;
}


