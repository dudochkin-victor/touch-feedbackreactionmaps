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

#ifndef MREACTIONMAP_H
#define MREACTIONMAP_H

#include <QMap>
#include <QObject>
#include <QTransform>

class QPaintDevice;
class MReactionMapPrivate;
class QPainter;
class QGraphicsItem;
class QGraphicsView;

/*!
 * @class MReactionMap
 * @brief MReactionMap can be used to add low latency haptic feedback to
 * applications
 *
 * @details MReactionMap draws a reactive map to shared memory which is used
 * by meegofeedbackd daemon to provide haptic feedback. The daemon listens to
 * touchscreen events directly from the touchscreen kernel interface and
 * delivers haptic feedback based on the reaction map that has been drawn
 * by MReactionMap. This is done to minimize the latency of the haptic
 * feedback.
 *
 * MReactionMap instance can be constructed after the top level QWidget of the
 * application has been created. Please note that there can be only one
 * MReactionMap instance per top level QWidget.
 *
 * After the MReactionMap instance has been created the MReactionMap API can
 * be used to draw on the created reaction map. Any point on the map defines
 * which feedback will be played when the user's finger enters or leaves the
 * display at that point.
 *
 * @state Stable
 * @copyright Nokia Corporation
 * @license Nokia Open Source
 * @osslibrary Qt 4.6
 * @scope Maemo API
 */
class MReactionMap : public QObject
{
public:

    /*!
     * @brief Class constructor
     *
     * @details Creates the widget's reaction map. There must be only one
     * reaction map instance per top-level widget.
     *
     * @note Reaction map may only be defined for a top level widget.
     *
     * @param topLevelWidget Top level QWidget (top level window) of the
     *                       application.
     * @param appIdentifier Application identifier that is used to locate
     *                      custom feedback effects for the program using
     *                      theming. If @p appIdentifier name is not provided,
     *                      application identifier is determined from the
     *                      name of the executable binary file of the
     *                      application.
     *
     * @state Stable
     */
    explicit MReactionMap(QWidget *topLevelWidget, const QString& appIdentifier = QString(), QObject *parent = 0);

    /*!
     * @brief Class destructor
     *
     * @details Destroys the reaction map.
     *
     * @state Stable
     */
    virtual ~MReactionMap();

    /*!
     * @details Default press feedback. Used when the user presses on an
     *          object.
     */
    static const QString Press;

    /*!
     * @details Default release feedback. Used when finger is released on the
     *          object.
     */
    static const QString Release;

    /*!
     * @details Default cancel feedback.
     *
     * @status Deprecated
     */
    static const QString Cancel;

    /*!
     * @details Transparent feedback. Used when the topmost window has
     * transparent areas. There might be a window beneath the current
     * window that uses reaction maps. Paint all transparent areas with
     * this value.
     *
     * @note This is a special value that cannot be overridden by theming.
     *
     * @sa MReactionMap::Inactive
     */
    static const QString Transparent;

    /*!
     * @details Inactive feedback. Used when no feedback is wanted when
     * finger is pressed or released.
     *
     * @note This is a special value that cannot be overridden by theming.
     *
     * @note The difference between Inactive and Transparent value is that
     * when finger is pressed or released on Transparent value, the used value
     * is looked from the next window behind the current window according to
     * the top-level window stacking order. If the window beneath the current
     * one uses reactionmaps, feedback is played according to what is drawn
     * on that window's reaction map. When finger is pressed or released
     * on a Inactive value, no feedback is given and no other windows are
     * considered.
     */
    static const QString Inactive;

    /*!
     * @brief Returns the reaction map for a top level widget.
     *
     * @details If the supplied widget is not a top level widget, reaction map
     * will be returned for top level widget that is the ancestor of the
     * supplied widget.
     *
     * @param anyWidget Pointer to a top level widget or a descendant of a top
     *                  level widget
     * @return Reaction map for the top level widget or 0 if there is no
     * reaction map specified for the top level widget.
     *
     * @status Stable
     */
    static MReactionMap *instance(QWidget *anyWidget);

