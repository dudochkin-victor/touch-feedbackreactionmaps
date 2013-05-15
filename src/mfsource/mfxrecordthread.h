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

#ifndef MFXRECORDTHREAD_H
#define MFXRECORDTHREAD_H

#include <QThread>
#include <QPoint>

#include <X11/Xlib.h>
#include <X11/extensions/record.h>
#include <X11/Xproto.h>

class MfReactionMapStack;

class MfXRecordThread : public QThread
{
    Q_OBJECT
public:
    MfXRecordThread(QObject *parent = 0);
    ~MfXRecordThread();

    static MfXRecordThread *instance();

    bool init(MfReactionMapStack *reactionMapStack,
              Display *display, XRecordContext context);

public slots:
    void screenPressed(int x, int y);
    void screenReleased(int x, int y);

protected:
    void run();

private:
    bool createContext();
    QPoint calcPixelCoords(int x, int y);

    Display *display;
    XRecordContext context;
    MfReactionMapStack *reactionMapStack;
    int displayWidth, displayHeight;

    Q_DISABLE_COPY(MfXRecordThread)
};

#endif
