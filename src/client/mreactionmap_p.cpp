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

#include "mreactionmap_p.h"
#include "mreactionmap.h"

#include <mfsettings.h>

#include <QApplication>
#include <QFileInfo>
#include <QWidget>
#include <QDebug>
#include <QDesktopWidget>
#include <QGraphicsItem>
#include <QGraphicsView>

#include <sys/shm.h>
#include <fcntl.h>

QMap<QWidget *, MReactionMap *> MReactionMapPrivate::reactionMaps;

MReactionMapPrivate::MReactionMapPrivate(QWidget *topLevelWidget,
                                         const QString& applicationName,
                                         QObject *parent)
    : QObject(parent), palette(0)
{
    this->topLevelWidget = topLevelWidget;
    this->topLevelWidget->installEventFilter(this);
    topLevelWidgetWinId = this->topLevelWidget->effectiveWinId();

    image = 0;
    isImageTemporary = false;
    sharedMemoryAddress = 0;
    drawingValue = 0;
    state = StateWaitingResponse;

    QFileInfo fileInfo(QCoreApplication::arguments().at(0));
    this->applicationName = fileInfo.fileName();
    if (!applicationName.isEmpty()) {
        QRegExp regExp("[0-9a-zA-Z_-]*");
        if (regExp.exactMatch(applicationName)) {
            this->applicationName = applicationName;
        }
    }

    palette = new MFeedbackPalette(this->applicationName);

    buildScreenTransform();

    reactionMapConnection = MReactionMapConnection::instance();

    connect(reactionMapConnection, SIGNAL(disconnected()), SLOT(onDisconnected()));
    connect(reactionMapConnection, SIGNAL(connectionFailed()), SLOT(onConnectionFailed()));

    reactionMapConnection->addReactionMap(this);
}

MReactionMapPrivate::~MReactionMapPrivate()
{
    reactionMaps.remove(topLevelWidget);

    if (image) {
        if (state != StateReady) {
            free(image);
        }
        image = 0;
    }

    if (palette) {
        delete palette;
        palette = 0;
    }

    if (sharedMemoryAddress) {
        if (shmdt(sharedMemoryAddress) == -1) {
            qWarning("MReactionMap: error when detaching shared"
                     " memory segment.");
        }
    }

    reactionMapConnection->removeReactionMap(this);
}

bool MReactionMapPrivate::buildSharedMemoryImage(const QString& tempFilePath)
{
    int id;
    key_t key;
    char proc_id = 'C';
    uchar *newImage = 0;
    MfPaletteEntry *newPalette;
    void *newSharedMemoryAddress = 0;

    key = ftok(tempFilePath.toAscii().constData(), proc_id);
    id = shmget(key, 0, S_IROTH | S_IWOTH);

    if (id == -1) {
        qCritical() << "Unable to get shared memory for reaction map"
                    << tempFilePath;
        return false;
    }

    newSharedMemoryAddress = shmat(id, 0, 0);

    if ((qptrdiff)newSharedMemoryAddress == -1) {
        qCritical() << "Unable to get shared memory address for"
                    << tempFilePath;
        newSharedMemoryAddress = 0;
        return false;
    }

    // Shared memory is organised as follows:
    // - Reaction map (reaction map width * reaction map height)
    // - Reaction map palette (256*sizeof(struct MfPaletteEntry))
    newImage = static_cast<uchar*>(newSharedMemoryAddress);
    newPalette = reinterpret_cast<MfPaletteEntry*>(&newImage[MfSettings::reactionMapWidth()*MfSettings::reactionMapHeight()]);

    if (image != 0) {
        // Pass stuff from current image to the new one
        memcpy(newImage, image, MfSettings::reactionMapWidth()*MfSettings::reactionMapHeight());
        // Free local temporary image if created previously
        if (isImageTemporary == true) {
            free(image);
            image = 0;
            isImageTemporary = false;
        }
    }
    image = newImage;
    palette->setPaletteMemory(newPalette);

    if (sharedMemoryAddress) {
        if (shmdt(sharedMemoryAddress) == -1) {
            qWarning("MReactionMap: error when detaching shared"
                     " memory segment.");
        }
        sharedMemoryAddress = 0;
    }
    sharedMemoryAddress = newSharedMemoryAddress;

    return true;
}

void MReactionMapPrivate::buildScreenTransform()
{
    QDesktopWidget *desktop = QApplication::desktop();
    QRect screenRect = desktop->screenGeometry();

    qreal scaleX = ((qreal)MfSettings::reactionMapWidth()) /
                   ((qreal)screenRect.width());

    qreal scaleY = ((qreal)MfSettings::reactionMapHeight()) /
                   ((qreal)screenRect.height());

    screenToReactionMapTransform = QTransform::fromScale(scaleX, scaleY);

    // By default use screen to reaction map transform when drawing
    transform = screenToReactionMapTransform;
}

QList<QGraphicsView*> MReactionMapPrivate::findViewForItem(QGraphicsItem *item)
{
    QList<QGraphicsView*> retVal;
    if (!item || !item->scene() || item->scene()->views().isEmpty()) {
        return retVal;
    }

    // Go through all views of the items and find out which view(s)
    // have a reaction map associated with the view (or the view's
    // parent top level widget)
    for (int i = 0; i < item->scene()->views().size(); ++i) {
        // Find the top level widget
        QWidget *parentWidget = item->scene()->views()[i];
        while (parentWidget && parentWidget->parentWidget()) {
            if (parentWidget->parentWidget()) {
                parentWidget = parentWidget->parentWidget();
            } else {
                break;
            }
        }

        if (topLevelWidget == parentWidget) {
            retVal << item->scene()->views()[i];
        }
    }

    return retVal;
}

