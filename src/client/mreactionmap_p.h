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

#ifndef MREACTIONMAPPRIVATE_H
#define MREACTIONMAPPRIVATE_H

#include <QTransform>
#include <QObject>
#include <QList>

#include "mfeedbackpalette.h"
#include "mreactionmapconnection.h"

class QGraphicsItem;
class QGraphicsView;
class MReactionMap;

/*!
 * @class MReactionMapPrivate
 *
 * @state Stable
 * @copyright Nokia Corporation
 * @license Nokia Open Source
 * @osslibrary Qt 4.6
 * @scope Private
 */
class MReactionMapPrivate : public QObject
{
    Q_OBJECT
public:
    MReactionMapPrivate(QWidget *topLevelWidget, const QString& applicationName,
                          QObject *parent);
    virtual ~MReactionMapPrivate();

    // Build a QImage using the shared memory identified by the given
    // path to a temp file.
    bool buildSharedMemoryImage(const QString& tempFilePath);

    QList<QGraphicsView*> findViewForItem(QGraphicsItem *item);
    void setTransform(QGraphicsItem *item, QGraphicsView *view);
    void setTransform(QTransform transform);
    // Build screenToReactionMapTransform attribute
    void buildScreenTransform();
    void setDrawingValue(quint8 newValue);
    void createTemporaryImage();
    void clear();
    void fillRectangle(int x, int y, int width, int height);

    void setShmInfo(QString shmInfo);

public slots:
    void onDisconnected();
    void onConnectionFailed();

public:
    // Pointer to the top-level widget that this reaction map belongs to
    QWidget *topLevelWidget;
    quint32 topLevelWidgetWinId;

    void *sharedMemoryAddress;
    uchar *image;
    bool isImageTemporary;
    MFeedbackPalette *palette;

    // Transformation matrix from the item local coordinate system to the reaction map coordinates
    QTransform transform;
    quint8 drawingValue;

    // From screen to reaction map coordinates
    QTransform screenToReactionMapTransform;

    enum ReactionMapState {
        // Initial state. Waiting for shared memory information from
        // meegofeedbackd.
        StateWaitingResponse,

        // Finished setup with meegofeedbackd daemon. Successfully
        // attached to shared memory.
        StateReady,

        // Some unrecoverable error has happened. There's no way out of this
        // state.
        StateFailed
    } state;

    MReactionMapConnection *reactionMapConnection;

    QString applicationName;

    bool eventFilter(QObject *obj, QEvent *event);

    static QMap<QWidget *, MReactionMap *> reactionMaps;
};

#endif
