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

#ifndef MFREACTIONMAP_H
#define MFREACTIONMAP_H

#include <QObject>

#include <QList>
#include <mfcommondata.h>

class MfSession;
class MfFeedback;
class MfReactionMapPrivate;

class QPoint;

class MfReactionMap : public QObject {
    Q_OBJECT
public:
    /*!
     * @param session Session to be used by the reaction map. MfReactionMap
     *                takes ownership of the received session
     * @param windowId X window ID of the client application
     * @param pid Process ID of the client application
     */
    MfReactionMap(MfSession *session, quint32 windowId, qint64 pid, QObject *parent = 0);
    virtual ~MfReactionMap();

    bool init();

    /*!
     *  Returns the press feedback for the given pixel
     *
     * @param isTransparent Will be set to true if at the specified coordinate the
     *                      reaction map is transparent.
     * @return press feedback for the given pixel or NULL if there's no feedback.
     */
    MfFeedback *pressFeedback(const QPoint &pixelCoords, bool &isTransparent);

    /*!
     *  Returns the release feedback for the given pixel
     *
     * @param isTransparent Will be set to true if at the specified coordinate the
     *                      reaction map is transparent.
     * @return release feedback for the given pixel or NULL if there's no feedback.
     */
    MfFeedback *releaseFeedback(const QPoint &pixelCoords, bool &isTransparent);

    /*!
     * Reaction map width
     *
     * @return width, in pixels.
     */
    int width() const;

    /*!
     * Reaction map height
     *
     * @return height, in pixels.
     */
    int height() const;

    QString tempFilePath() const;

    /*!
     * Sets new window ID for reaction map.
     *
     * Used when the window ID of a widget using a reaction map changes.
     */
    void setWindowId(quint32 windowId);

    quint32 windowId() const;

    /*!
     * Sets a pixel with the specified value.
     *
     * @warning Use only for performing tests
     */
    void setPixel(const QPoint &position,
                  const QString &pressFeedback,
                  const QString &releaseFeedback);

    /*!
     * Returns the application name stored in the MfSession belonging to this
     * reaction map.
     * Useful when debugging to identify a reaction map.
     */
    QString applicationName() const;

protected:
    MfReactionMapPrivate *d;
};

#endif
