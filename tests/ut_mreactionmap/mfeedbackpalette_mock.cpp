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

#include "mfeedbackpalette_mock.h"

#include <QDebug>

QString MFeedbackPalette::applicationName;

MFeedbackPalette::MFeedbackPalette(const QString &applicationName)
{
    this->applicationName = applicationName;
}

MFeedbackPalette::~MFeedbackPalette()
{
}

int MFeedbackPalette::id(const QString &feedbackName) const
{
    if (feedbackName == "transparent") {
        return 1;
    } else if (feedbackName == "press") {
        return 2;
    } else if (feedbackName == "release") {
        return 3;
    } else if (feedbackName == "foo") {
        return 4;
    } else if (feedbackName == "bar") {
        return 5;
    } else {
        return 0;
    }
}

void MFeedbackPalette::setPaletteMemory(MfPaletteEntry *palette)
{
    Q_UNUSED(palette);
}

void MFeedbackPalette::useOwnPaletteMemory()
{
}

uchar MFeedbackPalette::drawingValue(const QString &pressFeedback,
                                       const QString &releaseFeedback)
{
    int pressFeedbackId = id(pressFeedback);
    int releaseFeedbackId = id(releaseFeedback);

    if (pressFeedbackId == 1 && releaseFeedbackId == 1) {
        return 1;
    } else if (pressFeedbackId == 2 && releaseFeedbackId == 3) {
        return 2;
    } else if (pressFeedbackId == 4 && releaseFeedbackId == 5) {
        return 3;
    } else {
        return 0;
    }
}

uchar MFeedbackPalette::findDrawingValue(const QString &pressFeedback,
                                           const QString &releaseFeedback) const
{
    Q_UNUSED(pressFeedback);
    Q_UNUSED(releaseFeedback);
    return 0;
}

QString MFeedbackPalette::latestApplicationName()
{
    return applicationName;
}

