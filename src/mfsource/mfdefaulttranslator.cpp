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

#include "mfdefaulttranslator.h"

#include <mfutil.h>

MfDefaultTranslator::MfDefaultTranslator()
{
    actualFingerId = 0;
    seenFingers.clear();
}

MfDefaultTranslator::~MfDefaultTranslator()
{
    qDeleteAll(Fingers.values());
    Fingers.clear();
    actualFingerId = 0;
}

void MfDefaultTranslator::addRawEvent(struct input_event* event)
{
    switch(event->type) {
        case EV_SYN:
            // Process touch point state here
            if (event->code == SYN_MT_REPORT) {
                tempPointState.timeStamp = event->time.tv_sec*1000+event->time.tv_usec/1000;
                seenFingers << actualFingerId;
                updateStates();
            } else
            if (event->code == SYN_REPORT) {
                // The number of SYN_MT_REPORT's between SYN_REPORT's indicate
                // how many fingers are touching the screen.
                // Iterate through the Finger hash to see which fingers are no
                // longer touching the screen.
                QHash<int, Finger*>::iterator fingerIter;
                for (fingerIter = Fingers.begin();
                     fingerIter != Fingers.end(); ++fingerIter)
                {
                    if (fingerIter.value() != 0 &&
                        seenFingers.contains(fingerIter.key()) == false &&
                        fingerIter.value()->state != FingerReleased) {
                        fingerIter.value()->state = FingerReleased;
                        if (timeStamping()) {
                            writeTimestamp("ts_release");
                        }
                        emit release(actualFingerId, QPoint(fingerIter.value()->x, fingerIter.value()->y));
                        qDebug("finger %d released", fingerIter.key());
                    }
                }
                seenFingers.clear();
            }
        break;

        // Other cases just build up the touch point state.
        case EV_ABS:
            if (event->code == ABS_MT_POSITION_X) {
                tempPointState.x = event->value;
                tempPointStateChanges.xData = true;
            } else
            if (event->code == ABS_MT_POSITION_Y) {
                tempPointState.y = event->value;
                tempPointStateChanges.yData = true;
            } else
            if (event->code == ABS_MT_TRACKING_ID) {
                actualFingerId = event->value;
            } else
            if (event->code == ABS_MT_TOUCH_MAJOR) {
                tempPointState.majorTouchingAxis = event->value;
                tempPointStateChanges.majorTouchingAxisData = true;
            } else {
                tempPointStateChanges.somethingData = true;
            }
        break;
    }
}


void MfDefaultTranslator::updateStates()
{
    // Add a new finger to the hash if it is not there
    if (!Fingers[actualFingerId]) {
        Finger* tempFinger = new Finger();

        tempFinger->state = FingerReleased;
        tempFinger->timeStamp = 0;
        tempFinger->x = 0;
        tempFinger->y = 0;
        Fingers[actualFingerId] = tempFinger;
    }

    // Detect the press events
    if (tempPointStateChanges.xData && tempPointStateChanges.yData &&
        Fingers[actualFingerId]->state == FingerReleased)
    {
        Fingers[actualFingerId]->state = FingerPressed;
        Fingers[actualFingerId]->x = tempPointState.x;
        Fingers[actualFingerId]->y = tempPointState.y;
        Fingers[actualFingerId]->timeStamp = tempPointState.timeStamp;
        if (timeStamping()) {
            writeTimestamp("ts_press");
        }
        emit press(actualFingerId, QPoint(Fingers[actualFingerId]->x, Fingers[actualFingerId]->y));
        qDebug("finger %d pressed", actualFingerId);
        goto end;
    }

    // Detect the moving of the fingers
    if (Fingers[actualFingerId]->state == FingerPressed ||
        Fingers[actualFingerId]->state == FingerMoving)
    {
        // Move to the state moving if the coordinates are too different
        if (Fingers[actualFingerId]->state == FingerPressed &&
            qAbs(tempPointState.x-Fingers[actualFingerId]->x)+
            qAbs(tempPointState.y-Fingers[actualFingerId]->y) > 10)
        {
            Fingers[actualFingerId]->state = FingerMoving;
            emit moving(actualFingerId);
            qDebug("finger %d moving", actualFingerId);
        }
        // Move back to the state press if the coordinates are quite similar
        if (Fingers[actualFingerId]->state == FingerMoving &&
            qAbs(tempPointState.x-Fingers[actualFingerId]->x)+
            qAbs(tempPointState.y-Fingers[actualFingerId]->y) < 5)
        {
            Fingers[actualFingerId]->state = FingerPressed;
            emit stopped(actualFingerId);
            qDebug("finger %d stopped", actualFingerId);
        }
        Fingers[actualFingerId]->timeStamp = tempPointState.timeStamp;
        // Some hardware report the position of the other finger wrongly
        // Let's filter it, but it works only for two fingers
        if ((actualFingerId == 0 && (!Fingers[1] || Fingers[1]->state == FingerReleased ||
            qAbs(tempPointState.x-Fingers[1]->x)+
            qAbs(tempPointState.y-Fingers[1]->y) > 20)) ||
            (actualFingerId == 1 && (!Fingers[0] || Fingers[0]->state == FingerReleased ||
            qAbs(tempPointState.x-Fingers[0]->x)+
            qAbs(tempPointState.y-Fingers[0]->y) > 20))) {
            Fingers[actualFingerId]->x = tempPointState.x;
            Fingers[actualFingerId]->y = tempPointState.y;
        }
    }

end:
    // Clear the temporary point state storage
    tempPointStateChanges.xData = false;
    tempPointStateChanges.yData = false;
    tempPointStateChanges.majorTouchingAxisData = false;
    tempPointStateChanges.somethingData = false;
}
