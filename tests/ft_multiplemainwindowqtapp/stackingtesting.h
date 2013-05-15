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

#ifndef STACKINGTESTING_H
#define STACKINGTESTING_H

#include <QApplication>

class QPushButton;

class StackingTesting : public QApplication
{
    Q_OBJECT

public:
    StackingTesting(int argc, char **argv);
    virtual ~StackingTesting();

public slots:
    void changeWindows();

private:
    QPushButton *window1;
    QPushButton *window2;

};

#endif
