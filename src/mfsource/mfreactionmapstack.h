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

#ifndef MFREACTIONMAPSTACK_H
#define MFREACTIONMAPSTACK_H

#include <QList>
#include <QMutex>
#include <QObject>
#include <QPoint>
#include <QHash>

class MfReactionMap;

class MfReactionMapStack : public QObject
{
    Q_OBJECT
public:
    explicit MfReactionMapStack(bool ignoreStacking = false, QObject *parent = 0);
    virtual ~MfReactionMapStack();

    void add(MfReactionMap *reactionMap);
    void remove(MfReactionMap *reactionMap);

    /*
     * Screen was pressed at the given position.
     */
    void pressed(const QPoint &position);

    /*
     * Screen was release at the given position.
     */
    void released(const QPoint &position);

    bool hasWindowReactionMap(quint32 windowId);

    bool isReactiveWindowOnTop() const;

signals:
    void reactiveWindowOnTop(bool state);

public slots:
    /* Reorder reaction maps according to their window ids.
     * @param window Current X Window ID stack, ordered bottom-to-top.
     */
    void reorder(QList<quint32> windowList);

private:
    void rebuildStack();

    #ifdef MF_DEBUG
    void print();
    #endif

    // Control access to public methods
    QMutex mutex;

    // Reaction map stack, according to the order in windowList
    // Positions for which there's no reaction map are filled with NULL
    //
    // NULL means that the application owning the window Id does not
    // have a reaction map (i.e., it does not use libmeegofeedback-reactionmaps)
    //
    // Ordered from top-to-bottom.
    // [0] has the top most reaction map and so on.
    QList<MfReactionMap*> stack;

    // How windows are currently being stacked.
    //
    // It's an ordered list of X Window Ids.
    // Gathered from last reorder() call
    // Ordered from bottom to top
    // [0] has the id of the window in the bottom of the stack
    QList<quint32> windowList;

    // Contains all added reaction maps, mapped by their window ids
    QHash<quint32, MfReactionMap*> reactionMapHash;

    // Contains all added reaction maps, but as a simple list
    QList<MfReactionMap*> reactionMapList;

    // Completely ignore the window stacking.
    // Just use the first reaction map we happen to have
    bool ignoreStacking;

    // Tells if the topmost window uses reaction map
    bool reactiveOnTop;
};

#endif
