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

#include "mftouchscreenlistener.h"
#include "mfxrecordthread.h"

#include <X11/Xlib.h>
#include <X11/extensions/record.h>
#include <X11/Xproto.h>

// Local function prototypes
static int *xerrorHandler(Display *display, XErrorEvent *errorEvent);

class MfTouchScreenListenerPriv
{
    public:
    MfTouchScreenListenerPriv();

    MfXRecordThread xrecordThread;
    MfReactionMapStack *reactionMapStack;
    Display *controlDisplay, *dataDisplay;
    XRecordContext context;
};

MfTouchScreenListenerPriv::MfTouchScreenListenerPriv()
    : reactionMapStack(NULL), controlDisplay(NULL), dataDisplay(NULL), context(0)
{
}

MfTouchScreenListener::MfTouchScreenListener(MfReactionMapStack *reactionMapStack,
                                             QObject *parent)
    : QObject(parent)
{
    d = new MfTouchScreenListenerPriv();
    d->reactionMapStack = reactionMapStack;
}

MfTouchScreenListener::~MfTouchScreenListener()
{
    if (!d) {
        return;
    }

    if (d->xrecordThread.isRunning()) {
        stopListening();
    }

    delete d;
    d = NULL;
}

bool MfTouchScreenListener::startListening()
{
    char *displayName = NULL;
    int recMajor, recMinor;
    XRecordRange *range = NULL;
    XRecordClientSpec clientSpec;

    // Init X to support threading
    if (!XInitThreads()) {
        qCritical("MfTouchScreenListener: Cannot initialize X to support threads.");
    }

    displayName = XDisplayName(NULL);
    if (displayName == NULL) {
        qCritical("MfTouchScreenListener: Cannot get display name.");
        return false;
    }

    // Open control display
    d->controlDisplay = XOpenDisplay(displayName);
    if (d->controlDisplay == NULL) {
        qCritical("MfTouchScreenListener: Cannot open display \"%s\".", displayName);
        return false;
    }

    // Open data display
    d->dataDisplay = XOpenDisplay(displayName);
    if (d->dataDisplay == NULL) {
        qCritical("MfTouchScreenListener: Cannot open display \"%s\".", displayName);
        goto CLEANUP;
    }

    XSetErrorHandler((XErrorHandler) xerrorHandler);

    // Synchronize control display
    XSynchronize(d->controlDisplay, true);

    if (!XRecordQueryVersion (d->controlDisplay, &recMajor, &recMinor)) {
        qCritical("MfTouchScreenListener: X server uses different XRecord version: (%d,%d)",
                  recMajor, recMinor);
        goto CLEANUP;
    }

    // Create XRecord context using control display
    range = XRecordAllocRange ();
    if (!range) {
        qCritical("MfTouchScreenListener: Cannot allocate range");
        goto CLEANUP;
    }

    range->device_events.first = ButtonPress;
    range->device_events.last = ButtonRelease;

    clientSpec = XRecordAllClients;

    d->context = XRecordCreateContext(d->controlDisplay,     /* display  */
                                      0,           /* datum_flags */
                                      &clientSpec, /* clients */
                                      1,           /* nclients */
                                      &range,     /* ranges */
                                      1);          /* nranges */

    if (!d->context) {
        qCritical("MfTouchScreenListener: Could not create XRecord context");
        goto CLEANUP;
    }

    XFree(range);
    range = NULL;

    // Prepare thread and run it
    d->xrecordThread.init(d->reactionMapStack, d->dataDisplay, d->context);
    d->xrecordThread.start();

    return true;

    CLEANUP:
        if (range) {
            XFree(range);
            range = NULL;
        }

        if (d->dataDisplay) {
            XCloseDisplay(d->dataDisplay);
            d->dataDisplay = NULL;
        }

        if (d->controlDisplay) {
            XCloseDisplay(d->controlDisplay);
            d->controlDisplay = NULL;
        }

        return false;
}

bool MfTouchScreenListener::stopListening()
{
    if (!d->xrecordThread.isRunning()) {
        // Nothing to be done
        return true;
    }

    // Disable XRecord context
    if (!XRecordDisableContext(d->controlDisplay, d->context)) {
        qCritical("MfTouchScreenListener: Cannot disable XRecord context");
    }

    // Wait until tread stops nicely or else use brute force
    if (!d->xrecordThread.wait(1000)) {
        // Ok, let's use brute force
        d->xrecordThread.terminate();
        d->xrecordThread.wait();
    }

    // Cleanup
    // Free context
    XRecordFreeContext(d->controlDisplay, d->context);
    d->context = 0;

    // Close data display
    XCloseDisplay(d->dataDisplay);
    d->dataDisplay = NULL;

    // Close control display
    XCloseDisplay(d->controlDisplay);
    d->controlDisplay = NULL;

    qDebug("MfTouchScreenListener: Stopped nicely");
    return true;
}

static int *xerrorHandler(Display *display, XErrorEvent *errorEvent)
{
    Q_UNUSED(display);
    Q_UNUSED(errorEvent);
    qCritical("MfXRecordThread: Error from X");
    return NULL;
}

bool MfTouchScreenListener::isActive()
{
    return true;
}

void MfTouchScreenListener::setActive(bool active)
{
    Q_UNUSED(active);
}
