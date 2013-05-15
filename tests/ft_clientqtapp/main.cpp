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

#include <mreactionmap.h>

#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QPushButton>

static void paintReaction(QWidget *topLevelWidget);

int main (int argc, char **argv)
{
    QApplication app(argc, argv);
    QPushButton mainWindow("Test button");
    mainWindow.setWindowFlags(Qt::FramelessWindowHint);
    MReactionMap reactionMap(&mainWindow);

    paintReaction(&mainWindow);

    mainWindow.show();

    return app.exec();
}

static void paintReaction(QWidget *topLevelWidget)
{
    MReactionMap *reactionMap = MReactionMap::instance(topLevelWidget);

    reactionMap->setReactiveDrawingValue();
    reactionMap->fillRectangle(0, 0, 432, 480);

    reactionMap->setDrawingValue("press", "");
    reactionMap->fillRectangle(432, 0, 432, 480);

}
