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

#ifndef TESTAPPLICATION_H
#define TESTAPPLICATION_H

#include <QApplication>
#include <QGridLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QWidget>
#include <QScrollBar>
#include <QObject>
#include <mreactionmap.h>
#include <stdio.h>

class TestApplication : public QApplication
{
    Q_OBJECT

public:
    TestApplication(int argc, char **argv);
    virtual ~TestApplication();

public slots:
    void drawReactionMap();

private:
    QGraphicsView *view1, *view2;
    QGraphicsScene *scene;
    QGraphicsRectItem *bigRect;
    QGraphicsRectItem *smallRect;
    QGraphicsRectItem *screenRect;
    QWidget *mainWindow;
    QGridLayout *layout;
    MReactionMap *reactionMap;
};

#endif

