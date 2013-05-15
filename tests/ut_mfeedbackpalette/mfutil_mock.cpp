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

#include "mfutil.h"

#include <QStringList>

QStringList getFeedbackNames(const QString& appName)
{
    Q_UNUSED(appName);

    QStringList finalFeedbackDirList;

    // Make sure the feedbacks are not in aplhabetical order
    finalFeedbackDirList << "feedback-2" << "feedback-4"
                         << "feedback-6" << "feedback-8"
                         << "feedback-7" << "feedback-5"
                         << "feedback-3" << "feedback-1";

    return finalFeedbackDirList;
}


