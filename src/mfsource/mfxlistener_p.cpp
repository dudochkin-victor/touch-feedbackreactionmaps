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

#include "mfxlistener_p.h"
#include "mfreactionmapstack.h"

#include <QDebug>

namespace {
    static xcb_connection_t* connection = NULL;
}

MfXListenerPriv::MfXListenerPriv(MfReactionMapStack *reactionMapStack)
    : rootWindow(0), netClientListStackingAtom(0), netWindowTypeAtom(0),
      netWindowTypeNotificationAtom(0), reactionMapStack(reactionMapStack)
{
}

MfXListenerPriv::~MfXListenerPriv()
{
    if (connection) {
        xcb_disconnect(connection);
        connection = NULL;
    }
}

xcb_connection_t* MfXListenerPriv::getConnection()
{
    return connection;
}

bool MfXListenerPriv::isWindowIgnored(quint32 windowId)
{
    xcb_get_property_cookie_t cookie;
    xcb_get_property_reply_t *propertyReply = NULL;
    xcb_atom_t *windowTypeData = NULL;
    int nitems = 0;
    bool retVal = false;

    cookie = xcb_get_property(connection, 0, windowId, netWindowTypeAtom, XCB_ATOM_ATOM, 0, (uint32_t)-1);
    propertyReply = xcb_get_property_reply(connection, cookie, NULL);

    if (propertyReply) {
        if (propertyReply->response_type == XCB_NONE) {
            // Window must be ignored since it no longer exists
            nitems = 0;
            retVal = true;
        } else if (propertyReply->type != XCB_ATOM_ATOM ||
                   propertyReply->bytes_after > 0) {
            qCritical("MfXListener: Error reading reply for _NET_WM_WINDOW_TYPE "
                      "property request for window ID 0x%08x", windowId);
            nitems = 0;
        } else {
            windowTypeData = (xcb_atom_t*)xcb_get_property_value(propertyReply);
            nitems = xcb_get_property_value_length(propertyReply) / 4;
        }
    } else {
        qWarning("MfXListener: Could not get _NET_WM_WINDOW_TYPE property for "
                 "window ID 0x%08x", windowId);
    }

    // Ignore window if it is of type _NET_WM_WINDOW_TYPE_NOTIFICATION
    for (int i = 0; i < nitems; ++i) {
        if (windowTypeData[i] == netWindowTypeNotificationAtom) {
            retVal = true;
            break;
        }
    }

    if (propertyReply) {
        free(propertyReply);
        propertyReply = NULL;
    }

    return retVal;
}

xcb_screen_t* MfXListenerPriv::screenOfDisplay(xcb_connection_t *conn, int screen)
{
    xcb_screen_iterator_t iter;

    iter = xcb_setup_roots_iterator(xcb_get_setup (conn));
    for (; iter.rem; --screen, xcb_screen_next(&iter)) {
        if (screen == 0) {
            return iter.data;
        }
    }

    return NULL;
}

