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

#include "ut_mreactionmap.h"

#include "mreactionmap.h"
#include "mreactionmap_p.h"
#include "mreactionmapconnection_mock.cpp"
#include "mfeedbackpalette_mock.cpp"
#include <mfsettings.h>
#include <QDesktopWidget>
#include <QGraphicsRectItem>

class MReactionMapTest : public MReactionMap
{
public:
    MReactionMapTest(QWidget *topLevelWidget, const QString& appIdentifier = QString(), QObject *parent = 0);
    virtual ~MReactionMapTest();

    uchar* getImage();
    quint8 getDrawingValue();
    MReactionMapPrivate* getPrivatePtr();
};

MReactionMapTest::MReactionMapTest(QWidget *topLevelWidget, const QString& appIdentifier,
                                       QObject *parent)
    : MReactionMap(topLevelWidget, appIdentifier, parent)
{
}

uchar* MReactionMapTest::getImage()
{
    return d->image;
}

quint8 MReactionMapTest::getDrawingValue()
{
    return d->drawingValue;
}

MReactionMapPrivate* MReactionMapTest::getPrivatePtr()
{
    return d;
}

MReactionMapTest::~MReactionMapTest()
{
}

QTEST_MAIN(Ut_MReactionMap)

Ut_MReactionMap::Ut_MReactionMap(QObject *parent)
    : QObject(parent)
{
}

void Ut_MReactionMap::init()
{
    scene = new QGraphicsScene(0, 0,
                               8*MfSettings::reactionMapWidth(),
                               8*MfSettings::reactionMapHeight());
    widget1 = new QGraphicsView(scene);
    childOfWidget1 = new QWidget(widget1);
    widget2 = new QGraphicsView(scene);
    reactionMap1 = new MReactionMapTest(widget1, "reactionMap1");
    reactionMap2 = new MReactionMapTest(widget2, "reactionMap2");

    item = new QGraphicsRectItem(10, 10, 100, 50);
    scene->addItem(item);
}

void Ut_MReactionMap::cleanup()
{
    delete reactionMap1;
    reactionMap1 = 0;

    delete reactionMap2;
    reactionMap2 = 0;

    delete widget1;
    widget1 = 0;

    delete widget2;
    widget2 = 0;

    delete item;
    item = 0;

    delete scene;
    scene = 0;

    delete MReactionMapConnection::reactionMapConnection;
    MReactionMapConnection::reactionMapConnection = 0;
}

/*
 * Check that correct built-in feedback names are returned.
 */
void Ut_MReactionMap::getFeedbackNames()
{
    QCOMPARE(MReactionMap::Press, QString("press"));
    QCOMPARE(MReactionMap::Release, QString("release"));
    QCOMPARE(MReactionMap::Transparent, QString("transparent"));
    QCOMPARE(MReactionMap::Inactive, QString(""));
}

/*
 * Check that application name is handled correctly.
 */
void Ut_MReactionMap::applicationName()
{
    MReactionMap *testMap;
    QWidget *testWidget;

    testWidget = new QWidget();

    // A valid applicationName should be passed to MFeedbackPalette as-is
    testMap = new MReactionMap(testWidget, "reactionMapWithValidName");
    QCOMPARE(MFeedbackPalette::latestApplicationName(), QString("reactionMapWithValidName"));
    delete testMap;
    testMap = 0;

    // An invalid application name should be discarded and argv[0] should
    // be used instead with possible preceding path stripped. "actualApplication"
    // comes from mocked QCoreApplication (qcoreapplication_mock.cpp)
    testMap = new MReactionMap(testWidget, "/some/dir/somewhere/reactionMapWithInvalidName");
    QCOMPARE(MFeedbackPalette::latestApplicationName(), QString("actualApplication"));
    delete testMap;
    testMap = 0;

    // If there is no applicationName defined, should be used with possible
    // preceding path stripped. "actualApplication" comes from mocked
    // QCoreApplication (qcoreapplication_mock.cpp)
    testMap = new MReactionMap(testWidget);
    QCOMPARE(MFeedbackPalette::latestApplicationName(), QString("actualApplication"));
    delete testMap;
    testMap = 0;

    delete testWidget;
    testWidget = 0;
}

/*
 * Returns true if given rectangle is filled with given value and all other
 * area is filled with inactive value. Rectangle coordinates are given in
 * reaction map resolution.
 */
