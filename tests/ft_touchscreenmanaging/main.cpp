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

#include <QCoreApplication>
#include <QDebug>
#include <mfreactorsource.h>

#include "mftouchscreenlistener_mock.h"
#include "mfxlistener_mock.h"
#include "mfserver_mock.h"

bool verifyStatuses(MfReactorSource *src, bool reactionMapActive,
                    bool touchScreenListenerActive,
                    int touchScreenListenerSetActiveCalled)
{
    bool retVal = true;

    if (!src) {
        qCritical() << "verifyStatuses: NULL MfReactorSource provided!";
        return false;
    }

    if (src->isActive() != reactionMapActive) {
        qCritical() << "verifyStatuses: MfReactorSource::isActive:" << src->isActive()
                    << "Expected:" << reactionMapActive;
        retVal = false;
    }

    if (isTouchScreenListenerActive != touchScreenListenerActive) {
        qCritical() << "verifyStatuses: isTouchScreenListenerActive:" << isTouchScreenListenerActive
                    << "Expected:" << touchScreenListenerActive;
        retVal = false;
    }

    if (touchScreenListenerSetActiveCallCount != touchScreenListenerSetActiveCalled) {
        qCritical() << "verifyStatuses: touchScreenListenerSetActiveCallCount:" << touchScreenListenerSetActiveCallCount
                    << "Expected:" << touchScreenListenerSetActiveCalled;
        retVal = false;
    }

    return retVal;
}

int main(int argc, char **argv)
{
    QCoreApplication fooApp(argc, argv);
    QList<quint32> windowList;
    QMap<QString, QString> deviceState;

    // Create a window list, top to bottom: 3, 2, 1, 0
    windowList << 0; // Does not have a reaction map
    windowList << 1; // Does have a reaction map
    windowList << 2; // Does have a reaction map
    windowList << 3; // Does not have a reaction map

    MfReactorSource *src = new MfReactorSource();

    src->init();

    // Report initial window stacking to MfReactionMapStack
    touchScreenListenerSetActiveCallCount = 0;
    stackEmitter.emitWindowStackChanged(windowList);
    fooApp.processEvents();

    // After initalization MfReactorSource should be inactive,
    // MfTouchScreenListener should be inactive and
    // MfTouchScreenListener::setActive()
    // should not have been called
    if (!verifyStatuses(src, false, false, 0)) {
        qFatal("Test FAILED.");
    }

    // Go through all possible states
    // 1. Reaction map on top and display is off
    touchScreenListenerSetActiveCallCount = 0;
    // Rearrange windows so that reactive is on top
    windowList.move(2, 3); // 2, 3, 1, 0
    stackEmitter.emitWindowStackChanged(windowList);
    fooApp.processEvents();
    // Notify display is "off"
    deviceState["display"] = "off";
    src->deviceStateChanged(deviceState);
    fooApp.processEvents();
    if (!verifyStatuses(src, false, false, 2)) {
        qFatal("Test FAILED.");
    }

    // 2. Reaction map on top and display is on
    touchScreenListenerSetActiveCallCount = 0;
    // Notify display is "on"
    deviceState["display"] = "on";
    src->deviceStateChanged(deviceState);
    fooApp.processEvents();
    if (!verifyStatuses(src, true, true, 1)) {
        qFatal("Test FAILED.");
    }

    // 3. Reaction map not on top and display is off
    touchScreenListenerSetActiveCallCount = 0;
    // Notify display is "off"
    deviceState["display"] = "off";
    src->deviceStateChanged(deviceState);
    // Rearrange windows so that reactive is not on top
    windowList.move(2, 3); // 3, 2, 1, 0
    stackEmitter.emitWindowStackChanged(windowList);
    fooApp.processEvents();
    if (!verifyStatuses(src, false, false, 1)) {
        qFatal("Test FAILED.");
    }

    // 4. Reaction map not on top and display is on
    touchScreenListenerSetActiveCallCount = 0;
    // Notify display is "off"
    deviceState["display"] = "on";
    src->deviceStateChanged(deviceState);
    fooApp.processEvents();
    if (!verifyStatuses(src, false, false, 0)) {
        qFatal("Test FAILED.");
    }

    delete src;

    qCritical() << "Test SUCCESSFUL";

    return 0;
}

