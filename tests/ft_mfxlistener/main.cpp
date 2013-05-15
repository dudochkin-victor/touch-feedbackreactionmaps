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

#include <mfxlistener.h>
#include <QCoreApplication>
#include <QStringList>
#include "logger.h"

#include <QTimer>
#include <QtDebug>

static void processCmdLineArgs(bool &nonstop);

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    Logger logger;
    MfXListener xListener(NULL);
    bool ok;
    bool nonstop = false;

    processCmdLineArgs(nonstop);

    ok = logger.connect(&xListener, SIGNAL(windowStackChanged(QList<quint32>)),
                        SLOT(logWindowStackChanged(QList<quint32>)));
    if (!ok) {
        qFatal("Unable to connect signals!");
    }

    if (nonstop) {
        xListener.startListening();
        app.exec();
        xListener.stopListening();
    } else {
        for (int i=0; i < 2; i++) {
            qDebug() << "Run #" << (i+1);

            // Quit after 5 seconds. MfXListener should stop its thread nicely
            QTimer::singleShot(5000, &app, SLOT(quit()));

            app.exec();

        }
    }
}

static void processCmdLineArgs(bool &nonstop)
{
    QStringList args = QCoreApplication::arguments();

    if (args.size() == 2) {
        QString arg = args[1];
        nonstop = arg == "nonstop";
    }
}
