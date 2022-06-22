///* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
// * -*- coding: utf-8 -*-
// *
// * Copyright (C) 2011 ~ 2018 Deepin, Inc.
// *               2011 ~ 2018 Wang Yong
// *
// * Author:     Wang Yong <wangyong@deepin.com>
// * Maintainer: Wang Yong <wangyong@deepin.com>
// *
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * any later version.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with this program.  If not, see <http://www.gnu.org/licenses/>.
// */

//#ifndef PICKER_H
//#define PICKER_H

//#include <QWidget>
//#include <QMenu>
//#include "colormenu.h"
//#include "animation.h"
//#include <QAction>
//#include <QTimer>
//#include <QLabel>

//#include "desktopinfo.h"

//class CPickerManager;
//class CScreenshotWidget: public QWidget
//{
//    Q_OBJECT
//public:
//    CScreenshotWidget(CPickerManager *parent);

//    //QPixmap getScreenPixMap();
//    void    setPixmap(const QPixmap &pixMap);
//    QPixmap pixMap();

//protected:
//    void paintEvent(QPaintEvent *event) override;
//    void showEvent(QShowEvent *event) override;
//    void keyPressEvent(QKeyEvent *event) override;
//    //void updatePixmap();

//private:
//    QPixmap         _scrennshotPixmap;
//    CPickerManager *_parentManager = nullptr;
//};


//class CPickerManager: QObject
//{
//    Q_OBJECT

//    Q_CLASSINFO("D-Bus Interface", "com.deepin.Picker")
//public:
//    CPickerManager();
//    ~CPickerManager();

//    QColor currentColor() const;

//public slots:
//    void onMouseMove(const QPoint &pos);

//private:
//    void updateCursor(const QPixmap &pixMap, const QPoint &posInPixmap);
//    void ensureDeskTopPixmap();

//private:
//    qreal _scaleFactor = 12;
//    QList<CScreenshotWidget *> _widgets;
//    QPixmap _shadowPixmap;
//    QColor  _curColor;
//    bool    _desktopPixmapDirty = true;
//    QPixmap _desktopPixmap;
//    friend class CScreenshotWidget;
//};

//class Picker : public QLabel
//{
//    Q_OBJECT

//    Q_CLASSINFO("D-Bus Interface", "com.deepin.Picker")

//public:
//    Picker(bool launchByDBus);
//    ~Picker();

//    QColor getColorAtCursor(int x, int y);


//    static bool isSpecialPlatform();

//protected:
//    void paintEvent(QPaintEvent *);

//signals:
//    void copyColor(QColor color, QString colorType);
//    void exit();

//    Q_SCRIPTABLE void colorPicked(QString appid, QString color);

//public slots:
//    void handleLeftButtonPress(const QPoint &pos, int button);
//    void handleRightButtonRelease(const QPoint &pos, int button);
//    void popupColorMenu();
//    void updateScreenshot();

//    void doMouseMove(const QPoint &p);

//    Q_SCRIPTABLE void StartPick(QString appid);

//private:
//    qreal _scaleFactor = 12;
//    QPoint _curpos;
//    Animation *animation;
//    ColorMenu *menu;
//    QPixmap screenPixmap;
//    QPixmap screenshotPixmap;
//    QTimer *updateScreenshotTimer;
//    QColor cursorColor;
//    QPixmap shadowPixmap;
//    QPixmap scaledPixmap;
//    bool displayCursorDot;
//    int blockHeight;
//    int blockWidth;
//    int cursorX;
//    int cursorY;
//    int height;
//    int screenshotSize;
//    int width;
//    int windowHeight;
//    int windowWidth;
//    bool isLaunchByDBus;
//    QString appid;
//    static DesktopInfo m_info;

//public:
//    static QPixmap getWaylandPlatformPixmap();
//};

//#endif