bool Ut_MReactionMap::checkArea(MReactionMapTest *reactionMap, uchar value,
                                  int x, int y, int width, int height)
{
    uchar *image;
    int mapWidth = MfSettings::reactionMapWidth();
    int mapHeight = MfSettings::reactionMapHeight();
    bool result = true;

    image = reactionMap->getImage();

    for (int i = 0; i < mapHeight; i++) {
        for (int j = 0; j < mapWidth; j++) {
            if (j >= x && j < (x+width) &&
                i >= y && i < (y+height)) {
                // Pixel is inside the given rectangle
                if (image[i*mapWidth+j] != value) {
                    result = false;
                }
            } else {
                // Pixel is outside the given rectangle
                if (image[i*mapWidth+j] != 0) {
                    result = false;
                }
            }
        }
    }

    return result;
}

/*
 * Check that instance returns correct reaction map for each widget.
 */
void Ut_MReactionMap::getInstances()
{
    MReactionMap *instance;

    // Check the query with invalid parameter
    QCOMPARE(MReactionMap::instance(NULL), (MReactionMap*)NULL);

    // Check that correct instance is returned for top-level widgets
    instance = MReactionMap::instance(widget1);
    QCOMPARE(instance, reactionMap1);

    instance = MReactionMap::instance(widget2);
    QCOMPARE(instance, reactionMap2);

    // Check that correct instance is returned for a child of
    // top-level widget
    instance = MReactionMap::instance(childOfWidget1);
    QCOMPARE(instance, reactionMap1);
}

/*
 * Set inactive, reactive, transparent and user defined drawing value and
 * check that correct drawing value is set each time.
 */
void Ut_MReactionMap::getDrawingValues()
{
    reactionMap1->setReactiveDrawingValue();
    QCOMPARE(reactionMap1->getDrawingValue(), static_cast<quint8>(2));

    reactionMap1->setInactiveDrawingValue();
    QCOMPARE(reactionMap1->getDrawingValue(), static_cast<quint8>(0));

    reactionMap1->setTransparentDrawingValue();
    QCOMPARE(reactionMap1->getDrawingValue(), static_cast<quint8>(1));

    reactionMap1->setDrawingValue("foo", "bar");
    QCOMPARE(reactionMap1->getDrawingValue(), static_cast<quint8>(3));
}

/*
 * Check that transform() returns the correct transformation matrix that maps
 * screen coordinates to reaction map coordinates.
 */
void Ut_MReactionMap::getTransform()
{
    QTransform transform;
    QRect reactionMapRect;
    QDesktopWidget *desktop = QApplication::desktop();
    QRect screenRect = desktop->screenGeometry();

    transform = reactionMap1->transform();

    reactionMapRect = transform.mapRect(screenRect);

    // Resulting rectangle should have reaction map dimensions
    QCOMPARE(reactionMapRect.width(), MfSettings::reactionMapWidth());
    QCOMPARE(reactionMapRect.height(), MfSettings::reactionMapHeight());
}

/*
 * Draw a rectangle using the original transform matrix and check that
 * the resulting reaction map is correct.
 */
void Ut_MReactionMap::drawRectangle1()
{
    QTransform transform;
    QRect reactionMapRect;
    QRect clientRect(20, 30, 40, 50);

    reactionMap1->setReactiveDrawingValue();
    reactionMap1->fillRectangle(clientRect.x(), clientRect.y(),
                                clientRect.width(), clientRect.height());

    transform = reactionMap1->transform();
    reactionMapRect = transform.mapRect(clientRect);
    QCOMPARE(checkArea(reactionMap1, 2, reactionMapRect.x(), reactionMapRect.y(),
                       reactionMapRect.width(), reactionMapRect.height()), true);
}

/*
 * Draw a rectangle using the 1:1 transform matrix and check that
 * the resulting reaction map is correct.
 */
void Ut_MReactionMap::drawRectangle2()
{
    QRect clientRect(0, 0, 100, 50);

    // Set 1:1 mapping of coordinates
    reactionMap1->setTransform(QTransform());
    reactionMap1->setTransparentDrawingValue();
    reactionMap1->fillRectangle(clientRect);

    QCOMPARE(checkArea(reactionMap1, 1, clientRect.x(), clientRect.y(),
                       clientRect.width(), clientRect.height()), true);
}

/*
 * Draw a rectangle using custom transform matrix and custom color and check
 * that the resulting reaction map is correct.
 */
