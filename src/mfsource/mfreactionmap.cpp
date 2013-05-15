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

#include "mfreactionmap.h"
#include "mfsettings.h"
#include <mfutil.h>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QStringList>

#include <mffeedback.h>
#include <mfsession.h>

#include <sys/shm.h>
#include <sys/time.h>
#include <fcntl.h>

class MfReactionMapPrivate
{
public:
    virtual ~MfReactionMapPrivate();

    bool initMap(const QString& tempFileName);
    QString generateTempFileName();

    void buildFeedbackList(QHash<QString, MfFeedback*> originalFeedbackHash);

    bool initialized;
    void *sharedMemoryAddress;
    int sharedMemoryId;
    uchar *image;
    MfPaletteEntry *palette;

    int tempFileFd;
    QString tempFilePath;

    MfSession *session;
    quint32 windowId;
    qint64 pid;

    // Index 0 and 1 are always NULL.
    // feedbacks start from index 2 onwards.
    // This enables a direct translation from
    // a feedback id in the reaction map
    // to the corresponding index in this list.
    //
    // Feedback id 0 in the reaction map means
    // that no feedback is given into that position.
    // Feedback id 1 in the reaction map means
    // that feedback in that position is transparent.
    QList<MfFeedback*> feedbackList;

    static unsigned int mapCount;
};

unsigned int MfReactionMapPrivate::mapCount = 0;

void MfReactionMapPrivate::buildFeedbackList(QHash<QString, MfFeedback*> originalFeedbackHash)
{
    QStringList feedbackNamesList;
    MfFeedback* feedback;

    feedbackNamesList = originalFeedbackHash.keys();
    qSort(feedbackNamesList);

    for (int i = 0; i < feedbackNamesList.size(); ++i) {
        feedback = originalFeedbackHash[feedbackNamesList[i]];
        feedbackList.append(feedback);
    }

    // Index 0 is always NULL, meaning "inactive".
    feedbackList.prepend(NULL);

    // Index 1 is always NULL, meaning "transparent".
    feedbackList.prepend(NULL);
}

MfReactionMap::MfReactionMap(MfSession *session, quint32 windowId, qint64 pid,
                             QObject *parent)
    : QObject(parent), d(new MfReactionMapPrivate)
{
    d->initialized = false;
    d->sharedMemoryAddress = 0;
    d->sharedMemoryId = -1;
    d->image = 0;
    d->tempFileFd = -1;
    d->session = session;
    d->windowId = windowId;
    d->pid = pid;

    if (session) {
        session->setParent(this);
    } else {
        qDebug("MfReactionMap: No session provided.");
    }
}

MfReactionMap::~MfReactionMap()
{
    if (d) {
        delete d;
        d = 0;
    }
}

MfReactionMapPrivate::~MfReactionMapPrivate()
{
    int result;

    if (tempFileFd != -1) {
        close(tempFileFd);
        tempFileFd = -1;
    }

    if (image != 0) {
#ifdef FR_LOCAL_REACTIONMAP
        free(image);
#endif
        image = 0;
    }

    if (palette != 0) {
#ifdef FR_LOCAL_REACTIONMAP
        free(palette);
#endif
        palette = 0;
    }

    if (sharedMemoryAddress) {
        result = shmdt(sharedMemoryAddress);
        if (result == -1) {
            qWarning("MfReactionMap: Failed to detach shared memory segment.");
        }
    }

    if (sharedMemoryId != -1) {
        shmid_ds buf;

        result = shmctl(sharedMemoryId, IPC_STAT, &buf);
        if (result == -1) {
            qWarning("MfReactionMap: Failed to get shm info.");
        } else if (buf.shm_nattch > 0) {
            qWarning() << "MfReactionMap:" << buf.shm_nattch
                       << "processes still attached to shm.";
        }

        result = shmctl(sharedMemoryId, IPC_RMID, NULL);
        if (result == -1) {
            qCritical("MfReactionMap: Failed to remove shared memory segment.");
        } else {
            // We're only ok to remove the temporary file if we successfully
            // destroy its corresponding shared memory. Otherwise it should
            // stay there as a reminder that the shared memory is still hanging around.
            if (!QFile::remove(tempFilePath)) {
                qWarning() << "MfReactionMap: failed to remove" << tempFilePath;
            }
        }

        sharedMemoryId = -1;
    }

    tempFilePath.clear();
}

bool MfReactionMap::init()
{
    return d->initMap(d->generateTempFileName());
}

MfFeedback *MfReactionMap::pressFeedback(const QPoint &pixelCoords, bool &isTransparent)
{
    int index = d->palette[d->image[pixelCoords.y()*MfSettings::reactionMapWidth()+pixelCoords.x()]].pressFeedbackId;
    if (index == 1) {
        isTransparent = true;
    } else {
        isTransparent = false;
    }

    if (index >= 0 && index < d->feedbackList.size()) {
        return d->feedbackList[index];
    } else {
        return NULL;
    }
}

MfFeedback *MfReactionMap::releaseFeedback(const QPoint &pixelCoords, bool &isTransparent)
{
    int index = d->palette[d->image[pixelCoords.y()*MfSettings::reactionMapWidth()+pixelCoords.x()]].releaseFeedbackId;
    if (index == 1) {
        isTransparent = true;
    } else {
        isTransparent = false;
    }

    if (index >= 0 && index < d->feedbackList.size()) {
        return d->feedbackList[index];
    } else {
        return NULL;
    }
}

