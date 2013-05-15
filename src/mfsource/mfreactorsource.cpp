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

#include "mfreactorsource.h"
#include "mfserver.h"
#include "mftouchscreenlistener.h"
#include "mfxlistener.h"

#include <mfsettings.h>

MfReactorSource::MfReactorSource()
    : touchScreenListener(NULL), reactionMapStack(MfSettings::ignoreStacking()),
      reactionMapActive(true), displayActive(true), initialized(false)
{
}

MfReactorSource::~MfReactorSource()
{
    if (server) {
        delete server;
        server = NULL;
    }

    if (touchScreenListener) {
        delete touchScreenListener;
        touchScreenListener = NULL;
    }

    if (xListener) {
        delete xListener;
        xListener = NULL;
    }
}

bool MfReactorSource::init()
{
    bool ok;

    if (initialized) {
        return true;
    }

    xListener = new MfXListener(&reactionMapStack);

    ok = reactionMapStack.connect(xListener,
                                  SIGNAL(windowStackChanged(QList<quint32>)),
                                  SLOT(reorder(QList<quint32>)));
    if (!ok) return false;

    ok = connect(&reactionMapStack, SIGNAL(reactiveWindowOnTop(bool)),
                 this, SLOT(updateState()));
    if (!ok) return false;

    ok = xListener->startListening();
    if (!ok) return false;

    touchScreenListener = new MfTouchScreenListener(&reactionMapStack);
    ok = touchScreenListener->startListening();
    if (!ok) return false;

    ok = connect(this, SIGNAL(stateChanged(bool)),
                 touchScreenListener, SLOT(setActive(bool)));
    if (!ok) return false;

    server = new MfServer(&reactionMapStack, this);
    ok = server->init();
    if (!ok) return false;

    initialized = true;

    // Get the real state after initalization
    updateState();

    return true;
}

bool MfReactorSource::isActive() const
{
    return reactionMapActive;
}

void MfReactorSource::updateState()
{
    bool oldVal = reactionMapActive, reactiveOnTop;

    if (!initialized) {
        return;
    }

    reactiveOnTop = reactionMapStack.isReactiveWindowOnTop();

    reactionMapActive = reactiveOnTop && displayActive;

    if (oldVal != reactionMapActive) {
        emit stateChanged(reactionMapActive);
    }
}

void MfReactorSource::deviceStateChanged(const QMap<QString, QString> &newState)
{
    if (newState.contains("display")) {
        if (newState["display"] == "off") {
            displayActive = false;
        } else {
            displayActive = true;
        }
        updateState();
    }
}

Q_EXPORT_PLUGIN2(meegofeedback-reactionmaps, MfReactorSource);