void Ut_MReactionMap::drawRectangle3()
{
    QTransform transform;
    QRectF reactionMapRect;
    QRectF clientRect(0, 0, 1, 1);

    transform = transform.scale(10, 10);
    transform = transform.translate(1, 1);

    reactionMap1->setTransform(transform);
    reactionMap1->setDrawingValue("foo", "bar");
    reactionMap1->fillRectangle(clientRect);

    reactionMapRect = transform.mapRect(clientRect);
    QCOMPARE(checkArea(reactionMap1, 3, reactionMapRect.x(), reactionMapRect.y(),
                       reactionMapRect.width(), reactionMapRect.height()), true);
}

/*
 * Draw a rectangle with temporary drawing value and check that the rectangle is
 * correct and that the original drawing value is retained.
 */
void Ut_MReactionMap::drawRectangle4()
{
    QTransform transform;
    QRect reactionMapRect;
    QRect clientRect(20, 20, 50, 50);

    reactionMap1->setDrawingValue("foo", "bar");
    reactionMap1->fillRectangle(clientRect, "press", "release");

    transform = reactionMap1->transform();
    reactionMapRect = transform.mapRect(clientRect);
    QCOMPARE(checkArea(reactionMap1, 2, reactionMapRect.x(), reactionMapRect.y(),
                       reactionMapRect.width(), reactionMapRect.height()), true);
    QCOMPARE(reactionMap1->getDrawingValue(), static_cast<quint8>(3));
}

/*
 * Draw a rectangle with temporary drawing value and custom tranformation matrix
 * and check that the rectangle is correct and that the original drawing value is
 * retained.
 */
void Ut_MReactionMap::drawRectangle5()
{
    QTransform transform;
    QRectF reactionMapRect;
    QRectF clientRect(10, 10, 20, 20);

    transform = transform.scale(2, 2);
    transform = transform.translate(1, 1);

    reactionMap1->setTransform(transform);
    reactionMap1->setInactiveDrawingValue();
    reactionMap1->fillRectangle(clientRect, "foo", "bar");

    reactionMapRect = transform.mapRect(clientRect);
    QCOMPARE(checkArea(reactionMap1, 3, reactionMapRect.x(), reactionMapRect.y(),
                       reactionMapRect.width(), reactionMapRect.height()), true);
    QCOMPARE(reactionMap1->getDrawingValue(), static_cast<quint8>(0));
}

/*
 * Draw a rectangle that is totally out of bounds and see that no rectangle
 * is drawn.
 */
void Ut_MReactionMap::drawInvalidRectangle1()
{
    QRectF clientRect(1000000, 10000000, 1000000, 1000000);

    // Set 1:1 transform
    reactionMap1->setTransform(QTransform());
    reactionMap1->setReactiveDrawingValue();
    reactionMap1->fillRectangle(clientRect);

    QCOMPARE(checkArea(reactionMap1, 0, 0, 0, 0, 0), true);
}

/*
 * Draw a rectangle that is partially out of bounds and check that only
 * intersecting rectangle is drawn.
 */
void Ut_MReactionMap::drawInvalidRectangle2()
{
    QRectF clientRect(MfSettings::reactionMapWidth()/2, MfSettings::reactionMapHeight()/2,
                      MfSettings::reactionMapWidth()*2, MfSettings::reactionMapHeight()*2);

    // Set 1:1 transform
    reactionMap1->setTransform(QTransform());
    reactionMap1->setReactiveDrawingValue();
    reactionMap1->fillRectangle(clientRect);

    QCOMPARE(checkArea(reactionMap1, 2,
                       MfSettings::reactionMapWidth()/2, MfSettings::reactionMapHeight()/2,
                       MfSettings::reactionMapWidth()/2, MfSettings::reactionMapHeight()/2), true);
}

/*
 * Draw a bounding rectangle of a QGraphicsItem and check that
 * the resulting reaction map is correct.
 */
void Ut_MReactionMap::drawBoundingRectangle1()
{
    QTransform transform;
    QRectF reactionMapRect;

    item->setPos(50,25);

    reactionMap1->setReactiveDrawingValue();
    reactionMap1->fillItemBoundRect(item);

    transform = item->sceneTransform() * reactionMap1->transform() * widget1->viewportTransform();
    reactionMapRect = transform.mapRect(item->rect());

    QCOMPARE(checkArea(reactionMap1, 2, qRound(reactionMapRect.x()), qRound(reactionMapRect.y()),
                       qRound(reactionMapRect.width()), qRound(reactionMapRect.height())), true);

    QCOMPARE(reactionMap2->getImage(), (uchar*)NULL);
}

/*
 * Draw a bounding rectangle of a QGraphicsItem and check that
 * the resulting reaction map is correct. Also check that the
 * temporary drawing value is not permanently stored.
 */
