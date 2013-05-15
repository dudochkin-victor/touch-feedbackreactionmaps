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

#include "mfreactionmapstack.h"
#include "mfxrecordthread.h"
#include "mfsettings.h"

/**
 * From xnee (libxnee/include/libxnee/xnee.h):
 * this typedef should, according to XRecord,
 * specification be defined in <X11/extensions/record.h>
 * ... can't find .... errrh?
 */
typedef union {
    unsigned char    type;
    xEvent           event;
    xResourceReq     req;
    xGenericReply    reply;
    xError           error;
    xConnSetupPrefix setup;
} XRecordDatum;

// Local function prototypes
static void xrecordInterceptCallback(XPointer closure, XRecordInterceptData *recordedData);

static MfXRecordThread *gInstance = NULL;

MfXRecordThread::MfXRecordThread(QObject *parent)
    : QThread(parent), display(NULL), context(NULL)
{
    if (gInstance) {
        qFatal("MfXRecordThread: Trying to create a second instance of MfXRecordThread");
    }

    gInstance = this;
}

MfXRecordThread::~MfXRecordThread()
{
    gInstance = NULL;
}

MfXRecordThread *MfXRecordThread::instance()
{
    return gInstance;
}

QPoint MfXRecordThread::calcPixelCoords(int x, int y)
{
    QPoint result;

    // Make sure no out-of-bounds values are created
    qBound(0, x, (displayWidth-1));
    qBound(0, y, (displayHeight-1));

    // Calculate coordinates in reaction map
    result.rx() = ( MfSettings::reactionMapWidth() * x ) / (displayWidth);
    result.ry() = ( MfSettings::reactionMapHeight() * y ) / (displayHeight);

    return result;
}

void MfXRecordThread::screenPressed(int x, int y)
{
    QPoint mapPoint;

    mapPoint = calcPixelCoords(x, y);
    reactionMapStack->pressed(mapPoint);
}

void MfXRecordThread::screenReleased(int x, int y)
{
    QPoint mapPoint;

    mapPoint = calcPixelCoords(x, y);
    reactionMapStack->released(mapPoint);
}

bool MfXRecordThread::init(MfReactionMapStack *reactionMapStack,
                           Display *display, XRecordContext context)
{
    if (!reactionMapStack || !display || !context) {
        return false;
    }

    this->reactionMapStack = reactionMapStack;
    this->display = display;
    this->context = context;

    // Get screen resolution
    displayWidth = XDisplayWidth(display, 0);
    displayHeight = XDisplayHeight(display, 0);

    return true;
}

void MfXRecordThread::run()
{
    // Start listening
    if (!XRecordEnableContext (display, context, xrecordInterceptCallback, (XPointer) this)) {
        qCritical("MfXRecordThread: Cannot enable XRecord context.");
    }
}

static void xrecordInterceptCallback(XPointer closure, XRecordInterceptData *recordedData)
{
    Q_UNUSED(closure);
    unsigned int x, y;
    XRecordDatum *datum = NULL;
    datum = (XRecordDatum *) recordedData->data;

    if (recordedData->category != XRecordFromServer) {
        goto CLEANUP;
    }

    if (datum == NULL) {
        goto CLEANUP;
    }

    if (datum->event.u.u.type == ButtonPress) {
        x = datum->event.u.keyButtonPointer.rootX;
        y = datum->event.u.keyButtonPointer.rootY;
        MfXRecordThread::instance()->screenPressed(x, y);
    } else if (datum->event.u.u.type == ButtonRelease) {
        x = datum->event.u.keyButtonPointer.rootX;
        y = datum->event.u.keyButtonPointer.rootY;
        MfXRecordThread::instance()->screenReleased(x, y);
    } else {
        qCritical("MfXRecordThread: Unknown event.");
    }

 CLEANUP:
    if (recordedData) {
        XRecordFreeData(recordedData);
    }
}
