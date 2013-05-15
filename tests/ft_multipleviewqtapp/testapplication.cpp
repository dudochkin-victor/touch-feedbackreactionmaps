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

#include "testapplication.h"

#include <QDebug>
#include <QTimer>

TestApplication::~TestApplication()
{
    delete mainWindow;
    mainWindow = NULL;

    delete reactionMap;
    reactionMap = NULL;
}

void TestApplication::drawReactionMap()
{
    MReactionMap *mapmap = MReactionMap::instance(view1);

    mapmap->clear();
    mapmap->setReactiveDrawingValue();
    mapmap->fillItemBoundRect(bigRect);
    mapmap->fillItemBoundRect(smallRect);
}

TestApplication::TestApplication(int argc, char **argv)
    : QApplication(argc, argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    mainWindow = new QWidget();
    view1 = new QGraphicsView();
    view2 = new QGraphicsView();
    bigRect = new QGraphicsRectItem(0, 0, 100, 100);
    smallRect = new QGraphicsRectItem(0, 0, 50, 50);
    screenRect = new QGraphicsRectItem(0, 0, 1000, 1000);
    layout = new QGridLayout();
    scene = new QGraphicsScene();

    reactionMap = new MReactionMap(mainWindow);

    // Put bigger rectangle in place
    bigRect->setPos(10, 50);
    scene->addItem(bigRect);

    // Put smaller rectangle in place
    smallRect->setPos(900, 900);
    scene->addItem(smallRect);
    scene->addItem(screenRect);

    view1->setScene(scene);
    view1->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    // Zoom in the other view a bit
    view2->setScene(scene);
    view2->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view2->scale(1.5, 1.5);

    // Add views to the layout
    layout->addWidget(view1, 0, 0);
    layout->addWidget(view2, 0, 1);

    mainWindow->setLayout(layout);
    mainWindow->show();

    // Redraw reaction map on slider value changes
    connect(view1->verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(drawReactionMap()));
    connect(view1->horizontalScrollBar(), SIGNAL(valueChanged(int)), SLOT(drawReactionMap()));
    connect(view2->verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(drawReactionMap()));
    connect(view2->horizontalScrollBar(), SIGNAL(valueChanged(int)), SLOT(drawReactionMap()));

    // Move views to suitable places
    view1->verticalScrollBar()->setSliderPosition(0);
    view1->horizontalScrollBar()->setSliderPosition(0);
    view2->verticalScrollBar()->setSliderPosition(0);
    view2->horizontalScrollBar()->setSliderPosition(110);

    // Draw initial reaction map after everything has appeared
    QTimer::singleShot(100, this, SLOT(drawReactionMap()));
}