    /*!
     * @brief Sets drawing value that gives no reaction when user touches it.
     *
     * @details This is the equivalent of
     * setDrawingValue(MReactionMap::Inactive, MReactionMap::Inactive).
     *
     * @sa MReactionMap::fillRectangle().
     *
     * @status Stable
     */
    void setInactiveDrawingValue();

    /*!
     * @brief Sets drawing value that reacts when user touches or lifts finger
     * from it.
     *
     * @details This is the equivalent of setDrawingValue(MReactionMap::Press,
     * MReactionMap::Release).
     *
     * @sa MReactionMap::fillRectangle().
     *
     * @status Stable
     */
    void setReactiveDrawingValue();

    /*!
     * @brief Sets drawing value that is transparent when user touches or
     * lifts finger from it.
     *
     * @details This is the equivalent of
     * setDrawingValue(MReactionMap::Transparent, MReactionMap::Transparent).
     *
     * @sa MReactionMap::fillRectangle().
     *
     * @status Stable
     */
    void setTransparentDrawingValue();

    /*!
     * @brief Sets drawing value that represents the specified press and
     * release feedbacks.
     *
     * @details Useful when you want to specify different feedbacks to be
     * played when the screen is pressed and released.
     *
     * @sa MReactionMap::fillRectangle().
     *
     * @note The maximum number of different press&release pairs is limited
     * to 255.
     *
     * @param pressFeedback Name of feedback to be played when screen is
     *                      pressed. Empty string means Inactive value.
     * @param releaseFeedback Name of feedback to be played when screen is
     *                        released. Empty string means Inactive value.
     *
     * @status Stable
     */
    void setDrawingValue(const QString &pressFeedback,
                         const QString &releaseFeedback);

    /*!
     * @brief Returns a QTransform transformation matrix that maps screen
     * coordinates to reaction map coordinates.
     *
     * @return QTransform transformation matrix that maps screen coordinates
     * to reaction map coordinates.
     *
     * @note Reaction maps does not necessarily have the same resolutio as the
     * device's display.
     *
     * @note This transformation matrix is used by default when drawing to
     * reaction map.
     *
     * @status Stable
     */
    QTransform transform() const;

    /*!
     * @brief Sets a QTransform transformation matrix which is used when
     * drawing to the reaction map.
     *
     * @details Sets a QTransform transformation matrix that maps given
     * coordinates to reaction map coordinates. This transformation matrix
     * is then used when drawing to the reaction map.
     *
     * @warning The only supported transformations are scaling and translating.
     * Use of rotating and shearing results undefined behavior.
     *
     * @param transform Mapping transformation.
     *
     * @sa MReactionMap::fillRectangle().
     *
     * @status Stable
     */
    void setTransform(QTransform transform);

    /*!
     * @brief Generates a QTransform transformation matrix from QGraphicsItem
     * and QGraphicsView which is used when drawing to the reaction map.
     *
     * @details Sets a QTransform transformation matrix that maps local item
     * coordinates in the given view to reaction map coordinates. This
     * transformation matrix is then used when drawing to the reaction map.
     *
     * @warning The only supported transformations are scaling and translating.
     * Use of rotating and shearing will result in undefined result.
     *
     * @param item Graphics item.
     * @param view Graphics view.
     *
     * @sa MReactionMap::fillRectangle().
     *
     * @status Stable
     */
    void setTransform(QGraphicsItem *item, QGraphicsView *view);

    /*!
     * @brief Draws a filled rectangle on the reaction map using the current
     * transformation matrix and drawing value.
     *
     * @param x X coordinate of the top-left corner of the rectangle.
     * @param y Y coordinate of the top-left corner of the rectangle.
     * @param width Width of the rectangle.
     * @param height Height of the rectangle.
     *
     * @status Stable
     */
    void fillRectangle(int x, int y, int width, int height);

