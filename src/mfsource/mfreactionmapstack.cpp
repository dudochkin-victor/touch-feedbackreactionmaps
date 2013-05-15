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

#include <QDebug>
#include <QMutexLocker>

#include <mffeedback.h>
#include <mfutil.h>

#include "mfreactionmap.h"

MfReactionMapStack::MfReactionMapStack(bool ignoreStacking, QObject *parent)
    : QObject(parent), ignoreStacking(ignoreStacking), reactiveOnTop(false)
{
    if (ignoreStacking) {
        qDebug("MfReactionMapStack: Ignoring window stacking.");
    }
}

MfReactionMapStack::~MfReactionMapStack()
{
}

void MfReactionMapStack::add(MfReactionMap *reactionMap)
{
    QMutexLocker locker(&mutex);

    Q_ASSERT(!reactionMapHash.contains(reactionMap->windowId()));
    reactionMapHash[reactionMap->windowId()] = reactionMap;

    Q_ASSERT(!reactionMapList.contains(reactionMap));
    reactionMapList.append(reactionMap);

    rebuildStack();
}

void MfReactionMapStack::remove(MfReactionMap *reactionMap)
{
    QMutexLocker locker(&mutex);

    if (reactionMapHash.remove(reactionMap->windowId())) {
        reactionMapList.removeOne(reactionMap);
        rebuildStack();
    }
}

void MfReactionMapStack::pressed(const QPoint &position)
{
    QMutexLocker locker(&mutex);
    MfReactionMap *reactionMap = 0;
    int i = 0;
    MfFeedback *feedback;
    bool isTransparent;

    if (ignoreStacking) {
        // Just use the first reaction map we happen to have
        if (reactionMapList.size() > 0) {
            feedback = reactionMapList[0]->pressFeedback(position, isTransparent);
            if (!isTransparent && feedback) {
                feedback->emitPlay(getTimestamp());
            }
        }
        return;
    }

    if (stack.size() == 0) {
        return;
    }

    do {
        reactionMap = stack[i];
        if (reactionMap) {
            feedback = reactionMap->pressFeedback(position, isTransparent);
            if (!isTransparent && feedback) {
                feedback->emitPlay(getTimestamp());
            }
        }
        i++;

    } while (reactionMap != 0 && isTransparent && i < stack.size());
}

void MfReactionMapStack::released(const QPoint &position)
{
    QMutexLocker locker(&mutex);
    MfReactionMap *reactionMap = 0;
    int i = 0;
    MfFeedback *feedback;
    bool isTransparent;

    if (ignoreStacking) {
        // Just use the first reaction map we happen to have
        if (reactionMapList.size() > 0) {
            feedback = reactionMapList[0]->releaseFeedback(position, isTransparent);
            if (!isTransparent && feedback) {
                feedback->emitPlay(getTimestamp());
            }
        }
        return;
    }

    if (stack.size() == 0) {
        return;
    }

    do {
        reactionMap = stack[i];
        if (reactionMap) {
            feedback = reactionMap->releaseFeedback(position, isTransparent);
            if (!isTransparent && feedback) {
                feedback->emitPlay(getTimestamp());
            }
        }
        i++;

    } while (reactionMap != 0 && isTransparent && i < stack.size());
}

void MfReactionMapStack::reorder(QList<quint32> windowList)
{
    QMutexLocker locker(&mutex);
    this->windowList = windowList;
    rebuildStack();
}

bool MfReactionMapStack::hasWindowReactionMap(quint32 windowId) {
    QMutexLocker locker(&mutex);

    return reactionMapHash.contains(windowId);
}

bool MfReactionMapStack::isReactiveWindowOnTop() const
{
    return reactiveOnTop;
}

void MfReactionMapStack::rebuildStack()
{
    stack.clear();
    quint32 windowId;
    bool onTop = false;

    for (int i = windowList.size() - 1; i >= 0; i--) {
        windowId = windowList[i];
        if (reactionMapHash.contains(windowId)) {
            stack.append(reactionMapHash[windowId]);
        } else {
            stack.append(NULL);
        }
    }

    // Send signal when a topmost window that uses reaction maps
    // appears or disappears.
    if (!windowList.isEmpty()) {
        onTop = reactionMapHash.contains(windowList.last());
    }
    if (onTop != reactiveOnTop) {
        reactiveOnTop = onTop;
        emit reactiveWindowOnTop(reactiveOnTop);
    }

#ifdef MF_DEBUG
    print();
#endif
}

#ifdef MF_DEBUG
void MfReactionMapStack::print()
{
    quint32 windowId;

    qDebug("\n-------------------------------------------");
    qDebug() << "REACTION MAP STACK (top to bottom)";

    for (int i = windowList.size() - 1; i >= 0; i--) {
        windowId = windowList[i];
        if (reactionMapHash.contains(windowId)) {
            qDebug() << hex << windowId << "- reaction map with"
                     << reactionMapHash[windowId]->applicationName()
                     << "session";
        } else {
            qDebug() << hex << windowId << "- no reaction map";
        }
    }

    qDebug("-------------------------------------------");
}
#endif
