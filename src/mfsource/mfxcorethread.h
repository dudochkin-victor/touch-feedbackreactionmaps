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

#ifndef MFXCORETHREAD_H
#define MFXCORETHREAD_H

#include <QThread>

#include <X11/Xlib.h>
#include <X11/Xproto.h>

class MfXCoreThread : public QThread
{
    Q_OBJECT
public:
    MfXCoreThread(QObject *parent = 0);

protected:
    void run();

private:
    bool setup();
    bool listen();
    Display *display;
};

#endif
