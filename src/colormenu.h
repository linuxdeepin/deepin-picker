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

#ifndef COLORMENU_H
#define COLORMENU_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QColor>
#include <QPaintEvent>

class ColorMenu : public QWidget
{
    Q_OBJECT

public:
    ColorMenu(int x, int y, int size, QColor color, QWidget *parent = 0);
    ~ColorMenu();

signals:
    void copyColor(QColor color, QString colorType);
    void exit();

public slots:
    void copyFloatRGBAColor();
    void copyFloatRGBColor();
    void copyHexColor();
    void copyCmykColor();
    void copyHsvColor();
    void copyRGBAColor();
    void copyRGBColor();
//    void showMenu();

protected:
    void paintEvent(QPaintEvent *);

private:
    QAction *hexAction;
    QAction *rgbAction;
    QAction *rgbFloatAction;
    QAction *rgbaAction;
    QAction *rgbaFloatAction;
    QAction *cmykAction;
    QAction *hsvAction;
    QColor windowColor;
    QMenu *colorMenu;
    bool clickMenuItem;
    int menuOffsetX;
    int menuOffsetY;
    int shadowBottomMargin;
    int shadowXMargin;
    int windowSize;
    int windowX;
    int windowY;
};

#endif
