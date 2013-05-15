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

#ifndef UT_MREACTIONMAP_H
#define UT_MREACTIONMAP_H

#include <QtTest/QtTest>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>

class MReactionMapTest;
class QGraphicsRectItem;

class Ut_MReactionMap : public QObject
{
    Q_OBJECT
public:
    Ut_MReactionMap(QObject *parent = 0);

private slots:
    void initTestCase(){}
    void cleanupTestCase(){}

    void init();
    void cleanup();

    void getFeedbackNames();
    void applicationName();
    void getInstances();
    void getDrawingValues();
    void getTransform();
    void drawRectangle1();
    void drawRectangle2();
    void drawRectangle3();
    void drawRectangle4();
    void drawRectangle5();
    void drawInvalidRectangle1();
    void drawInvalidRectangle2();
    void drawBoundingRectangle1();
    void drawBoundingRectangle2();
    void dimensions();
    void clear();
    void windowIdChange();
    void receiveSharedMemory();
    void disconnect1();
    void disconnect2();
    void disconnect3();
    void reconnect();

private:
    bool checkArea(MReactionMapTest *reactionMap, uchar value, int x, int y, int width, int height);

    QGraphicsScene *scene;
    QGraphicsView *widget1;
    QGraphicsView *widget2;
    QWidget *childOfWidget1;
    QGraphicsRectItem *item;
    MReactionMapTest *reactionMap1;
    MReactionMapTest *reactionMap2;
};

#endif