bool MfXListenerPriv::checkNetClientListStackingProperty()
{
    quint32 *stackedWindowsReturned = NULL;
    int windowCount = 0;
    QList<quint32> stackedWindowsNew;
    xcb_get_property_cookie_t cookie;
    xcb_get_property_reply_t *propertyReply = NULL;

    cookie = xcb_get_property(connection, 0, rootWindow, netClientListStackingAtom, XCB_ATOM_WINDOW, 0, (uint32_t)-1);
    propertyReply = xcb_get_property_reply(connection, cookie, NULL);

    if (propertyReply) {
        if (propertyReply->response_type == XCB_NONE) {
            qCritical("MfXListener: Root window does not have"
                      " _NET_CLIENT_LIST_STACKING property");
            goto ERROR;
        } else if (propertyReply->type != XCB_ATOM_WINDOW) {
            qCritical("MfXListener: _NET_CLIENT_LIST_STACKING property type"
                      "is not XCB_ATOM_WINDOW");
            goto ERROR;
        } else if (propertyReply->bytes_after > 0) {
            qCritical() << "MfXListener: Unexpected behaviour:" << propertyReply->bytes_after
                        << "bytes still unread after return.\n";
            goto ERROR;
        }

        stackedWindowsReturned = (quint32*)xcb_get_property_value(propertyReply);
        windowCount = xcb_get_property_value_length(propertyReply) / 4;
    } else {
        qCritical("MfXListener: Could not get _NET_CLIENT_LIST_STACKING property");
        goto ERROR;
    }

    for (unsigned int i = windowCount; i > 0; --i) {
        // Skip windows with certain type
        if (isWindowIgnored(stackedWindowsReturned[i-1]) == false) {
            // Add window to the beginning of stack
            stackedWindowsNew.prepend(stackedWindowsReturned[i-1]);

            // Stop adding windows when first window without reaction map
            // is encountered. The list ends with first window without
            // associated reaction map.
            if (reactionMapStack->hasWindowReactionMap(stackedWindowsReturned[i-1]) == false) {
                break;
            }
        }
    }

    // Notify about changed stacking order
    if (stackedWindowsNew != stackedWindows) {
        stackedWindows = stackedWindowsNew;
        emit windowStackChanged(stackedWindows);
    }

    // Free returned data
    if (propertyReply) {
        free(propertyReply);
        propertyReply = NULL;
    }

    return true;

    ERROR:
    if (propertyReply) {
        free(propertyReply);
        propertyReply = NULL;
    }
    return false;
}

bool MfXListenerPriv::init()
{
    const static uint32_t valueList[] = { XCB_EVENT_MASK_PROPERTY_CHANGE };
    const char *atomNames[3] = { "_NET_CLIENT_LIST_STACKING", "_NET_WM_WINDOW_TYPE", "_NET_WM_WINDOW_TYPE_NOTIFICATION" };
    xcb_atom_t atoms[3];
    xcb_intern_atom_cookie_t cookies[3];
    int screen_num = 0;
    xcb_screen_t *screen;
    xcb_generic_error_t *error;
    xcb_void_cookie_t reply;

    connection = xcb_connect(NULL, &screen_num);

    if (xcb_connection_has_error(connection)) {
        qWarning("MfXListener: Cannot connect to X server");
        return false;
    }

    screen = screenOfDisplay(connection, screen_num);
    if (screen) {
        rootWindow = screen->root;
    } else {
        qWarning("MfXListener: Cannot find default screen");
        xcb_disconnect(connection);
        return false;
    }

    for (int i = 0; i < 3; ++i) {
        cookies[i] = xcb_intern_atom(connection, 0, strlen(atomNames[i]), atomNames[i]);
    }
    for (int i = 0; i < 3; ++i) {
        xcb_intern_atom_reply_t *atomReply = NULL;
        atomReply = xcb_intern_atom_reply(connection, cookies[i], NULL);
        if (atomReply) {
            atoms[i] = atomReply->atom;
            free (atomReply);
        } else {
            qCritical("MfXListener: Failed to intern atoms.");
            xcb_disconnect(connection);
            return false;
        }
    }

    netClientListStackingAtom = atoms[0];
    netWindowTypeAtom = atoms[1];
    netWindowTypeNotificationAtom = atoms[2];

    reply = xcb_change_window_attributes_checked(connection, rootWindow, XCB_CW_EVENT_MASK, valueList);
    error = xcb_request_check(connection, reply);
    if (error) {
        qCritical("MfXListener: Failed to change root window attributes.");
        xcb_disconnect(connection);
        return false;
    }

    // Get the initial value and emit change signal for the first time.
    checkNetClientListStackingProperty();

    return !xcb_connection_has_error(connection);
}

bool MfXListenerPriv::processXEvents()
{
    xcb_generic_event_t *event = NULL;

    while ((event = xcb_poll_for_event(connection)) != NULL) {
        if (event->response_type == XCB_PROPERTY_NOTIFY) {
            xcb_property_notify_event_t *prop = (xcb_property_notify_event_t*)event;
            if (prop->atom == netClientListStackingAtom) {
                checkNetClientListStackingProperty();
            }
        }
        free(event);
    }

    return !xcb_connection_has_error(connection);
}
