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

#include "mfsettings.h"

#include <QSettings>

bool MfSettings::_loaded = false;
int MfSettings::_reactionMapWidth = -1;
int MfSettings::_reactionMapHeight = -1;
bool MfSettings::_ignoreStacking = false;

int MfSettings::reactionMapWidth()
{
    if (!_loaded) {
        load();
    }

    return _reactionMapWidth;
}

int MfSettings::reactionMapHeight()
{
    if (!_loaded) {
        load();
    }

    return _reactionMapHeight;
}

bool MfSettings::ignoreStacking()
{
    if (!_loaded) {
        load();
    }

    return _ignoreStacking;
}

void MfSettings::load()
{
    QSettings settings("/etc/meegofeedbackd/reactionmaps.conf", QSettings::NativeFormat);

    _reactionMapWidth = settings.value("reaction-map-width", QVariant(216)).toInt();
    _reactionMapHeight = settings.value("reaction-map-height", QVariant(120)).toInt();
    _ignoreStacking = settings.value("ignore-stacking", QVariant(false)).toBool();

    _loaded = true;
}
