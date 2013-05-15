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

#include "ut_mfeedbackpalette.h"

#include "mfeedbackpalette.h"

QTEST_MAIN(Ut_MFeedbackPalette)

Ut_MFeedbackPalette::Ut_MFeedbackPalette(QObject *parent)
    : QObject(parent)
{
    palette = 0;
}

void Ut_MFeedbackPalette::init()
{
    palette = static_cast<MfPaletteEntry*>(malloc(256*sizeof(MfPaletteEntry)));
    memset(palette, 0, 256*sizeof(MfPaletteEntry));

    feedbackPalette = new MFeedbackPalette("fooApplication");
}

void Ut_MFeedbackPalette::cleanup()
{
    delete feedbackPalette;
    feedbackPalette = 0;

    if (palette != 0) {
        free(palette);
        palette = 0;
    }
}

/*
 * Check that correct ID's are returned.
 */
void Ut_MFeedbackPalette::findIds()
{
    // Check that id returns the expected values.
    // Feedbacks should be in aplhabetical order.
    QCOMPARE(feedbackPalette->id("transparent"), 1);
    QCOMPARE(feedbackPalette->id("feedback-1"), 2);
    QCOMPARE(feedbackPalette->id("feedback-2"), 3);
    QCOMPARE(feedbackPalette->id("feedback-3"), 4);
    QCOMPARE(feedbackPalette->id("feedback-4"), 5);
    QCOMPARE(feedbackPalette->id("feedback-5"), 6);
    QCOMPARE(feedbackPalette->id("feedback-6"), 7);
    QCOMPARE(feedbackPalette->id("feedback-7"), 8);
    QCOMPARE(feedbackPalette->id("feedback-8"), 9);

    // Non existing feedbacks should return zero
    QCOMPARE(feedbackPalette->id("foobar"), 0);

    // Empty string is considered inactive value
    QCOMPARE(feedbackPalette->id(""), 0);
}


/*
 * Define drawing values before setting memory space for allocated
 * palette. Check that the palette values are actually created.
 */
void Ut_MFeedbackPalette::drawingValuesWithoutGivenPalette()
{
    QList<uchar>(paletteValues);

    paletteValues << feedbackPalette->drawingValue("foobar", "invalid");
    paletteValues << feedbackPalette->drawingValue("feedback-1", "feedback-2");
    paletteValues << feedbackPalette->drawingValue("feedback-3", "feedback-4");
    paletteValues << feedbackPalette->drawingValue("feedback-5", "feedback-6");
    paletteValues << feedbackPalette->drawingValue("feedback-7", "feedback-8");
    paletteValues << feedbackPalette->drawingValue("transparent", "transparent");
    paletteValues << feedbackPalette->drawingValue("feedback-1", "feedback-1");
    paletteValues << feedbackPalette->drawingValue("feedback-2", "feedback-2");
    paletteValues << feedbackPalette->drawingValue("feedback-3", "feedback-3");
    paletteValues << feedbackPalette->drawingValue("feedback-4", "feedback-4");
    paletteValues << feedbackPalette->drawingValue("feedback-5", "feedback-5");
    paletteValues << feedbackPalette->drawingValue("feedback-6", "feedback-6");
    paletteValues << feedbackPalette->drawingValue("feedback-7", "feedback-7");
    paletteValues << feedbackPalette->drawingValue("feedback-8", "feedback-8");
    paletteValues << feedbackPalette->drawingValue("feedback-1", "feedback-8");
    paletteValues << feedbackPalette->drawingValue("feedback-2", "feedback-7");
    paletteValues << feedbackPalette->drawingValue("feedback-3", "feedback-6");

    for (int i = 0; i < paletteValues.size(); ++i) {
        QCOMPARE(paletteValues[i], static_cast<uchar>(i));
    }
}

/*
 * Define drawing values before setting memory space for allocated
 * palette. Set allocated palette in to use. Check that the palette
 * values are copied to the given palette correctly.
 */
