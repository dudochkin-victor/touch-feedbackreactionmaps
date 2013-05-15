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

#include "mreactionmapviewer.h"

#include <QApplication>
#include <QColor>
#include <QFileInfo>
#include <iostream>

using namespace std;

static bool processArguments(int &clientPid, int &mapIndex, QString &colorMappingFilePath, int &refreshTimeMs);
static void printUsage();

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MReactionMapViewer viewer;
    int clientPid = -1;
    int refreshTimeMs;
    int mapIndex;
    QString colorMappingFilePath;

    // Force the raster graphics system to avoid crashing
    QApplication::setGraphicsSystem("raster");

    if (!processArguments(clientPid, mapIndex, colorMappingFilePath, refreshTimeMs)) {
        return 1;
    }

    if (!viewer.init(clientPid, mapIndex, refreshTimeMs, colorMappingFilePath)) {
        return 1;
    }

    cout << "Updating screen every " << refreshTimeMs << " milliseconds." << endl;

    viewer.showFullScreen();

    app.exec();

    return 0;
}

static bool processArguments(int &clientPid, int &mapIndex, QString &colorMappingFilePath, int &refreshTimeMs)
{
    QStringList arguments;
    bool ok = false, conversionOk = false;

    arguments = QCoreApplication::arguments();

    // Default value
    mapIndex = 0;
    refreshTimeMs = 1000;

    if (arguments.size() <= 1) {
        printUsage();
    }
    if (arguments.size() >= 2) {
        clientPid = arguments[1].toInt(&conversionOk);
        if (conversionOk == true) {
            ok = true;
        } else {
            cout << "Invalid argument for PID: \"" << qPrintable(arguments[1]) << "\"" << endl;
            ok = false;
        }
    }
    if (arguments.size() >= 3) {
        mapIndex = arguments[2].toInt(&conversionOk);
        if (conversionOk == true) {
            ok = true;
        } else {
            cout << "Invalid argument for map index: \"" << qPrintable(arguments[2]) << "\"" << endl;
            ok = false;
        }
    }
    if (arguments.size() >= 4) {
        colorMappingFilePath = arguments[3];
        QFileInfo colorMapFile(colorMappingFilePath);
        if (colorMapFile.exists() == true) {
            ok = true;
        } else {
            cout << "Provided color map file does not exist: \"" << qPrintable(arguments[3]) << "\"" << endl;
            ok = false;
        }
    }
    if (arguments.size() == 5) {
        refreshTimeMs = arguments[4].toInt(&conversionOk);
        if (conversionOk == true) {
            ok = true;
        } else {
            cout << "Invalid argument for refresh interval: \"" << qPrintable(arguments[4]) << "\"" << endl;
            ok = false;
        }
    }

    return ok;
}

static void printUsage()
{
    cout << "Usage: meegoreactionmap-viewer PID [MAP_INDEX] [COLOR_MAPPING_FILEPATH] [REFRESH_TIME]" << endl;
    cout << endl;
    cout << "PID - Process id of the application whose reaction map you want to view." << endl;
    cout << "MAP_INDEX - Index of the reaction map if monitored process uses more than one" << endl;
    cout << "            reaction map. Index defaults to 0." << endl;
    cout << "COLOR_MAPPING_FILEPATH - Path to a text file describing the color mapping to be" << endl;
    cout << "                         used" << endl;
    cout << "REFRESH_TIME - Interval, in milliseconds, between each screen update." << endl;
    cout << endl;
    cout << "Color mapping file format:" << endl;
    cout << "==========================" << endl;
    cout << "Color mapping file has a very strict format. It describes what colors should be" << endl;
    cout << "used to display the different reaction values on the reaction map. Each line of the" << endl;
    cout << "file describes a color to be used for a press and release drawing value pair." << endl;
    cout << "Example:" << endl;
    cout << endl;
    cout << "transparent,transparent=255,255,255" << endl;
    cout << "press,release=0,255,0" << endl;
    cout << "press-loud,=0,0,255" << endl;
    cout << ",=127,127,127" << endl;
    cout << endl;
    cout << "In each line there is a press and release drawing value pair separated by a comma" << endl;
    cout << "followed by RGB color value separated by commas. The first line describes that an" << endl;
    cout << "area filled with transparent drawing value should be painted in white (red=255," << endl;
    cout << "green=255, blue=255). The second line states that an area filled with a value" << endl;
    cout << "that gives \"press\" feedback when pressed and \"release\" feedback when released" << endl;
    cout << "should be painted in green. The third line states that an area filled with a value" << endl;
    cout << "that gives \"press-loud\" feedback when pressed and no feedback when released should" << endl;
    cout << "be painted in blue. The fourth line states that an area filled with non-reactive value" << endl;
    cout << "should be painted in gray. All reaction values that are not defined in this file will" << endl;
    cout << "be drawn in black." << endl;
    cout << endl;
    cout << "Please note that \"transparent\" and \"\" drawing values are reserved special values" << endl;
    cout << "that are not tied to an actual feedback effect file. Use one \"feedbacks=RGB\" mapping" << endl;
    cout << "per line. No blank lines or whitespaces are allowed. If you don't supply a color mapping" << endl;
    cout << "file, the following mapping will be used:" << endl;
    cout << endl;
    cout << "press,release=255,0,0" << endl;
    cout << endl;
    cout << "Special advice:" << endl;
    cout << "===============" << endl;
    cout << "It is useful to forward the output of this program to another X display than the actual" << endl;
    cout << "monitored program is using. Simply set the DISPLAY environment variable to point to the" << endl;
    cout << "other X display. If used in device, X forwarding via SSH works fine." << endl;
}
