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

#include "ut_mfreactionmapstack.h"

#include <QHash>

#include "mfreactionmapstack.h"

#include "mfsession_mock.h"

QTEST_MAIN(Ut_MfReactionMapStack)

Ut_MfReactionMapStack::Ut_MfReactionMapStack(QObject *parent)
    : QObject(parent),
      fooFeedback("foo"), barFeedback("bar"),
      homeReactionMap(0), vkbReactionMap(0)
{
}

void Ut_MfReactionMapStack::init()
{
    MfSession *session = 0;

    session = new MfSession(QString("mhome"));
    session->feedbackHash[fooFeedback.name()] = &fooFeedback;
    homeReactionMap = new MfReactionMap(session, 123, 7000);
    QVERIFY(homeReactionMap->init());
    session = 0; // reaction map took ownership of the session.

    session = new MfSession(QString("meego-keyboard"));
    session->feedbackHash[barFeedback.name()] = &barFeedback;
    vkbReactionMap = new MfReactionMap(session, 456, 7010);
    QVERIFY(vkbReactionMap->init());
    session = 0; // reaction map took ownership of the session.
}

void Ut_MfReactionMapStack::cleanup()
{
    fooFeedback.timesPlayed = 0;
    barFeedback.timesPlayed = 0;

    delete homeReactionMap;
    homeReactionMap = 0;

    delete vkbReactionMap;
    vkbReactionMap = 0;
}

/*
 * There's one reaction map and its corresponding window is currently on
 * the stack. Therefore this reaction map should be used to find the appropriate
 * input feedback.
 */
void Ut_MfReactionMapStack::pressOneReactionMap()
{
    MfReactionMapStack reactionMapStack;
    QList<quint32> windowList;
    QPoint pressPosition(10, 10);

    reactionMapStack.add(homeReactionMap);

    // Say that there's only home screen's window in the window stack.
    windowList << homeReactionMap->windowId();
    reactionMapStack.reorder(windowList);

    // Feedback "foo" should be given when that position is pressed
    homeReactionMap->setPixel(pressPosition, "foo", "");

    // Say that the user has pressed the screen on that same position.
    reactionMapStack.pressed(pressPosition);

    // Check that the feedback was played.
    QCOMPARE(fooFeedback.timesPlayed, 1);
}

/*
 * One reaction map is present but there are no windows currently being
 * stacked. Therefore this reaction map shouldn't be queried at all.
 */
void Ut_MfReactionMapStack::pressOneReactionMapNoWindowStack()
{
    MfReactionMapStack reactionMapStack;
    QList<quint32> windowList;
    QPoint pressPosition(10, 10);

    reactionMapStack.add(homeReactionMap);

    // Say that no windows are currently being shown/stacked.
    reactionMapStack.reorder(windowList);

    // Feedback "foo" should be given when that position is pressed
    homeReactionMap->setPixel(pressPosition, "foo", "");

    // Say that the user has pressed the screen on that same position.
    reactionMapStack.pressed(pressPosition);

    // Check that no feedback was played.
    QCOMPARE(fooFeedback.timesPlayed, 0);
}

/*
 * There are two reaction maps, on stacked on top of the other.
 *
 * User presses over a position that is transparent on the top-most
 * reaction map. Therefore the value on the top-most reaction map should be
 * ignored and the value from the reaction map below should be used instead.
 */
void Ut_MfReactionMapStack::pressTransparentReactionMap()
{
    MfReactionMapStack reactionMapStack;
    QList<quint32> windowList;
    QPoint pressPosition(10, 10);

    reactionMapStack.add(homeReactionMap);
    reactionMapStack.add(vkbReactionMap);

    // Virtual keyboard is in front of home screen
    windowList << homeReactionMap->windowId();
    windowList << vkbReactionMap->windowId();
    reactionMapStack.reorder(windowList);

    // Feedback "foo" should be given when that position is pressed
    homeReactionMap->setPixel(pressPosition, "foo", "");

    // Virtual keyboard is transparent on that position
    vkbReactionMap->setPixel(pressPosition, "transparent", "transparent");

    // Say that the user has pressed the screen on that same position.
    reactionMapStack.pressed(pressPosition);

    // Check that home screen feedback was played.
    QCOMPARE(fooFeedback.timesPlayed, 1);
    QCOMPARE(barFeedback.timesPlayed, 0);
}

/*
 * There are two reaction maps, on stacked on top of the other.
 *
 * User presses over a position that is not transparent on the top-most
 * reaction map. Therefore the value on the top-most reaction map should be
 * used and not the value from the reaction map below.
 */
void Ut_MfReactionMapStack::pressNonTransparentReactionMap()
{
    MfReactionMapStack reactionMapStack;
    QList<quint32> windowList;
    QPoint pressPosition(10, 10);

    reactionMapStack.add(homeReactionMap);
    reactionMapStack.add(vkbReactionMap);

    // Virtual keyboard is in front of home screen
    windowList << homeReactionMap->windowId();
    windowList << vkbReactionMap->windowId();
    reactionMapStack.reorder(windowList);

    // Feedback "foo" should be given when that position is pressed
    homeReactionMap->setPixel(pressPosition, "foo", "");

    // Virtual keyboard is NOT transparent on that position
    vkbReactionMap->setPixel(pressPosition, "bar", "");

    // Say that the user has pressed the screen on that same position.
    reactionMapStack.pressed(pressPosition);

    // Check that virtual keyboard feedback was played.
    QCOMPARE(fooFeedback.timesPlayed, 0);
    QCOMPARE(barFeedback.timesPlayed, 1);
}

/*!
 * Press a window that does not have a reaction map.
 *
 * Even though there's a window below it which does have a reaction map
 * no feedback should be given.
 *
 * An absent reaction map work as reaction map filled with
 * MfReactionMap::setPixel("", "")
 * Meaning no press or release feedback and non-transparent.
 */
void Ut_MfReactionMapStack::pressWindowNoReactionMap()
{
    MfReactionMapStack reactionMapStack;
    QList<quint32> windowList;
    QPoint pressPosition(10, 10);

    reactionMapStack.add(homeReactionMap);

    // Say that there's a window on top of home screen for which there's no
    // reaction map
    windowList << homeReactionMap->windowId();
    windowList << 1; // an unknown window id.
    reactionMapStack.reorder(windowList);

    // Feedback "foo" should be given when that position is pressed
    homeReactionMap->setPixel(pressPosition, "foo", "");

    // Say that the user has pressed the screen on that same position.
    reactionMapStack.pressed(pressPosition);

    // Check that NO feedback was played.
    QCOMPARE(fooFeedback.timesPlayed, 0);
}

/*
 * Basic test for the release case. Since it follows the very same logic of
 * the press case, no need to duplicate all tests again for it.
 */
void Ut_MfReactionMapStack::release()
{
    MfReactionMapStack reactionMapStack;
    QList<quint32> windowList;
    QPoint releasePosition(10, 10);

    reactionMapStack.add(homeReactionMap);

    // Say that there's only home screen's window in the window stack.
    windowList << homeReactionMap->windowId();
    reactionMapStack.reorder(windowList);

    // Feedback "foo" should be given when finger is raised from that position
    homeReactionMap->setPixel(releasePosition, "", "foo");

    // Say that the user has raised his finger from that same position.
    reactionMapStack.released(releasePosition);

    // Check that the feedback was played.
    QCOMPARE(fooFeedback.timesPlayed, 1);
}
