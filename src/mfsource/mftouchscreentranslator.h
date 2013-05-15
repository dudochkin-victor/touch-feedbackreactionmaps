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

#ifndef MFTOUCHSCREENTRANSLATOR_H
#define MFTOUCHSCREENTRANSLATOR_H

#include <linux/input.h>

#include <QObject>
#include <QPoint>

class MfTouchScreenTranslator : public QObject
{
    Q_OBJECT

signals:
    void press(int fingerId, const QPoint& position);
    void release(int fingerId, const QPoint& position);
    void moving(int fingerId);
    void stopped(int fingerId);
public:
    virtual void addRawEvent(struct input_event* event) = 0;
};

#endif
