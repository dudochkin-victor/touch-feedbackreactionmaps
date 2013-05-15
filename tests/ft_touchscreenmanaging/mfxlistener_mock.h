/* This file is part of meegofeedbackd
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

#ifndef MFXLISTENERMOCK_H
#define MFXLISTENERMOCK_H

#include <QObject>
#include <QList>

class MfReactionMapStack;

class MfXListenerMock : public QObject
{
    Q_OBJECT

public:
    MfXListenerMock(MfReactionMapStack *reactionMapStack, QObject *parent = 0) : QObject(parent) { Q_UNUSED(reactionMapStack); };
    virtual ~MfXListenerMock() {};

signals:
    void windowStackChanged(QList<quint32> newWindowStack);

public slots:
    void emitWindowStackChanged(QList<quint32> newWindowStack);
};

extern MfXListenerMock stackEmitter;

#endif
