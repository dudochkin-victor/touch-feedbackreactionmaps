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

#include "mfserver_mock.h"
#include <mfserver.h>
#include <mfreactionmapstack.h>
#include <mfreactionmap.h>

#include <QDebug>

MfServer::MfServer(MfReactionMapStack *stack, QObject *parent)
    : QObject(parent), stack(stack)
{
}

bool MfServer::init()
{
    MfReactionMap *map;

    // Create two reaction maps with window id's 1 and 2 and
    // add them to the reactionmap stack.
    map = new MfReactionMap(NULL, 1, 1, this);
    stack->add(map);

    map = new MfReactionMap(NULL, 2, 2, this);
    stack->add(map);

    return true;
}

void MfServer::removePreExistingSharedMemory()
{
}

void MfServer::onNewConnection()
{
}
