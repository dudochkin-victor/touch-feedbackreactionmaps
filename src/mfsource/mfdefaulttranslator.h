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

#ifndef MFDEFAULTTRANSLATOR_H
#define MFDEFAULTTRANSLATOR_H

#include "mftouchscreentranslator.h"

#include <QHash>

class MfDefaultTranslator : public MfTouchScreenTranslator
{
    Q_OBJECT

    enum FingerState
    {
        FingerReleased = 0,
        FingerPressed,
        FingerMoving
    };

    struct Finger
    {
        FingerState state;
        int x;
        int y;
        int timeStamp;
    };

    struct TouchPointState {
        int x;
        int y;
        int majorTouchingAxis;
        int timeStamp;
    };

    struct TouchPointStateChanges {
        bool xData;
        bool yData;
        int majorTouchingAxisData;
        bool somethingData;
    };

public:
    explicit MfDefaultTranslator();
    ~MfDefaultTranslator();

public:
    void addRawEvent(struct input_event* event);
    void updateStates();

private:

    // Internal variables
    QHash<int, Finger*> Fingers;
    QList<int> seenFingers;
    TouchPointState tempPointState;
    TouchPointStateChanges tempPointStateChanges;
    int actualFingerId;
};

#endif