    /*!
     * @brief Draws a filled rectangle on the reaction map using the current
     * transformation matrix and drawing value.
     *
     * @param rectangle The filled rectangle to be drawn.
     *
     * @status Stable
     */
    void fillRectangle(const QRect &rectangle);

    /*!
     * @brief Draws a filled rectangle on the reaction map using the current
     * transformation matrix and drawing value.
     *
     * @param rectangle The filled rectangle to be drawn.
     *
     * @status Stable
     */
    void fillRectangle(const QRectF &rectangle);

    /*!
     * @brief Draws a filled rectangle on the reaction map with the given
     * press and release values. The current transformation matrix is used.
     *
     * @details Even though the specified values override the currently set
     * drawing value, the currently set drawing value itself won't be changed
     * by this method. Therefore subsequent calls to e.g.
     * fillRectangle(QRect &rectangle) will still use the drawing value that
     * was set before this method was called.
     *
     * @param rectangle The filled rectangle to be drawn.
     * @param pressFeedback Name of feedback to be played when screen is
     *                      pressed. Empty string means Inactive value.
     * @param releaseFeedback Name of feedback to be played when screen is
     *                        released. Empty string means Inactive value.
     * @sa setTransform()
     *
     * @status Stable
     */
    void fillRectangle(const QRect &rectangle,
                       const QString &pressFeedback,
                       const QString &releaseFeedback);

    /*!
     * @brief Draws a filled rectangle on the reaction map with the given press
     * and release values. The current transformation matrix is used.
     *
     * @details Even though the specified values override the currently set
     * drawing value, the currently set drawing value itself won't be changed
     * by this method. Therefore subsequent calls to e.g.
     * fillRectangle(QRect &rectangle) will still use the drawing value that
     * was set before this method was called.
     *
     * @param rectangle The filled rectangle to be drawn.
     * @param pressFeedback Name of feedback to be played when screen is
     *                      pressed. Empty string means Inactive value.
     * @param releaseFeedback Name of feedback to be played when screen is
     *                        released. Empty string means Inactive value.
     * @sa setTransform()
     *
     * @status Stable
     */
    void fillRectangle(const QRectF &rectangle,
                       const QString &pressFeedback,
                       const QString &releaseFeedback);

    /*!
     * @brief Draws a filled bound rectangle for a graphics item.
     *
     * @details Draws a filled bound rectangle of a graphics item in the
     * view that is associated with the reaction map instance that this
     * function is called from.
     *
     * @param item A graphics item to be drawn.
     *
     * @status Stable
     */
    void fillItemBoundRect(QGraphicsItem *item);

    /*!
     * @brief Draws a filled bound rectangle for a graphics item.
     *
     * @details Draws a filled bound rectangle of a graphics item in the
     * view that is associated with the reaction map instance that this
     * function is called from. Even though the specified drawing values
     * override the currently set drawing values, the currently set drawing
     * value itself won't be changed by this method.
     *
     * @param item A graphics item to be drawn.
     * @param pressFeedback Name of feedback to be played when screen is pressed.
     *                      Empty string means Inactive value.
     * @param releaseFeedback Name of feedback to be played when screen is released.
     *                        Empty string means Inactive value.
     *
     * @status Stable
     */
    void fillItemBoundRect(QGraphicsItem *item,
                           const QString &pressFeedback,
                           const QString &releaseFeedback);

    /*!
     * @brief Reaction map width.
     *
     * @return width, in pixels.
     *
     * @status Stable
     */
    int width() const;

    /*!
     * @brief Reaction map height.
     *
     * @return height, in pixels.
     *
     * @status Stable
     */
    int height() const;

    /*!
     * @brief Clears the reaction map.
     *
     * @details Clears the reaction map by filling the entire reaction map
     * with Inactive drawing value.
     *
     * @status Stable
     */
    void clear();

protected:

    MReactionMapPrivate *d;
};

#endif
