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

#ifndef MFXLISTENER_P_H
#define MFXLISTENER_P_H

#include <QObject>
#include <QDataStream>
#include <QTextStream>
#include <QMetaType>
#include <QList>

#include <xcb/xcb.h>
#include <xcb/xproto.h>

class MfReactionMapStack;

class MfXListenerPriv : public QObject {
    Q_OBJECT
public:
    MfXListenerPriv(MfReactionMapStack *reactionMapStack);
    ~MfXListenerPriv();

    bool init();
    bool processXEvents();
    bool checkNetClientListStackingProperty();
    bool isWindowIgnored(quint32 windowId);
    xcb_connection_t* getConnection();
    xcb_screen_t* screenOfDisplay(xcb_connection_t *conn, int screen);

    xcb_window_t rootWindow;
    xcb_atom_t netClientListStackingAtom;
    xcb_atom_t netWindowTypeAtom;
    xcb_atom_t netWindowTypeNotificationAtom;

    QList<quint32> stackedWindows;
    MfReactionMapStack *reactionMapStack;

signals:
    void windowStackChanged(QList<quint32> newWindowStack);
};

#endif