void Ut_MReactionMap::drawBoundingRectangle2()
{
    QTransform transform;
    QRectF reactionMapRect;

    item->setPos(5,5);

    widget2->scale(2,2);
    widget2->centerOn(0,0);

    reactionMap2->setReactiveDrawingValue();
    reactionMap2->fillItemBoundRect(item, "foo", "bar");

    transform = item->sceneTransform() * reactionMap2->transform() * widget2->viewportTransform();
    reactionMapRect = transform.mapRect(item->rect());

    QCOMPARE(checkArea(reactionMap2, 3, qRound(reactionMapRect.x()), qRound(reactionMapRect.y()),
                       qRound(reactionMapRect.width()), qRound(reactionMapRect.height())), true);

    QCOMPARE(reactionMap1->getImage(), (uchar*)NULL);

    QCOMPARE(reactionMap2->getDrawingValue(), static_cast<quint8>(2));
}

/*
 * Check that reaction map width and height are reported correctly.
 */
void Ut_MReactionMap::dimensions()
{
    QCOMPARE(reactionMap1->width(), MfSettings::reactionMapWidth());
    QCOMPARE(reactionMap1->height(), MfSettings::reactionMapHeight());
}

/*
 * Draw on a reaction map, clear it and check that reaction map is actually cleared.
 */
void Ut_MReactionMap::clear()
{
    QRect clientRect(20, 30, 40, 50);

    reactionMap1->setReactiveDrawingValue();
    reactionMap1->fillRectangle(clientRect);

    reactionMap1->clear();
    QCOMPARE(checkArea(reactionMap1, 0, 0, 0, 0, 0), true);
}

/*
 * Check that change of windowID is handled correctly.
 */
void Ut_MReactionMap::windowIdChange()
{
    quint32 oldWindowId, newWindowId;

    MReactionMapConnection *testConnection;

    testConnection = MReactionMapConnection::instance();

    // Get current window ID
    oldWindowId = testConnection->reactionMaps[reactionMap1->getPrivatePtr()].topLevelWidgetWinId;

    // Make sure window ID's match
    QCOMPARE(oldWindowId, static_cast<quint32>(widget1->effectiveWinId()));

    // Set FramelessWindowHint, this will change the window ID of this widget
    widget1->setWindowFlags(Qt::FramelessWindowHint);

    widget1->show();
    widget1->hide();

    // Get new window ID
    newWindowId = testConnection->reactionMaps[reactionMap1->getPrivatePtr()].topLevelWidgetWinId;

    // Make sure window ID's match
    QCOMPARE(newWindowId, static_cast<quint32>(widget1->effectiveWinId()));

    // Make sure window ID really changed
    QVERIFY(oldWindowId != newWindowId);
}


/*
 * Check that receiving shared memory works as expected.
 */
void Ut_MReactionMap::receiveSharedMemory()
{
    QRect clientRect(0, 0, 100, 50);
    uchar *imagePtr;
    uchar *oldImagePtr;
    MReactionMapConnection *testConnection;

    testConnection = MReactionMapConnection::instance();

    // Set 1:1 mapping of coordinates
    reactionMap1->setTransform(QTransform());
    reactionMap1->setTransparentDrawingValue();
    reactionMap1->fillRectangle(clientRect);

    // Get temporary locally allocated image pointer
    oldImagePtr = reactionMap1->getImage();

    // Check that locally created image is as expected
    QCOMPARE(checkArea(reactionMap1, 1, clientRect.x(), clientRect.y(),
                       clientRect.width(), clientRect.height()), true);

    // Send information about shared memory
    testConnection->setSharedMemoryData(reactionMap1->getPrivatePtr());

    // Get shared memory pointer
    imagePtr = reactionMap1->getImage();

    // Check that shared memory image is the same as locally created was
    QCOMPARE(checkArea(reactionMap1, 1, clientRect.x(), clientRect.y(),
                       clientRect.width(), clientRect.height()), true);

    // Verify that memory pointers have actually changed
    QVERIFY(oldImagePtr != imagePtr);
}

/*
 * Check that disconnection from MFeedbackd works as expected
 * when disconnection happens before getting shared memory information.
 */
