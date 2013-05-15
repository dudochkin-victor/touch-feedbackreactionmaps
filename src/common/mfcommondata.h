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

#ifndef MFCOMMONDATA_H
#define MFCOMMONDATA_H

typedef struct
{
    quint8 pressFeedbackId;
    quint8 releaseFeedbackId;
} MfPaletteEntry;

// The following defines are needed for communication between
// the client and the server
#define MF_REQUEST_ADD      0
#define MF_REQUEST_UPDATE   1
#define MF_REQUEST_DELETE   2
// Description of communication between reaction map client process
// and server process:
//
// Initialization:
// Every time the client process connects to the server, the first
// thing communicated to the server is the PID of the client process
// (type: qint64). This must also happen after the client process has
// been disconnected from the server for some reason and needs to
// reconnect. In fact, whenever the connection to the server is broken,
// all data about added reaction maps must be communicated to the server
// again.
//
// Data flow from client to server:
// After initialization the client can send three types of requests
// to the server. Each request begins with the request header (as
// defined above, type: quint8) and some parameters follow.
//   MF_REQUEST_ADD:
//   This request is sent when a new reaction map is instantiated.
//   After the header comes three parameters:
//    - Unique identifier for the created reaction map. (type: quint32)
//    - Application name that is used to load all custom feedbacks that
//      the application may have installed. (type: QString)
//    - Top level window ID for the reaction map. (type: quint32)
//   MF_REQUEST_UPDATE:
//   This request is sent when the top level window ID for the reaction
//   map has been changed.
//   After the header comes two parameters:
//    - Unique identifier for the created reaction map. (type: quint32)
//    - Top level window ID for the reaction map. (type: quint32)
//   MF_REQUEST_DELETE:
//   This request is sent when the reaction map in question has been removed.
//   After the header comes one parameter:
//    - Unique identifier for the created reaction map. (type: quint32)
//
// Data flow from server to client:
// Some time after the server has been requested a new reaction map, the
// server sends the shared memory area information reserved for the reaction
// map to the client. The format of the information is as follows:
//  - Unique identifier for the created reaction map. This is the very
//    same identifier as was given in MF_REQUEST_ADD. (type: quint32)
//  - Shared memory information that is a path to a file. (type: QString)
//

#endif
