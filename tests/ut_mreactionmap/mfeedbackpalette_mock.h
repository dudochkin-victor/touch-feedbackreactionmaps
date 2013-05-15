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

#ifndef MFEEDBACKPALETTEMOCK_H
#define MFEEDBACKPALETTEMOCK_H

#include <QString>
#include <mfcommondata.h>

class MFeedbackPalette
{
public:
    MFeedbackPalette(const QString &applicationName);
    virtual ~MFeedbackPalette();

    uchar drawingValue(const QString &pressFeedback,
                       const QString &releaseFeedback);

    uchar findDrawingValue(const QString &pressFeedback,
                           const QString &releaseFeedback) const;

    void setPaletteMemory(MfPaletteEntry *palette);

    void useOwnPaletteMemory();

    int id(const QString &feedbackName) const;

    static QString latestApplicationName();

protected:
    static QString applicationName;
};

#endif
