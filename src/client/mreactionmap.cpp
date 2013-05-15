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

#include "mreactionmap.h"
#include "mreactionmap_p.h"

#include <QColor>
#include <QCoreApplication>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>

#include <mfsettings.h>

const QString MReactionMap::Press = "press";
const QString MReactionMap::Release = "release";
const QString MReactionMap::Cancel = "cancel";
const QString MReactionMap::Transparent = "transparent";
const QString MReactionMap::Inactive = "";

MReactionMap::MReactionMap(QWidget *topLevelWidget, const QString& appIdentifier,
                           QObject *parent)
    : QObject(parent), d(new MReactionMapPrivate(topLevelWidget, appIdentifier, this))
{
    if (topLevelWidget->parentWidget() != 0) {
        qFatal("Trying to create a MReactionMap for a widget which is not a top level widget!");
    }

    if (d->reactionMaps.contains(topLevelWidget)) {
        qFatal("Trying to create a second instance of MReactionMap for a widget!");
    }

    d->reactionMaps.insert(topLevelWidget, this);
}

MReactionMap::~MReactionMap()
{
    delete(d);
    d = NULL;
}

MReactionMap *MReactionMap::instance(QWidget *anyWidget)
{
    QWidget *topLevel = NULL, *test = NULL;

    if (!anyWidget)
        return NULL;

    // Find top level parent widget for anyWidget
    topLevel = anyWidget;
    for(;;) {
        test = topLevel->parentWidget();
        if (test != 0) {
            topLevel = test;
        } else {
            break;
        }
    }

    return MReactionMapPrivate::reactionMaps.value(topLevel, NULL);
}

void MReactionMap::setInactiveDrawingValue()
{
    setDrawingValue(MReactionMap::Inactive, MReactionMap::Inactive);
}

void MReactionMap::setReactiveDrawingValue()
{
    setDrawingValue(MReactionMap::Press, MReactionMap::Release);
}

void MReactionMap::setTransparentDrawingValue()
{
    setDrawingValue(MReactionMap::Transparent, MReactionMap::Transparent);
}

void MReactionMap::setDrawingValue(const QString &pressFeedback,
                                     const QString &releaseFeedback)
{
    quint8 value = d->palette->drawingValue(pressFeedback, releaseFeedback);

    d->setDrawingValue(value);
}

void MReactionMap::clear()
{
    d->clear();
}

QTransform MReactionMap::transform() const
{
    return d->screenToReactionMapTransform;
}

void MReactionMap::setTransform(QGraphicsItem *item, QGraphicsView *view)
{
    if (!item || !view) {
        return;
    }

    d->setTransform(item, view);
}

void MReactionMap::setTransform(QTransform transform)
{
    d->setTransform(transform);
}

void MReactionMap::fillRectangle(int x, int y, int width, int height)
{
    d->fillRectangle(x, y, width, height);
}

void MReactionMap::fillRectangle(const QRect &rectangle)
{
    d->fillRectangle(rectangle.x(), rectangle.y(),
                     rectangle.width(), rectangle.height());
}

void MReactionMap::fillRectangle(const QRectF &rectangle)
{
    d->fillRectangle(rectangle.x(), rectangle.y(),
                     rectangle.width(), rectangle.height());
}

void MReactionMap::fillRectangle(const QRect &rectangle,
                                 const QString &pressFeedback,
                                 const QString &releaseFeedback)
{
    quint8 currentDrawingValue = d->drawingValue;

    setDrawingValue(pressFeedback, releaseFeedback);
    fillRectangle(rectangle);

    d->drawingValue = currentDrawingValue;
}

void MReactionMap::fillRectangle(const QRectF &rectangle,
                                 const QString &pressFeedback,
                                 const QString &releaseFeedback)
{
    quint8 currentDrawingValue = d->drawingValue;

    setDrawingValue(pressFeedback, releaseFeedback);
    fillRectangle(rectangle);
    d->drawingValue = currentDrawingValue;
}

void MReactionMap::fillItemBoundRect(QGraphicsItem *item)
{
    if (!item) {
        return;
    }

    QList<QGraphicsView*> views = d->findViewForItem(item);

    // If a view associated with this reaction map was found, use that
    // view's transformation to draw to this reaction map.
    if (!views.isEmpty()) {
        QTransform currentTransform = d->transform;
        QRectF itemRect = item->boundingRect();
        // Fill the bounding rectangle in every view
        for (int i = 0; i < views.size(); ++i) {
            // Don't draw ouside the view, only draw the part of item
            // that is currently visible inside the view.
            QRectF viewRect = item->deviceTransform(views[i]->viewportTransform()).inverted().mapRect(views[i]->rect());
            QRectF resultRect = viewRect.intersected(itemRect);

            // Don't try to draw empty rectangels
            if (!resultRect.isEmpty()) {
                setTransform(item, views[i]);
                fillRectangle(resultRect);
            }
        }
        d->setTransform(currentTransform);
    }
}

void MReactionMap::fillItemBoundRect(QGraphicsItem *item,
                                     const QString &pressFeedback,
                                     const QString &releaseFeedback)
{
    if (!item) {
        return;
    }

    quint8 currentDrawingValue = d->drawingValue;

    setDrawingValue(pressFeedback, releaseFeedback);
    fillItemBoundRect(item);
    d->drawingValue = currentDrawingValue;
}

int MReactionMap::width() const
{
    return MfSettings::reactionMapWidth();
}

int MReactionMap::height() const
{
    return MfSettings::reactionMapHeight();
}

