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

#ifndef MFFEEDBACK_MOCK_H
#define MFFEEDBACK_MOCK_H

#include <QObject>
#include <QString>

class MfFeedback : public QObject {
    Q_OBJECT

public:
    MfFeedback(const QString &name) : QObject(0) { Q_UNUSED(name); }
    virtual ~MfFeedback() {}

    QString name() const {return QString();}

    bool operator== (const MfFeedback &other) const { Q_UNUSED(other); return true;}

    void emitPlay(qint64 timestamp) { Q_UNUSED(timestamp); }

private slots:
    void play(qint64 timestamp) { Q_UNUSED(timestamp); }

};

#endif
