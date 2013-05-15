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

#include "mfxcorethread.h"

// Local function prototypes
static int *xerrorHandler(Display *display, XErrorEvent *errorEvent);

MfXCoreThread::MfXCoreThread(QObject *parent)
    : QThread(parent), display(NULL)
{
}

void MfXCoreThread::run()
{
    if (!setup()) {
        goto CLEANUP;
    }

    listen();

 CLEANUP:

    if (display) {
        XCloseDisplay(display);
        display = NULL;
    }
}

bool MfXCoreThread::setup()
{
    char *displayName = NULL;

    displayName = XDisplayName(NULL);
    if (displayName == NULL) {
        qCritical("MfXCoreThread: Cannot get display name.");
    }

    display = XOpenDisplay(displayName);
    if (display == NULL) {
        qCritical("MfXCoreThread: Cannot open display \"%s\".", displayName);
        return false;
    }

    XSetErrorHandler((XErrorHandler) xerrorHandler);

    XAllowEvents(display, AsyncBoth, CurrentTime);

    XSelectInput(display, DefaultRootWindow(display),
                 ButtonPressMask | ButtonReleaseMask);

    return true;
}

bool MfXCoreThread::listen()
{
    XEvent e;

    XGrabButton(display, AnyButton, AnyModifier, DefaultRootWindow(display),
                True, ButtonPressMask | ButtonReleaseMask,
                GrabModeSync, GrabModeAsync, None, None);

    while (true) {
        XNextEvent(display, &e);
        switch (e.type) {
            case ButtonPress:
                if (e.xbutton.button <= 3) {
                    /* Now trigger a sound event, the quick version */
                }

                fprintf(stderr, "ButtonPress: %u %u/%u\n",
                        e.xbutton.button,
                        e.xbutton.x, e.xbutton.y);
                break;

            case ButtonRelease:
                fprintf(stderr, "ButtonRelease: %u %u/%u\n",
                        e.xbutton.button,
                        e.xbutton.x, e.xbutton.y);
                break;
        }

//      XSendEvent(e.xbutton.display, InputFocus, False, e.type, &e);
        XAllowEvents(e.xbutton.display, ReplayPointer, CurrentTime);
    }

    return true;
}

static int *xerrorHandler(Display *display, XErrorEvent *errorEvent)
{
    Q_UNUSED(display);
    Q_UNUSED(errorEvent);
    qCritical("MfXCoreThread: Error from X");
    return NULL;
}
