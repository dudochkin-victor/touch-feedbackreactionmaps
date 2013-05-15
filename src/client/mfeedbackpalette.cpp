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

#include "mfeedbackpalette.h"

#include <QDebug>
#include <QtAlgorithms>
#include <QDir>

#include <mfutil.h>

MFeedbackPalette::MFeedbackPalette(const QString &applicationName)
{
    palette = 0;
    paletteSize = 0;
    ownPaletteMemory = false;
    if (!createFeedbackList(applicationName)) {
        return;
    }
}

MFeedbackPalette::~MFeedbackPalette()
{
    if (palette != 0 && ownPaletteMemory == true) {
        free(palette);
        palette = 0;
    }
}

int MFeedbackPalette::id(const QString &feedbackName) const
{
    if (nameToIdHash.contains(feedbackName)) {
        return nameToIdHash[feedbackName];
    } else {
        return 0;
    }
}

void MFeedbackPalette::setPaletteMemory(MfPaletteEntry *palette) {
    Q_ASSERT(palette);

    // Copy existing palette to the given memory space.
    if (this->palette != 0) {
        memcpy(palette, this->palette, 256*sizeof(MfPaletteEntry));
        if (ownPaletteMemory == true) {
            free(this->palette);
            this->palette = 0;
            ownPaletteMemory = false;
        }
    }

    this->palette = palette;
}

void MFeedbackPalette::createOwnPaletteMemory()
{
    // Palette is used without a given pointer to memory. Create
    // a palette that is used until a pointer is given. When we
    // get the memory space, copy the palette to the given space
    // and start using that one.
    palette = static_cast<MfPaletteEntry*>(malloc(256*sizeof(MfPaletteEntry)));
    memset(palette, 0, 256*sizeof(MfPaletteEntry));
    paletteSize = 0;
    ownPaletteMemory = true;
}

void MFeedbackPalette::useOwnPaletteMemory() {
    MfPaletteEntry* oldPalette = 0;
    int oldPaletteSize;

    if (ownPaletteMemory == true) {
        // Already using own palette memory
        return;
    }

    oldPalette = palette;
    oldPaletteSize = paletteSize;

    createOwnPaletteMemory();

    if (oldPalette != 0) {
        memcpy(palette, oldPalette, 256*sizeof(MfPaletteEntry));
        paletteSize = oldPaletteSize;
    }
}

uchar MFeedbackPalette::drawingValue(const QString &pressFeedback,
                                     const QString &releaseFeedback)
{
    int pressFeedbackId = id(pressFeedback);
    int releaseFeedbackId = id(releaseFeedback);
    int i = 0;

    if (palette == 0) {
        createOwnPaletteMemory();
    }

    // Try to find the given indexes in the reaction palette
    for (i = 0; i <= paletteSize; ++i) {
        if (palette[i].pressFeedbackId == pressFeedbackId &&
            palette[i].releaseFeedbackId == releaseFeedbackId) {
            return static_cast<uchar>(i);
        }
    }

    // Check the free space in the palette
    if (paletteSize == 255) {
        qCritical() << "No space left in the reaction palette.";
        return 0;
    }

    // Add a new index into the palette
    paletteSize++;
    palette[i].pressFeedbackId = pressFeedbackId;
    palette[i].releaseFeedbackId = releaseFeedbackId;

    return static_cast<uchar>(paletteSize);
}

uchar MFeedbackPalette::findDrawingValue(const QString &pressFeedback,
                                         const QString &releaseFeedback) const
{
    int pressFeedbackId = id(pressFeedback);
    int releaseFeedbackId = id(releaseFeedback);
    int i = 0;

    if (palette == 0) {
        // No palette set yet, return value for no reaction
        return 0;
    }

    bool endOfPalette = false;

    // Try to find the given indexes in the reaction palette
    for (i = 0; i < 256 && !endOfPalette; ++i) {
        if (palette[i].pressFeedbackId == pressFeedbackId &&
            palette[i].releaseFeedbackId == releaseFeedbackId) {
            return static_cast<uchar>(i);
        }
        endOfPalette = palette[i+1].pressFeedbackId == 0 &&
                       palette[i+1].releaseFeedbackId == 0;
    }

    // Value not found, return "inactive" value
    return 0;
}

bool MFeedbackPalette::createFeedbackList(const QString &applicationName)
{
    QStringList feedbackList;

    feedbackList = getFeedbackNames(applicationName);
    if (feedbackList.isEmpty()) {
        return false;
    }
    qSort(feedbackList);
    nameToIdHash.clear();

    for (int i = 0; i < feedbackList.size(); ++i) {
        // Ids begin from 2 instead of 0 (zero)
        // 0 (zero) means inactive (no feedback).
        // 1 (one) means transparent.
        nameToIdHash[feedbackList[i]] = i + 2;
    }

    // Add transparent value
    if (!nameToIdHash.contains("transparent")) {
        nameToIdHash["transparent"] = 1;
    } else {
        qCritical() << "Default feedbacks and custom feedbacks may not be named \"transparent\".";
    }

    return true;
}
