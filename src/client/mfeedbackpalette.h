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

#ifndef MFEEDBACKPALETTE_H
#define MFEEDBACKPALETTE_H

#include <QHash>
#include <QString>
#include <mfcommondata.h>

//! \internal

/*!
 * @class MFeedbackPalette
 * @brief MFeedbackPalette stores the palette of MReactionMap
 *
 * @state Stable
 * @copyright Nokia Corporation
 * @license Nokia Open Source
 * @osslibrary Qt 4.6
 * @scope Private
 */
class MFeedbackPalette
{
public:
    MFeedbackPalette(const QString &applicationName);
    virtual ~MFeedbackPalette();

    /*!
     * @brief Returns the value that represents the specified press and
     * release feedbacks. If value isn't found, one will be created.
     *
     * @warning Palette has only room for 256 different values.
     *
     * @param pressFeedback Name of feedback to be played when screen is
     *                      pressed.
     * @param releaseFeedback Name of feedback to be played when screen is
     *                        released.
     *
     * @return Value that encodes the specified press and release feedbacks
     * in the reaction map.
     */
    uchar drawingValue(const QString &pressFeedback,
                       const QString &releaseFeedback);

    /*!
     * @brief Returns the value that represents the specified press and
     * release feedbacks. If value isn't found, value for no reaction (0) is
     * returned.
     *
     * @param pressFeedback Name of feedback to be played when screen is
     *                      pressed.
     * @param releaseFeedback Name of feedback to be played when screen is
     *                        released.
     *
     * @return Value that encodes the specified press and release feedbacks
     * in the reaction map.
     */
    uchar findDrawingValue(const QString &pressFeedback,
                           const QString &releaseFeedback) const;

    /*!
     * @brief Sets the memory space that stores the used palette.
     *
     * @param palette Pointer to a memory space that has 256 MfPaletteEntry
     *                items reserved.
     */
    void setPaletteMemory(MfPaletteEntry *palette);

    /*!
     * @brief Allocates a new local temporary palette and copies previously
     * given palette to it.
     *
     * @sa MFeedbackPalette::setPaletteMemory().
     */
    void useOwnPaletteMemory();

    /*!
     * @brief Returns the id of a feedback, given its name
     *
     * @param feedbackName Name of feedback. Empty string means no
     *                     feedback.
     *
     * @return Value that encodes the specified feedback.
     */
    int id(const QString &feedbackName) const;

protected:
	bool createFeedbackList(const QString &applicationName);
    void createOwnPaletteMemory();

    MfPaletteEntry *palette;
    int paletteSize;
    bool ownPaletteMemory;

    QHash<QString, int> nameToIdHash;
};

//! \internal_end

#endif
