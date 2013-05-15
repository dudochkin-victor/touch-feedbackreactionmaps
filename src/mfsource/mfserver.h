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

#ifndef MFSERVER_H
#define MFSERVER_H

#include <QLocalServer>

class MfReactionMapStack;

class MfServer : public QObject
{
    Q_OBJECT
public:
    explicit MfServer(MfReactionMapStack *stack, QObject *parent = 0);

    bool init();

private slots:
    void onNewConnection();

private:
    QLocalServer socketServer;
    MfReactionMapStack *stack;

    /*
     * Clean up shared memory segments left by previous (crashed) instances.
     */
    void removePreExistingSharedMemory();
};

#endif
