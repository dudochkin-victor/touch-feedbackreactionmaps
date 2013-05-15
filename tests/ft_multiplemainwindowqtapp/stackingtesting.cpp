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

#include "stackingtesting.h"

#include <QPushButton>
#include <QTimer>
#include <QDebug>
#include <mreactionmap.h>


StackingTesting::StackingTesting(int argc, char **argv)
    : QApplication(argc, argv)
{
    // Create window 1
    window1 = new QPushButton("Feedbacks");
    window1->setWindowFlags(Qt::FramelessWindowHint);
    MReactionMap *reactionMap1;
    reactionMap1 = new MReactionMap(window1, "Feedbacks");

    // Create window 2
    // Note: Reaction map is created before setting window flags
    //       to test that reaction map stacking works correctly
    //       even when window ID of a top-level widget changes
    //       on runtime. (Setting window flags causes window ID
    //       change)
    window2 = new QPushButton("Transparent");
    MReactionMap *reactionMap2;
    reactionMap2 = new MReactionMap(window2, "Transparent");
    window2->setWindowFlags(Qt::FramelessWindowHint);

    MReactionMap *reactionMapInstance;

    // For window 1: First half of the screen (divided vertically)
    // gives press and release feedback. Second half of the screen
    // gives only press feedback.
    reactionMapInstance = MReactionMap::instance(window1);
    reactionMapInstance->setReactiveDrawingValue();
    reactionMapInstance->fillRectangle(0, 0, 432, 480);
    reactionMapInstance->setDrawingValue("press", "");
    reactionMapInstance->fillRectangle(432, 0, 432, 480);

    // For window 2: Middle of the screen is transparent. Right and
    // left part of the screen don't give any feedback.
    reactionMapInstance = MReactionMap::instance(window2);
    reactionMapInstance->setTransparentDrawingValue();
    reactionMapInstance->fillRectangle(216, 0, 432, 480);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(changeWindows()));
    timer->start(2000);

    window2->show();
    window1->show();

}

StackingTesting::~StackingTesting()
{
}

void StackingTesting::changeWindows() {
    static bool window1Visible = true;
    if (window1Visible) {
        window2->show();
        window1->hide();
        window1Visible = false;
    } else {
        window1->show();
        window2->hide();
        window1Visible = true;
    }
}