void Ut_MReactionMap::disconnect1()
{
    QRect clientRect(0, 0, 100, 50);
    MReactionMapConnection *testConnection;

    testConnection = MReactionMapConnection::instance();

    // Set 1:1 mapping of coordinates
    reactionMap1->setTransform(QTransform());
    reactionMap1->setTransparentDrawingValue();
    reactionMap1->fillRectangle(clientRect);

    // Check that locally created image is as expected
    QCOMPARE(checkArea(reactionMap1, 1, clientRect.x(), clientRect.y(),
                       clientRect.width(), clientRect.height()), true);

    // Disconnect, but do not fail
    testConnection->disconnect(false);

    // Check that image has not changed
    QCOMPARE(checkArea(reactionMap1, 1, clientRect.x(), clientRect.y(),
                       clientRect.width(), clientRect.height()), true);

    // Check that reaction map was added to connections
    QVERIFY(testConnection->reactionMaps.contains(reactionMap1->getPrivatePtr()) == true);
}

/*
 * Check that disconnection from MFeedbackd works as expected
 * when disconnection happens after getting shared memory information.
 */
void Ut_MReactionMap::disconnect2()
{
    QRect clientRect(0, 0, 100, 50);
    MReactionMapConnection *testConnection;

    testConnection = MReactionMapConnection::instance();

    // Set 1:1 mapping of coordinates
    reactionMap1->setTransform(QTransform());
    reactionMap1->setTransparentDrawingValue();
    reactionMap1->fillRectangle(clientRect);

    // Check that locally created image is as expected
    QCOMPARE(checkArea(reactionMap1, 1, clientRect.x(), clientRect.y(),
                       clientRect.width(), clientRect.height()), true);

    // Send information about shared memory
    testConnection->setSharedMemoryData(reactionMap1->getPrivatePtr());

    // Disconnect, but do not fail
    testConnection->disconnect(false);

    // Check that image has not changed
    QCOMPARE(checkArea(reactionMap1, 1, clientRect.x(), clientRect.y(),
                       clientRect.width(), clientRect.height()), true);

    // Check that reaction map was added to connections
    QVERIFY(testConnection->reactionMaps.contains(reactionMap1->getPrivatePtr()) == true);
}

/*
 * Check that disconnection from MFeedbackd works as expected
 * when connection simply fails.
 */
void Ut_MReactionMap::disconnect3()
{
    QRect clientRect(0, 0, 100, 50);
    MReactionMapConnection *testConnection;

    testConnection = MReactionMapConnection::instance();

    // Set 1:1 mapping of coordinates
    reactionMap1->setTransform(QTransform());
    reactionMap1->setTransparentDrawingValue();
    reactionMap1->fillRectangle(clientRect);

    // Check that locally created image is as expected
    QCOMPARE(checkArea(reactionMap1, 1, clientRect.x(), clientRect.y(),
                       clientRect.width(), clientRect.height()), true);

    // Send information about shared memory
    testConnection->setSharedMemoryData(reactionMap1->getPrivatePtr());

    // Disconnect and fail so that reconnection is no longer an option
    testConnection->disconnect(true);

    // Check that image has not changed
    QCOMPARE(checkArea(reactionMap1, 1, clientRect.x(), clientRect.y(),
                       clientRect.width(), clientRect.height()), true);

    // Check that reaction map was added to connections
    QVERIFY(testConnection->reactionMaps.contains(reactionMap1->getPrivatePtr()) == true);
}

/*
 * Check that everything works as expected when disconnection occurs and
 * connection is re-established and new shared memory information is received
 */
void Ut_MReactionMap::reconnect()
{
    QRect clientRect(0, 0, 100, 50);
    MReactionMapConnection *testConnection;

    testConnection = MReactionMapConnection::instance();

    // Set 1:1 mapping of coordinates
    reactionMap1->setTransform(QTransform());
    reactionMap1->setTransparentDrawingValue();
    reactionMap1->fillRectangle(clientRect);

    // Check that locally created image is as expected
    QCOMPARE(checkArea(reactionMap1, 1, clientRect.x(), clientRect.y(),
                       clientRect.width(), clientRect.height()), true);

    // Send information about shared memory
    testConnection->setSharedMemoryData(reactionMap1->getPrivatePtr());

    // Disconnect, but do not fail
    testConnection->disconnect(false);

    // Check that image has not changed
    QCOMPARE(checkArea(reactionMap1, 1, clientRect.x(), clientRect.y(),
                       clientRect.width(), clientRect.height()), true);

    // Check that reaction map was added to connections
    QVERIFY(testConnection->reactionMaps.contains(reactionMap1->getPrivatePtr()) == true);

    // Send information about shared memory
    testConnection->setSharedMemoryData(reactionMap1->getPrivatePtr());

    // Check that image has not changed
    QCOMPARE(checkArea(reactionMap1, 1, clientRect.x(), clientRect.y(),
                       clientRect.width(), clientRect.height()), true);
}

