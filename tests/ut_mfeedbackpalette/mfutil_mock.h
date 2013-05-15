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

#ifndef MFUTILMOCK_H
#define MFUTILMOCK_H

#include <QString>

/* Returns a list of string which contains the names of the feedbacks according
 * to base theme and application name. In other words returns the available feedback
 * names for the given application.
 */
QStringList getFeedbackNames(const QString& appName);

#endif
