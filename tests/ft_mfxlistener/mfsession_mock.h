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

#ifndef MFSESSION_MOCK_H
#define MFSESSION_MOCK_H

#include <QObject>
#include <QString>
#include <QHash>

class MfFeedback;

class MfSession : public QObject
{
    Q_OBJECT
public:
    MfSession(const QString &applicationName, QObject *parent = 0)
        : QObject(parent), _applicationName(applicationName) {}

    virtual ~MfSession() {}

    QHash<QString, MfFeedback*> feedbackHash;

    const QString &applicationName() {return _applicationName;}
private:
    QString _applicationName;
};

#endif
