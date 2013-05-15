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

#include "mfreactionmap.h"
#include "mftouchscreenlistener.h"
#include "mfxcorethread.h"

class MfTouchScreenListenerPriv
{
    public:
    MfXCoreThread xcoreThread;
};

MfTouchScreenListener::MfTouchScreenListener(MfReactionMapStack *reactionMapStack,
                                             QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(reactionMapStack);
    d = new MfTouchScreenListenerPriv();
}

MfTouchScreenListener::~MfTouchScreenListener()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

bool MfTouchScreenListener::startListening()
{
    d->xcoreThread.start();
    return true;
}

bool MfTouchScreenListener::isActive()
{
    return true;
}

void MfTouchScreenListener::setActive(bool active)
{
    Q_UNUSED(active);
}
