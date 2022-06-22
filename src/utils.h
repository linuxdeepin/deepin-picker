/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2018 Deepin, Inc.
 *               2011 ~ 2018 Wang Yong
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Wang Yong <wangyong@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QObject>
#include <QString>
#include <QPainter>
#include <QColor>

class Utils : public QObject
{
    Q_OBJECT

public:
    static QString getQrcPath(QString imageName);
//    static QString getQssPath(QString qssName);
    static void passInputEvent(int wid);
    static QString colorToHex(QColor color);
    static QString colorToCMYK(QColor color);
    static QString colorToHSV(QColor color);
    static QString colorToRGB(QColor color);
    static QString colorToRGBA(QColor color);
    static QString colorToFloatRGB(QColor color);
    static QString colorToFloatRGBA(QColor color);
    static qreal easeInOut(qreal x);
//    static qreal easeInQuad(qreal x);
//    static qreal easeInQuint(qreal x);
//    static qreal easeOutQuad(qreal x);
//    static qreal easeOutQuint(qreal x);
//    static void warnNoComposite();
};
