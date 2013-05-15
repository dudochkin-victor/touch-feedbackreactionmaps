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

#include "ut_colormap.h"

#include <QBuffer>
#include <colormap.h>

#include <iostream>
using namespace std;

QTEST_MAIN(Ut_ColorMap)

void Ut_ColorMap::regularPressRelease()
{
    QByteArray bytes("foo,bar=123,200,10");
    QList<ColorMap> list;
    ColorMap colorMap;

    QBuffer buffer(&bytes);
    list = ColorMap::loadFromIODevice(buffer);

    QCOMPARE(list.size(), 1);

    colorMap = list[0];

    QCOMPARE(colorMap.pressFeedbackName(), QString("foo"));
    QCOMPARE(colorMap.releaseFeedbackName(), QString("bar"));
    QCOMPARE(colorMap.color(), qRgb(123,200,10));
}

void Ut_ColorMap::justPress()
{
    QByteArray bytes("foo,=123,200,10");
    QList<ColorMap> list;
    ColorMap colorMap;

    QBuffer buffer(&bytes);
    list = ColorMap::loadFromIODevice(buffer);

    QCOMPARE(list.size(), 1);

    colorMap = list[0];

    QCOMPARE(colorMap.pressFeedbackName(), QString("foo"));
    QCOMPARE(colorMap.releaseFeedbackName(), QString(""));
    QCOMPARE(colorMap.color(), qRgb(123,200,10));
}

void Ut_ColorMap::multipleEntries()
{
    QByteArray bytes("foo,bar=123,200,10\nspam,eggs=10,21,34");
    QList<ColorMap> list;
    ColorMap colorMap;

    QBuffer buffer(&bytes);
    list = ColorMap::loadFromIODevice(buffer);

    QCOMPARE(list.size(), 2);

    colorMap = list[0];
    QCOMPARE(colorMap.pressFeedbackName(), QString("foo"));
    QCOMPARE(colorMap.releaseFeedbackName(), QString("bar"));
    QCOMPARE(colorMap.color(), qRgb(123,200,10));

    colorMap = list[1];
    QCOMPARE(colorMap.pressFeedbackName(), QString("spam"));
    QCOMPARE(colorMap.releaseFeedbackName(), QString("eggs"));
    QCOMPARE(colorMap.color(), qRgb(10,21,34));
}
