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

#ifndef MREACTIONMAPVIEWER_H
#define MREACTIONMAPVIEWER_H

#include <QWidget>
#include <QImage>
#include <QHash>
#include <mfeedbackpalette.h>
#include "colormap.h"

class MReactionMapViewer : public QWidget
{
    Q_OBJECT
public:
    MReactionMapViewer();
    virtual ~MReactionMapViewer();

    /*
     * \param pid Process id of application whose reaction map we want to view.
     * \param mapIndex Index of reaction map to view in case there are more                                                                       .
     *                         than one reaction maps defined for the process                                                                                                                                                   .
     * \param refreshInterval Refresh interval, in milliseconds.
     * \param colorMappingFilePath Path to color map file.
     */
    bool init(int pid, int mapIndex, int refreshInterval, const QString &colorMappingFilePath);

protected:
    void showReaction(const QString &pressFeedback,
                      const QString &releaseFeedback,
                      QRgb color);

    void paintEvent(QPaintEvent *event);

private slots:
    void updateReactionMapView();

private:
    static QString findTempFilePath(int pid, int mapIndex);
    bool loadReactionMap(const QString &tempFilePath);
    QString fetchApplicationName(const QString &tempFilePath);

    QImage *reactionMapView;
    QList<ColorMap> colorMapping;
    QHash<uchar, QRgb> reactionToViewHash;

    uchar *reactionMap;
    MFeedbackPalette *feedbackPalette;
    void *sharedMemoryAddress;

};

#endif
