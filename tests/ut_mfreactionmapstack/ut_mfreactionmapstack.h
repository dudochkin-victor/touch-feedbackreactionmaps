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

#ifndef UT_MFREACTIONMAPSTACK_H
#define UT_MFREACTIONMAPSTACK_H

#include <QtTest/QtTest>
#include <QObject>

#include "mfreactionmap.h"

#include "mffeedback_mock.h"

class Ut_MfReactionMapStack : public QObject
{
    Q_OBJECT
public:
    Ut_MfReactionMapStack(QObject *parent = 0);

private slots:
    void initTestCase(){}
    void cleanupTestCase(){}

    void init();
    void cleanup();

    void pressOneReactionMap();
    void pressOneReactionMapNoWindowStack();
    void pressTransparentReactionMap();
    void pressNonTransparentReactionMap();
    void pressWindowNoReactionMap();
    void release();

private:
    MfFeedback fooFeedback;
    MfFeedback barFeedback;
    MfReactionMap *homeReactionMap;  // Home screen reaction map
    MfReactionMap *vkbReactionMap;  // Virtual keyboard reaction map
};

#endif