void Ut_MFeedbackPalette::drawingValuesWithGivenPalette1()
{
    int i;
    QList<QString> pressFeedbacks;
    QList<QString> releaseFeedbacks;
    quint8 pressId, releaseId;

    // Palette entries
    pressFeedbacks << "invalid" << "feedback-1" << "feedback-2" << "feedback-3"
                   << "feedback-4" << "feedback-5" << "feedback-6" << "feedback-7"
                   << "feedback-8" << "transparent" << "feedback-1" << "feedback-3"
                   << "feedback-4" << "feedback-2";
    releaseFeedbacks << "" << "feedback-1" << "feedback-2" << "feedback-3"
                     << "feedback-4" << "feedback-5" << "feedback-6" << "feedback-7"
                     << "feedback-8" << "transparent" << "feedback-2" << "feedback-4"
                     << "feedback-3" << "feedback-1";

    // Generate palette values
    for (i = 0; i < pressFeedbacks.size(); ++i) {
        feedbackPalette->drawingValue(pressFeedbacks[i], releaseFeedbacks[i]);
    }

    // Set palette memory
    feedbackPalette->setPaletteMemory(palette);

    // Check that memory is set to expected values
    for (i = 0; i < pressFeedbacks.size(); ++i) {
        pressId = feedbackPalette->id(pressFeedbacks[i]);
        releaseId = feedbackPalette->id(releaseFeedbacks[i]);

        QCOMPARE(palette[i].pressFeedbackId, pressId);
        QCOMPARE(palette[i].releaseFeedbackId, releaseId);
    }
}

/*
 * Define drawing values after setting memory space for allocated
 * palette. Check that the palette values are actually created to
 * the given memory space.
 */
void Ut_MFeedbackPalette::drawingValuesWithGivenPalette2()
{
    int i;
    QList<QString> pressFeedbacks;
    QList<QString> releaseFeedbacks;
    quint8 pressId, releaseId;

    // Set palette memory
    feedbackPalette->setPaletteMemory(palette);

    // Palette entries
    pressFeedbacks << "invalid" << "feedback-1" << "feedback-2" << "feedback-3"
                   << "feedback-4" << "feedback-5" << "feedback-6" << "feedback-7"
                   << "feedback-8" << "transparent" << "feedback-1" << "feedback-3"
                   << "feedback-4" << "feedback-2";
    releaseFeedbacks << "" << "feedback-1" << "feedback-2" << "feedback-3"
                     << "feedback-4" << "feedback-5" << "feedback-6" << "feedback-7"
                     << "feedback-8" << "transparent" << "feedback-2" << "feedback-4"
                     << "feedback-3" << "feedback-1";

    // Generate palette values
    for (i = 0; i < pressFeedbacks.size(); ++i) {
        feedbackPalette->drawingValue(pressFeedbacks[i], releaseFeedbacks[i]);
    }

    // Check that memory is set to expected values
    for (i = 0; i < pressFeedbacks.size(); ++i) {
        pressId = feedbackPalette->id(pressFeedbacks[i]);
        releaseId = feedbackPalette->id(releaseFeedbacks[i]);

        QCOMPARE(palette[i].pressFeedbackId, pressId);
        QCOMPARE(palette[i].releaseFeedbackId, releaseId);
    }
}

/*
 * Check that set palette values are found with findDrawingValue.
 */
void Ut_MFeedbackPalette::findDrawingValue()
{
    int i;
    QList<QString> pressFeedbacks;
    QList<QString> releaseFeedbacks;
    QList<uchar> values;

    // Set palette memory
    feedbackPalette->setPaletteMemory(palette);

    // Palette entries
    pressFeedbacks << "invalid" << "feedback-1" << "feedback-2" << "feedback-3"
                   << "feedback-4" << "feedback-5" << "feedback-6" << "feedback-7"
                   << "feedback-8" << "transparent" << "feedback-1" << "feedback-3"
                   << "feedback-4" << "feedback-2";
    releaseFeedbacks << "" << "feedback-1" << "feedback-2" << "feedback-3"
                     << "feedback-4" << "feedback-5" << "feedback-6" << "feedback-7"
                     << "feedback-8" << "transparent" << "feedback-2" << "feedback-4"
                     << "feedback-3" << "feedback-1";

    // Generate palette values
    for (i = 0; i < pressFeedbacks.size(); ++i) {
        values << feedbackPalette->drawingValue(pressFeedbacks[i], releaseFeedbacks[i]);
    }

    // Check that the drawing values are correct
    for (i = 0; i < pressFeedbacks.size(); ++i) {
        QCOMPARE(values[i], feedbackPalette->findDrawingValue(pressFeedbacks[i], releaseFeedbacks[i]));
    }

}

