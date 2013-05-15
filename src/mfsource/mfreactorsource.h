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

#ifndef MFREACTORSOURCE_H
#define MFREACTORSOURCE_H

#include <QObject>
#include <mfsourcebase.h>

#include "mfreactionmapstack.h"

class MfServer;
class MfXListener;
class MfTouchScreenListener;

class MfReactorSource : public MfSourceBase
{
    Q_OBJECT
    Q_INTERFACES(MfSourceInterface)
public:
    MfReactorSource();
    virtual ~MfReactorSource();

    // Exported method (defined in MfSourceBase)
    bool init();

    // Internal method
    bool isActive() const;

public slots:
    // Exported method (defined in MfSourceBase)
    void deviceStateChanged(const QMap<QString, QString> &newState);

signals:
    void stateChanged(bool active);

private slots:
    void updateState();

private:
    MfTouchScreenListener *touchScreenListener;
    MfServer *server;
    MfReactionMapStack reactionMapStack;
    MfXListener *xListener;
    bool reactionMapActive;
    bool displayActive;
    bool initialized;
};

#endif
