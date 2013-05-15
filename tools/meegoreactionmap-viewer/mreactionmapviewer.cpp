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

#include <mfsettings.h>

#include <mfutil.h>

#include <QDebug>
#include <QDir>
#include <QPainter>
#include <QTimer>

#include <sys/shm.h>
#include <fcntl.h>

MReactionMapViewer::MReactionMapViewer()
    : reactionMapView(0), reactionMap(0), feedbackPalette(0), sharedMemoryAddress(0)
{
}

MReactionMapViewer::~MReactionMapViewer()
{
    if (reactionMapView) {
        delete reactionMapView;
        reactionMapView = 0;
    }

    if (reactionMap) {
        delete reactionMap;
        reactionMap = 0;
    }

    if (feedbackPalette) {
        delete feedbackPalette;
        feedbackPalette = 0;
    }

    if (sharedMemoryAddress) {
        if (shmdt(sharedMemoryAddress) == -1) {
            qWarning("Error when detaching shared"
                     " memory segment.");
        }
        sharedMemoryAddress = 0;
    }

}

bool MReactionMapViewer::init(int pid, int mapIndex, int refreshInterval, const QString &colorMappingFilePath)
{
    QString tempFilePath;
    QString applicationName;

    tempFilePath = findTempFilePath(pid, mapIndex);
    if (tempFilePath.size() == 0) {
        return false;
    }

    applicationName = fetchApplicationName(tempFilePath);
    feedbackPalette = new MFeedbackPalette(applicationName);

    if (!loadReactionMap(tempFilePath)) {
        return false;
    }

    reactionMapView = new QImage(MfSettings::reactionMapWidth(),
                                 MfSettings::reactionMapHeight(),
                                 QImage::Format_RGB32);

    colorMapping = ColorMap::loadFromFile(colorMappingFilePath);

    if (colorMapping.size() == 0) {
        colorMapping.append(ColorMap("press", "release", qRgb(255, 0, 0)));
    }


    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateReactionMapView()));
    timer->start(refreshInterval);

    qDebug() << "Viewing reaction map of application window:" << applicationName << "( PID:" << pid << "map index:" << mapIndex << ")";
    return true;
}

void MReactionMapViewer::showReaction(const QString &pressFeedback,
                                     const QString &releaseFeedback,
                                     QRgb color)
{
    uchar reactionValue;
    reactionValue = feedbackPalette->findDrawingValue(pressFeedback, releaseFeedback);
    // 0 is reserved for Inactive and 1 is reserved for Transparent. If 0
    // returned for non Inactive value, that means that value was not found
    // from palette and it should be ignored.
    if (reactionValue != 0  || (pressFeedback == "" && releaseFeedback == "")) {
        reactionToViewHash[reactionValue] = color;
    }
}

void MReactionMapViewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.scale(width() / reactionMapView->width(),
                  height() / reactionMapView->height());
    painter.drawImage(QPoint(0, 0), *reactionMapView);
}

void MReactionMapViewer::updateReactionMapView()
{
    QRgb color;
    uchar reaction;
    ColorMap colorMap;

    // Update palette
    reactionToViewHash.clear();
    for (int i = 0; i < colorMapping.size(); ++i) {
        colorMap = colorMapping[i];
        showReaction(colorMap.pressFeedbackName(),
                     colorMap.releaseFeedbackName(),
                     colorMap.color());
    }

    // Paint image
    for (int y = 0; y < MfSettings::reactionMapHeight(); y++) {
        for (int x = 0; x < MfSettings::reactionMapWidth(); x++) {
            reaction = reactionMap[y*MfSettings::reactionMapWidth()+x];

            if (reactionToViewHash.contains(reaction)) {
                color = reactionToViewHash[reaction];
            } else {
                color = qRgb(0, 0, 0); // paint it black
            }
            reactionMapView->setPixel(x, y, color);
        }
    }

    // Repaint widget to show new state of reaction map
    update();
}

QString MReactionMapViewer::findTempFilePath(int pid, int mapIndex)
{
    QString filePath;
    QDir tempDir(mfTempDir());
    QStringList nameFilters;
    QString fileNameFilter;
    QStringList fileNamesList;

    if (!tempDir.exists()) {
        qCritical() << "Temporary directory does not exist:" << mfTempDir();
        return QString();
    }

    // We know the pid but not the window id, which is also part of the file name.
    fileNameFilter.append("rm*_");
    fileNameFilter.append(QString::number(pid));

    nameFilters << fileNameFilter;
    fileNamesList = tempDir.entryList(nameFilters,
                                      QDir::Files | QDir::NoDotAndDotDot);

    if (fileNamesList.size() == 0) {
        qCritical() << "Couldn't find reaction map for pid" << pid;
        return QString();
    } else if (mapIndex > 0 && mapIndex >= fileNamesList.size()) {
        qCritical() << "Couldn't find reaction map for index" << mapIndex;
        return QString();
    }

    // Make sure the reaction maps are sorted so that the map that was created
    // first is on top.
    fileNamesList.sort();

    // We got it!
    filePath = mfTempDir();
    filePath.append("/");
    filePath.append(fileNamesList[mapIndex]);
    return filePath;
}

bool MReactionMapViewer::loadReactionMap(const QString &tempFilePath)
{
    int id;
    key_t key;
    char proc_id = 'C';
    MfPaletteEntry *palette = 0;

    key = ftok(tempFilePath.toAscii().constData(), proc_id);
    id = shmget(key, 0, S_IROTH);

    if (id == -1) {
        qCritical() << "Unable to get shared memory for reaction map"
                    << tempFilePath;
        return false;
    }

    sharedMemoryAddress = shmat(id, 0, 0);

    if ((qptrdiff)sharedMemoryAddress == -1) {
        qCritical() << "Unable to get shared memory address for"
                    << tempFilePath;
        sharedMemoryAddress = 0;
        return false;
    }

    reactionMap = (uchar*)sharedMemoryAddress;
    palette = (MfPaletteEntry*)&reactionMap[MfSettings::reactionMapWidth()*MfSettings::reactionMapHeight()];
    feedbackPalette->setPaletteMemory(palette);

    return true;
}

QString MReactionMapViewer::fetchApplicationName(const QString &tempFilePath)
{
    QFile tempFile(tempFilePath);
    QString applicationName;

    if (tempFile.open(QIODevice::ReadOnly)) {
        applicationName = tempFile.readAll();
        tempFile.close();
    } else {
        qCritical() << "Failed to open" << tempFilePath << "for reading.";
    }

    return applicationName;
}