void MReactionMapPrivate::setTransform(QGraphicsItem *item, QGraphicsView *view)
{
    // From item's local coordinate system to scene coordinate system
    QTransform itemToSceneTransform;

    // From scene coordinate system to QGraphicsView coordinate system
    QTransform sceneToViewTransform = view->viewportTransform();

    // From viewport to actual screen coordinates
    QTransform viewToScreenTransform;

    // GraphicsView position in screen coordinates (actual display pixels)
    QPoint viewPosScreenCoords;

    itemToSceneTransform = item->sceneTransform();

    viewPosScreenCoords = view->mapToGlobal(QPoint(0,0));

    viewToScreenTransform =
        QTransform::fromTranslate(viewPosScreenCoords.x(), viewPosScreenCoords.y());

    // It's itemToScene followed by sceneToView, viewToScreen and finally by screenToReactionMap
    transform =
        itemToSceneTransform * sceneToViewTransform * viewToScreenTransform
        * screenToReactionMapTransform;
}

void MReactionMapPrivate::setTransform(QTransform transform)
{
    // Set current QTransform
    this->transform = transform;
}

void MReactionMapPrivate::setDrawingValue(quint8 newValue)
{
    drawingValue = newValue;
}

void MReactionMapPrivate::createTemporaryImage()
{
    Q_ASSERT(state != StateReady);

    // Client wants to paint the image but we didn't receive the shm key
    // from meegofeedbackd yet. Client doesn't have to know about it.
    // Thus let's create a local image and use it. When the shm key finally
    // arrives we copy the contents of our local image into the real one,
    // in shared memory.
    image = static_cast<uchar*>(malloc(MfSettings::reactionMapWidth()*MfSettings::reactionMapHeight()));
    memset(image, 0, MfSettings::reactionMapWidth()*MfSettings::reactionMapHeight());
    isImageTemporary = true;
}

void MReactionMapPrivate::clear()
{
    if (image == 0) {
        createTemporaryImage();
    }

    // Zeroes mean no reaction
    memset(image, 0, MfSettings::reactionMapWidth()*MfSettings::reactionMapHeight());
}

void MReactionMapPrivate::fillRectangle(int x, int y, int width, int height)
{
    int imgWidth = MfSettings::reactionMapWidth();
    int imgHeight = MfSettings::reactionMapHeight();

    if (image == 0) {
        createTemporaryImage();
    }

    // Transform to reaction map coordinates
    QRect original(x, y, width, height);
    QRect result = transform.mapRect(original);
    int x1 = result.x();
    int y1 = result.y();
    int x2 = x1+result.width()-1;
    int y2 = y1+result.height()-1;

    // Check the invalid coordinates
    x1 = qBound(0, x1, imgWidth-1);
    y1 = qBound(0, y1, imgHeight-1);
    x2 = qBound(0, x2, imgWidth-1);
    y2 = qBound(0, y2, imgHeight-1);

    // Do not draw anything if coordinates result to empty rectangle
    if (x1 == x2 || y1 == y2) {
        return;
    }

    // Draw the rectangle
    for (int i = y1; i <= y2; i++) {
        memset(image+i*imgWidth+x1, drawingValue, x2-x1+1);
    }
}

void MReactionMapPrivate::onConnectionFailed()
{
    switch (state) {
        case StateReady:
            // Disconnect should be emitted before failing completely.
            qFatal("MReactionMap: Connection failed without emitting disconnect first!");
            break;

        case StateWaitingResponse:
            // Connection failed totally, give up.
            state = StateFailed;
            break;

        case StateFailed:
        default:
            // Do nothing
            break;
    };
}

void MReactionMapPrivate::onDisconnected()
{
    switch (state) {
        case StateReady:
        {
            // Connection suddenly dropped, need to start over.
            state = StateWaitingResponse;

            // Create local buffer for shared memory and copy existing data
            // from there before detaching from shared memory. This way we'll
            // have the same reaction map ready to use if server recovers.
            Q_ASSERT(isImageTemporary == false);
            uchar *oldImage = image;
            image = NULL;
            createTemporaryImage();
            memcpy(image, oldImage, MfSettings::reactionMapWidth()*MfSettings::reactionMapHeight());
            palette->useOwnPaletteMemory();

            reactionMapConnection->addReactionMap(this);
            break;
        }

        case StateWaitingResponse:
            // Connection was terminated during connection attempts or after
            // connecting before getting shared memory info. Try again.
            reactionMapConnection->addReactionMap(this);
            break;

        case StateFailed:
        default:
            // Do nothing
            break;
    };
}

void MReactionMapPrivate::setShmInfo(QString shmInfo)
{
    if (state == StateWaitingResponse) {
        if (buildSharedMemoryImage(shmInfo)) {
            state = StateReady;
            qDebug("MReactionMap: Ready.");
        } else {
            state = StateFailed;
            reactionMapConnection->removeReactionMap(this);
        }
    } else {
        qFatal("MReactionMap: Received shm information unexpectedly.");
    }
}

bool MReactionMapPrivate::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::WinIdChange) {
        // If window ID of the top level widget has changed, inform server.
        if (topLevelWidget->effectiveWinId() != topLevelWidgetWinId) {
            topLevelWidgetWinId = topLevelWidget->effectiveWinId();
            reactionMapConnection->updateReactionMapWindowId(this);
        }
    }

    // standard event processing
    return QObject::eventFilter(obj, event);
}
