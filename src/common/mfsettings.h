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

#ifndef MFSETTINGS_H
#define MFSETTINGS_H

class MfSettings {
public:
    static int reactionMapWidth();
    static int reactionMapHeight();

    // Completely ignore the window stacking.
    // Just use the first reaction map we happen to have
    static bool ignoreStacking();

private:
    static void load();

    static bool _loaded;
    static int _reactionMapWidth;
    static int _reactionMapHeight;
    static bool _ignoreStacking;
};
#endif
