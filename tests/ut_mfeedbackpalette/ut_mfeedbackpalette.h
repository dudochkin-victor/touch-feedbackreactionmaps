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

#ifndef UT_MFEEDBACKPALETTE_H
#define UT_MFEEDBACKPALETTE_H

#include <QtTest/QtTest>
#include <QObject>

#include "mfeedbackpalette.h"
#include <mfcommondata.h>

class MFeedbackPaletteTest;

class Ut_MFeedbackPalette : public QObject
{
    Q_OBJECT
public:
    Ut_MFeedbackPalette(QObject *parent = 0);

private slots:
    void initTestCase(){}
    void cleanupTestCase(){}

    void init();
    void cleanup();

    void findIds();
    void drawingValuesWithoutGivenPalette();
    void drawingValuesWithGivenPalette1();
    void drawingValuesWithGivenPalette2();
    void findDrawingValue();

private:
    MFeedbackPalette *feedbackPalette;
    MfPaletteEntry *palette;
};

#endif
