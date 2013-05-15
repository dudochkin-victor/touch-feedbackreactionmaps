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

#include "colormap.h"

#include <QFile>

QList<ColorMap> ColorMap::loadFromFile(const QString &filePath)
{
    QList<ColorMap> list;
    QFile file(filePath);

    if (filePath.size() == 0) {
        return list;
    }

    if (!file.exists()) {
        return list;
    }

    return loadFromIODevice(file);
}

QList<ColorMap> ColorMap::loadFromIODevice(QIODevice &ioDevice)
{
    QList<ColorMap> list;
    QByteArray lineBytes;
    QString lineString;
    ColorMap colorMap;

    if (!ioDevice.open(QIODevice::ReadOnly)) {
        qCritical("Unable to open the io device.");
        return list;
    }

    do {
        lineBytes = ioDevice.readLine();
        if (lineBytes.size() > 0) {
            lineString.clear();
            lineString.append(lineBytes);
            if (colorMap.load(lineString)) {
                list << colorMap;
            }
        }
    } while(lineBytes.size() > 0);

    ioDevice.close();

    return list;
}

bool ColorMap::load(const QString& string)
{
    QStringList parts = string.split('=');
    bool ok;

    if (parts.size() != 2) {
        return false;
    }

    ok = loadFeedbackNames(parts[0]);
    if (!ok) return false;

    ok = loadColor(parts[1]);
    if (!ok) return false;

    return true;
}

bool ColorMap::loadFeedbackNames(const QString& string)
{
    QStringList parts = string.split(',');

    if (parts.size() != 2) {
        return false;
    }

    _pressFeedbackName = parts[0];
    _releaseFeedbackName = parts[1];

    return true;
}

bool ColorMap::loadColor(const QString& string)
{
    QStringList parts = string.split(',');
    bool ok;
    int red;
    int green;
    int blue;

    if (parts.size() != 3) {
        return false;
    }

    red = parts[0].toInt(&ok);
    if (!ok) return false;
    if (red < 0 || red > 255) return false;

    green = parts[1].toInt(&ok);
    if (!ok) return false;
    if (green < 0 || green > 255) return false;

    blue = parts[2].toInt(&ok);
    if (!ok) return false;
    if (blue < 0 || blue > 255) return false;

    _color = qRgb(red, green, blue);

    return true;
}
