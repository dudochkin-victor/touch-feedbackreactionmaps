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

#ifndef COLORMAP_H
#define COLORMAP_H

#include <QColor>
#include <QList>
#include <QString>

class ColorMap
{
public:
    ColorMap() : _color(qRgb(0,0,0)) {}
    ColorMap(const QString &pressFeedback,
             const QString &releaseFeedback,
             QRgb color) : _pressFeedbackName(pressFeedback),
                           _releaseFeedbackName(releaseFeedback),
                           _color(color) {}

    static QList<ColorMap> loadFromFile(const QString &filePath);

    static QList<ColorMap> loadFromIODevice(QIODevice &ioDevice);

    const QString &pressFeedbackName() const {return _pressFeedbackName;}
    const QString &releaseFeedbackName() const {return _releaseFeedbackName;}
    QRgb color() const {return _color;}

private:
    // loads values from a string with format:
    // PRESS-FEEDBACK-NAME,RELEASE-FEEDBACK-NAME=RED,GREEN,BLUE
    bool load(const QString& string);

    // loads feedback names from a string with format:
    // PRESS-FEEDBACK-NAME,RELEASE-FEEDBACK-NAME
    bool loadFeedbackNames(const QString& string);

    // loads color value from a string with format:
    // RED,GREEN,BLUE
    bool loadColor(const QString& string);

    QString _pressFeedbackName;
    QString _releaseFeedbackName;
    QRgb _color;
};

#endif