bool MfReactionMapPrivate::initMap(const QString& tempFileName)
{
    if (initialized) {
        return false;
    }

    int width = MfSettings::reactionMapWidth();
    int height = MfSettings::reactionMapHeight();

    if (session) {
        buildFeedbackList(session->feedbackHash);
    }

#ifdef FR_LOCAL_REACTIONMAP
    // Allocate memory for image. No shared memory involved.
    // This is useful when performing tests.
    image = static_cast<uchar*>(malloc(width*height));
    memset(image, 0, width*height);
    palette = static_cast<MfPaletteEntry*>(malloc(256*sizeof(MfPaletteEntry)));
    memset(palette, 0, 256*sizeof(MfPaletteEntry));
    Q_UNUSED(tempFileName);
#else
    QByteArray applicationName = session->applicationName().toAscii();
    int result;
    key_t key;
    char proc_id = 'C';
    int size = width*height+256*sizeof(MfPaletteEntry); // Each pixel takes 8 bits+reaction palette

    int flags = S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | IPC_CREAT;

    if (mfTempDir() == 0) {
        return false;
    }
    tempFilePath = mfTempDir();
    tempFilePath.append("/");
    tempFilePath.append(tempFileName);

    tempFileFd = creat(tempFilePath.toAscii().constData(),
               S_IRUSR | S_IWUSR | S_IROTH);
    if (tempFileFd == -1) {
        qCritical() << "Unable to create file for reaction map"
                    << tempFileName;
        return false;
    }

    // Write application name into the file.
    result = write(tempFileFd, applicationName.constData(), applicationName.size());
    if (result != applicationName.size()) {
        qCritical() << "Error writing into" << tempFileName;
        return false;
    }

    key = ftok(tempFilePath.toAscii().constData(), proc_id);
    sharedMemoryId = shmget(key, size, flags);

    if (sharedMemoryId == -1) {
        qCritical() << "Unable to create shared memory for reaction map"
                    << tempFileName;
        return false;
    }

    sharedMemoryAddress = shmat(sharedMemoryId, 0, 0);

    if ((qptrdiff)sharedMemoryAddress == -1) {
        qCritical() << "Unable to get shared memory address for"
                    << tempFileName;
        return false;
    }

    // Make our image for that shared memory
    image = static_cast<uchar*>(sharedMemoryAddress);
    memset(image, 0, width*height);
    // An index with feedback codes
    palette = reinterpret_cast<MfPaletteEntry*>(&image[width*height]);
    memset(palette, 0, 256*sizeof(MfPaletteEntry));

#endif
    initialized = true;

    return initialized;
}

int MfReactionMap::width() const
{
    return MfSettings::reactionMapWidth();
}

int MfReactionMap::height() const
{
    return MfSettings::reactionMapHeight();
}

QString MfReactionMap::tempFilePath() const
{
    return d->tempFilePath;
}

void MfReactionMap::setWindowId(quint32 windowId)
{
    d->windowId = windowId;
}

quint32 MfReactionMap::windowId() const
{
    return d->windowId;
}

void MfReactionMap::setPixel(const QPoint &position,
                             const QString &pressFeedback,
                             const QString &releaseFeedback)
{
    int pressFeedbackId = 0;
    int releaseFeedbackId = 0;
    int i;

    Q_ASSERT(d->feedbackList.size() > 0);

    // Find the feedback ID's for the feedback names. If feedback
    // is not found, use 0.
    for (i = 2; i < d->feedbackList.size(); ++i) {
        if (d->feedbackList[i]->name() == pressFeedback) {
            pressFeedbackId = i;
            break;
        }
    }
    if (i >= d->feedbackList.size() && pressFeedback == "transparent") {
        pressFeedbackId = 1;
    }

    for (i = 2; i < d->feedbackList.size(); ++i) {
        if (d->feedbackList[i]->name() == releaseFeedback) {
            releaseFeedbackId = i;
            break;
        }
    }
    if (i >= d->feedbackList.size() && releaseFeedback == "transparent") {
        releaseFeedbackId = 1;
    }

    for (i = 2; i < 256; ++i) {
        if (d->palette[i].pressFeedbackId == 0 &&
            d->palette[i].releaseFeedbackId == 0) {
            d->palette[i].pressFeedbackId = static_cast<quint8>(pressFeedbackId);
            d->palette[i].releaseFeedbackId = static_cast<quint8>(releaseFeedbackId);
            break;
        }
    }

    if (i < 256) {
        unsigned int imageSize, imageIndex;
        imageSize = MfSettings::reactionMapWidth()*MfSettings::reactionMapHeight();
        imageIndex = position.y()*MfSettings::reactionMapWidth() + position.x();
        if (imageIndex >= imageSize) {
            qCritical() << "Reaction map coordinate out of bounds!";
        } else {
            d->image[imageIndex] = static_cast<uchar>(i);
        }
    }
}

QString MfReactionMapPrivate::generateTempFileName()
{
    QString tempFileName = "rm";

    tempFileName.append(QString::number(mapCount++));
    tempFileName.append("_");
    tempFileName.append(QString::number(pid));

    return tempFileName;
}

QString MfReactionMap::applicationName() const
{
    if (d->session) {
        return d->session->applicationName();
    } else {
        return QString();
    }
}
