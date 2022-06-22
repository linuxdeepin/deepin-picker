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

//#include "picker.h"
//#include "animation.h"
//#include "settings.h"
//#include "utils.h"
//#include "colormenu.h"
//#include <QPainter>
//#include <QBitmap>
//#include <QPixmap>
//#include <QMenu>
//#include <QAction>
//#include <QClipboard>
//#include <QStyleFactory>
//#include <QScreen>
//#include <QApplication>
//#include <QDesktopWidget>
//#include <QDebug>
//#include <QPainterPath>

//#include <dregionmonitor.h>
//#include <QDBusInterface>
//#include <QDBusReply>
//#include <QWindow>

//DesktopInfo Picker::m_info = DesktopInfo();
//CScreenshotWidget::CScreenshotWidget(CPickerManager *parent): QWidget(nullptr),
//    _parentManager(parent)
//{
//    //为顶层窗口
//    this->setWindowFlags(this->windowFlags() | Qt::Window | Qt::FramelessWindowHint
//                         | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint);
//    winId();
////    connect(this->windowHandle(), &QWindow::screenChanged, this, [ = ]() {
////        updatePixmap();
////        this->update();
////    });
//}

//void CScreenshotWidget::paintEvent(QPaintEvent *event)
//{
//    Q_UNUSED(event);
//    QPainter painter(this);
//    painter.drawPixmap(rect(), pixMap(), pixMap().rect());
//}

//void CScreenshotWidget::showEvent(QShowEvent *event)
//{
//    //updatePixmap();
//    //this->update();
//    QWidget::showEvent(event);
//}

//void CScreenshotWidget::keyPressEvent(QKeyEvent *event)
//{
//    if (event->matches(QKeySequence::Cancel))
//        QApplication::quit();
//    QWidget::keyPressEvent(event);
//}

////void CScreenshotWidget::updatePixmap()
////{
////    if (Picker::isSpecialPlatform()) {
////        _scrennshotPixmap = Picker::getWaylandPlatformPixmap();
////    } else {
////        auto screen = this->windowHandle()->screen();
////        _scrennshotPixmap = screen->grabWindow(0);
////    }
////}

////QPixmap CScreenshotWidget::getScreenPixMap()
////{
////    if (_scrennshotPixmap.isNull()) {
////        updatePixmap();
////    }
////    return _scrennshotPixmap;
////}

//void CScreenshotWidget::setPixmap(const QPixmap &pixMap)
//{
//    _scrennshotPixmap = pixMap;
//}

//QPixmap CScreenshotWidget::pixMap()
//{
//    return _scrennshotPixmap;
//}

//CPickerManager::CPickerManager(): QObject(nullptr)
//{
////    int windowHeight = 236/* * t_ratio*/;
////    int windowWidth = 236/* * t_ratio*/;

////    shadowPixmap = QPixmap(Utils::getQrcPath("shadow.png"));
////    if (m_info.waylandDectected()) {
////        scaledPixmap = shadowPixmap.scaled(windowWidth, windowHeight);
////    } else {
////        scaledPixmap = shadowPixmap.scaled(windowWidth / t_ratio, windowHeight / t_ratio);
////    }

//    auto screens = QApplication::screens();
//    for (auto screen : screens) {
//        auto pix = screen->grabWindow(0);
//        //auto geometry = QRect(screen->geometry().topLeft(), screen->geometry().size() * screen->devicePixelRatio());
//        CScreenshotWidget *pWidget = new CScreenshotWidget(this);
//        pWidget->setGeometry(screen->geometry());
//        _widgets.append(pWidget);
//        pWidget->show();
//    }

////    DRegionMonitor *eventMonitor = new DRegionMonitor;
////    //设置捕获区域
////    eventMonitor->setWatchedRegion(QRegion(INT_MIN, INT_MIN, INT_MAX * 2, INT_MAX * 2));

////    QObject::connect(eventMonitor, &DRegionMonitor::keyPress, this, [ = ](const QString & name) {
////        if (name == "Escape")
////            QApplication::quit();
////    });


////    // Binding handler to xrecord signal.
////    QObject::connect(eventMonitor, &DRegionMonitor::cursorMove, this, [ = ](const QPoint & p) {
////        onMouseMove(p);
////    });
////    QObject::connect(eventMonitor, &DRegionMonitor::buttonPress, this, [ = ]() {

////    });

////    eventMonitor->setCoordinateType(DRegionMonitor::Original);
////    eventMonitor->registerRegion();

////    if (!eventMonitor->registered()) {
////        qWarning() << "Failed on register monitor";
////        QApplication::quit();
////    }
//}

//CPickerManager::~CPickerManager()
//{
//}

//QColor CPickerManager::currentColor() const
//{
//    return _curColor;
//}

//void CPickerManager::onMouseMove(const QPoint &pos)
//{
//    ensureDeskTopPixmap();
//    updateCursor(_desktopPixmap, pos);
//}

//void CPickerManager::updateCursor(const QPixmap &pixMap, const QPoint &posInPixmap)
//{
//    qreal scalFactor = 8;

//    const QSize const_focusSize = QSize(200, 200) * qApp->devicePixelRatio() / scalFactor;

//    //必须保证聚焦区域的宽高都是奇数(这样才能均分像素点，保证中心焦点像素和显示像素重合)
//    QSizeF focusSize = QSizeF(const_focusSize.width() % 2 == 0 ? (const_focusSize.width() + 1) : const_focusSize.width(),
//                              const_focusSize.height() % 2 == 0 ? (const_focusSize.height() + 1) : const_focusSize.height());
//    QRectF focusRect = QRectF(posInPixmap, focusSize).translated(-focusSize.width() / 2, -focusSize.height() / 2);

//    QPixmap focusPixmap = pixMap.copy(focusRect.toRect());

//    //获取焦点区域图形
//    focusPixmap = focusPixmap.scaled(focusPixmap.size() * scalFactor);

//    QRect focusPixmapVaildRect = focusPixmap.rect();
//    {
//        //生成不规则的鼠标图(要不焦点区域的放大图再增大20个像素以保证边界阴影能显示出来)
//        QSize cusrorPixSize = QSize(20, 20) * qApp->devicePixelRatio() + focusPixmapVaildRect.size() ;
//        QPixmap cursorPix(cusrorPixSize);
//        cursorPix.fill(Qt::transparent);
//        QPainter painter(&cursorPix);

//        painter.setRenderHint(QPainter::Antialiasing);

//        //0.绘制阴影
//        painter.save();
//        painter.setRenderHint(QPainter::SmoothPixmapTransform);
//        painter.drawPixmap(cursorPix.rect(), shadowPixmap);
//        painter.restore();


//        painter.save();
//        QRectF vaildRect = QRectF(QPointF((cusrorPixSize.width() - focusPixmapVaildRect.width()) / 2.0,
//                                          (cusrorPixSize.height() - focusPixmapVaildRect.height()) / 2.0), focusPixmapVaildRect.size());

//        painter.translate(vaildRect.topLeft());
//        vaildRect.translate(-vaildRect.topLeft());

//        //1.设置有效的裁剪区域(可见区域是一个圆形)
//        QPainterPath clipPath;
//        clipPath.addEllipse(vaildRect);
//        painter.setClipPath(clipPath);
//        painter.drawPixmap(vaildRect.toRect(), focusPixmap, focusPixmapVaildRect);

//        //2.绘制圆形的外边线
//        QPen p(QColor(255, 255, 255, 87));
//        p.setWidth(3 * qApp->devicePixelRatio());
//        painter.setPen(p);
//        painter.drawEllipse(vaildRect);


//        //3.绘制中心像素点;放大了的焦点图像的像素大小对应也放大了scalFactor倍
//        qreal logicPixelWidth = 1  * scalFactor;
//        QPointF innerCenter = vaildRect.center();
//        QRectF innerRect(innerCenter, QSizeF(1, 1));
//        qreal offset = (logicPixelWidth - 1) / 2;
//        innerRect.adjust(-offset, -offset, offset, offset);

//        painter.setPen(QColor(255, 255, 255, 89));
//        painter.drawRect(innerRect);

//        painter.setPen(QColor(0, 0, 0, 89));
//        painter.drawRect(innerRect.adjusted(-1, -1, 1, 1));


//        painter.restore();

//        painter.setPen(QColor(255, 0, 0, 100));


//        _curColor = focusPixmap.toImage().pixelColor(focusPixmap.rect().center());

//        QFont f;
//        painter.setFont(f);
//        QString text = _curColor.name() + QString("w=%1lw=%2").arg(focusPixmap.width()).arg(logicPixelWidth);
//        QSize colorNameSz   = QSize(QFontMetrics(f).width(text), QFontMetrics(f).height()) ;

//        QRectF colorNameRect = QRectF(QPointF((cursorPix.width() - colorNameSz.width()) / 2,
//                                              (cursorPix.height() - colorNameSz.height()) / 2 + colorNameSz.height() + 6), colorNameSz);
//        painter.drawText(colorNameRect, text);


//        if (QApplication::overrideCursor() == nullptr) {
//            QApplication::setOverrideCursor(QCursor(cursorPix));
//        } else {
//            QApplication::changeOverrideCursor(QCursor(cursorPix));
//        }
//    }
//}
//QRect getDeskTopRect()
//{
//    QRect deskTopRect(0, 0, 0, 0);
//    auto screens = QApplication::screens();
//    for (auto screen : screens) {

//        auto geometry = QRect(screen->geometry().topLeft(), screen->geometry().size() *
//                              screen->devicePixelRatio());
//        deskTopRect = deskTopRect.united(geometry);
//    }
//    return deskTopRect;
//}
//QPixmap getDesktopPixmap()
//{
//    QPixmap pix(getDeskTopRect().size());
//    pix.fill(Qt::transparent);
//    QPainter painter(&pix);
//    auto screens = QApplication::screens();
//    for (auto screen : screens) {
//        auto pix = screen->grabWindow(0);
//        auto geometry = QRect(screen->geometry().topLeft(), screen->geometry().size() * screen->devicePixelRatio());
//        painter.drawPixmap(geometry, pix, pix.rect());
//    }
//    painter.end();
//    return pix;
//}


//void CPickerManager::ensureDeskTopPixmap()
//{
//    if (_desktopPixmapDirty) {
//        _desktopPixmap = getDesktopPixmap();
//        _desktopPixmapDirty = false;
//    }
//}

//Picker::Picker(bool launchByDBus)
//{
//    // Init app id.
//    isLaunchByDBus = launchByDBus;

//    // Init window flags.
//    setWindowFlags(windowFlags() | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::Tool);
//    setAttribute(Qt::WA_TranslucentBackground, false);
//    setMouseTracking(true);
//    installEventFilter(this);

//    // Init attributes.
//    qreal t_ratio = this->devicePixelRatioF();
//    blockHeight = 20;
//    blockWidth = 20;
//    displayCursorDot = false;
//    height = 220;
//    screenshotSize = 11;
//    width = 220;
//    windowHeight = 236;
//    windowWidth = 236;

//    shadowPixmap = QPixmap(Utils::getQrcPath("shadow.png"));
//    if (m_info.waylandDectected()) {
//        scaledPixmap = shadowPixmap.scaled(windowWidth, windowHeight);
//    } else {
//        scaledPixmap = shadowPixmap.scaled(windowWidth / t_ratio, windowHeight / t_ratio);
//    }

//    // Init window size and position.
//    if (m_info.waylandDectected()) {
//        screenPixmap = getWaylandPlatformPixmap();
//    } else {
//        screenPixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
//    }
//    this->setPixmap(screenPixmap);
//    resize(screenPixmap.size());
//    move(0, 0);
//}

//Picker::~Picker()
//{
//    animation->deleteLater();
//    menu->deleteLater();
//    updateScreenshotTimer->deleteLater();
//}

//void Picker::paintEvent(QPaintEvent *)
//{
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
//    if (!m_info.waylandDectected())
//        painter.scale(1 / this->devicePixelRatioF(), 1 / this->devicePixelRatioF());
//    painter.drawPixmap(rect(), screenPixmap);
//    painter.end();
//}

//QPixmap Picker::getWaylandPlatformPixmap()
//{
//    QPixmap res;
//    QDBusInterface kwinInterface(QStringLiteral("org.kde.KWin"),
//                                 QStringLiteral("/Screenshot"),
//                                 QStringLiteral("org.kde.kwin.Screenshot"));
//    QDBusReply<QString> reply = kwinInterface.call(QStringLiteral("screenshotFullscreen"));
//    res = QPixmap(reply.value());
//    if (!res.isNull()) {
//        QFile dbusResult(reply.value());
//        dbusResult.remove();
//    }
//    return res;
//}

//void Picker::doMouseMove(const QPoint &pos)
//{
//    _curpos = pos;
//    qreal scalFactor = _scaleFactor;

//    const QSize const_focusSize = QSize(200, 200) * qApp->devicePixelRatio() / scalFactor;

//    //必须保证聚焦区域的宽高都是奇数(这样才能均分像素点，保证中心焦点像素和显示像素重合)
//    QSizeF focusSize = QSizeF(const_focusSize.width() % 2 == 0 ? (const_focusSize.width() + 1) : const_focusSize.width(),
//                              const_focusSize.height() % 2 == 0 ? (const_focusSize.height() + 1) : const_focusSize.height());
//    QRectF focusRect = QRectF(pos, focusSize).translated(-focusSize.width() / 2, -focusSize.height() / 2);

//    QPixmap focusPixmap = screenPixmap.copy(focusRect.toRect());

//    //获取焦点区域图形
//    focusPixmap = focusPixmap.scaled(focusPixmap.size() * scalFactor);

//    //scalFactor = focusPixmap.width() / focusSize.width();


//    QRect focusPixmapVaildRect = focusPixmap.rect();
//    {
//        //生成不规则的鼠标图(要不焦点区域的放大图再增大20个像素以保证边界阴影能显示出来)
//        QSize cusrorPixSize = QSize(20, 20) * qApp->devicePixelRatio() + focusPixmapVaildRect.size() ;
//        QPixmap cursorPix(cusrorPixSize);
//        cursorPix.fill(Qt::transparent);
//        QPainter painter(&cursorPix);

//        painter.setRenderHint(QPainter::Antialiasing);

//        //0.绘制阴影
//        painter.save();
//        painter.setRenderHint(QPainter::SmoothPixmapTransform);
//        painter.drawPixmap(cursorPix.rect(), shadowPixmap);
//        painter.restore();


//        painter.save();
//        QRectF vaildRect = QRectF(QPointF((cusrorPixSize.width() - focusPixmapVaildRect.width()) / 2.0,
//                                          (cusrorPixSize.height() - focusPixmapVaildRect.height()) / 2.0), focusPixmapVaildRect.size());

//        painter.translate(vaildRect.topLeft());
//        vaildRect.translate(-vaildRect.topLeft());

//        //1.设置有效的裁剪区域(可见区域是一个圆形)
//        QPainterPath clipPath;
//        clipPath.addEllipse(vaildRect);
//        painter.setClipPath(clipPath);
//        painter.drawPixmap(vaildRect.toRect(), focusPixmap, focusPixmapVaildRect);

//        //2.绘制圆形的外边线
//        QPen p(QColor(255, 255, 255, 87));
//        p.setWidth(3 * qApp->devicePixelRatio());
//        painter.setPen(p);
//        painter.drawEllipse(vaildRect);


//        //3.绘制中心像素点;放大了的焦点图像的像素大小对应也放大了scalFactor倍
//        qreal logicPixelWidth = 1  * scalFactor;
//        QPointF innerCenter = vaildRect.center();
//        QRectF innerRect(innerCenter, QSizeF(1, 1));
//        qreal offset = (logicPixelWidth - 1) / 2;
//        innerRect.adjust(-offset, -offset, offset, offset);

//        painter.setPen(QColor(255, 255, 255, 89));
//        painter.drawRect(innerRect);

//        painter.setPen(QColor(0, 0, 0, 89));
//        painter.drawRect(innerRect.adjusted(-1, -1, 1, 1));


//        painter.restore();

//        painter.setPen(QColor(255, 0, 0, 100));


//        QColor color = focusPixmap.toImage().pixelColor(focusPixmap.rect().center());

//        QFont f;
//        painter.setFont(f);
//        QString text = color.name() + QString("w=%1lw=%2").arg(focusPixmap.width()).arg(logicPixelWidth);
//        QSize colorNameSz   = QSize(QFontMetrics(f).width(text), QFontMetrics(f).height()) ;

//        QRectF colorNameRect = QRectF(QPointF((cursorPix.width() - colorNameSz.width()) / 2,
//                                              (cursorPix.height() - colorNameSz.height()) / 2 + colorNameSz.height() + 6), colorNameSz);
//        painter.drawText(colorNameRect, text);


//        if (QApplication::overrideCursor() == nullptr) {
//            QApplication::setOverrideCursor(QCursor(cursorPix));
//        } else {
//            QApplication::changeOverrideCursor(QCursor(cursorPix));
//        }
//        // qDebug() << "elsp ======= " << time.elapsed();
//    }
//}

//void Picker::handleLeftButtonPress(const QPoint &pos, int button)
//{
//    if (button != DTK_WIDGET_NAMESPACE::DRegionMonitor::Button_Left)
//        return;

//    if (!displayCursorDot && isVisible()) {
//        // Rest cursor and hide window.
//        // NOTE: Don't call hide() at here, let process die,
//        // Otherwise mouse event will pass to application window under picker.
//        QApplication::setOverrideCursor(Qt::ArrowCursor);

//        // Rest color type to hex if config file not exist.
//        Settings *settings = new Settings();

//        // Emit copyColor signal to copy color to system clipboard.
//        cursorColor = getColorAtCursor(pos.x(), pos.y());
//        copyColor(cursorColor, settings->getOption("color_type", "HEX").toString());

//        // Send colorPicked signal when call by DBus and no empty appid.
//        if (isLaunchByDBus && appid != "") {
//            colorPicked(appid, Utils::colorToHex(cursorColor));
//        }
//    }
//}

//void Picker::handleRightButtonRelease(const QPoint &pos, int button)
//{
//    if (button != DTK_WIDGET_NAMESPACE::DRegionMonitor::Button_Right)
//        return;

//    if (!displayCursorDot && isVisible()) {
//        // Set displayCursorDot flag when click right button.
//        displayCursorDot = true;

//        cursorColor = getColorAtCursor(pos.x(), pos.y());

//        // Popup color menu window.
//        qreal devicePixelRatio = qApp->devicePixelRatio();
//        menu = new ColorMenu(
//            pos.x() / devicePixelRatio - blockWidth / 2,
//            pos.y() / devicePixelRatio - blockHeight / 2,
//            blockWidth,
//            cursorColor);
//        connect(menu, &ColorMenu::copyColor, this, &Picker::copyColor, Qt::QueuedConnection);
//        connect(menu, &ColorMenu::exit, this, &Picker::exit, Qt::QueuedConnection);
//        menu->show();
//        menu->setFocus();       // set focus to monitor 'aboutToHide' signal of color menu

//        // Display animation before poup color menu.
//        animation = new Animation(pos.x() / devicePixelRatio, pos.y() / devicePixelRatio, screenshotPixmap, cursorColor);
//        connect(animation, &Animation::finish, this, &Picker::popupColorMenu, Qt::QueuedConnection);

//        // Rest cursor to default cursor.
//        QApplication::setOverrideCursor(Qt::ArrowCursor);

//        // Show animation after rest cursor to avoid flash screen.
//        animation->show();
//    }
//}

//QColor Picker::getColorAtCursor(int x, int y)
//{
//    return QColor(screenPixmap.copy(x, y, 1, 1).toImage().pixel(0, 0));
//}

//bool Picker::isSpecialPlatform()
//{
//    return m_info.waylandDectected();
//}

//void Picker::popupColorMenu()
//{
//    // Hide picker main window and popup color menu.
//    // NOTE: Don't call hide() at here, let process die,
//    // Otherwise mouse event will pass to application window under picker.
//    menu->showMenu();
//}

//void Picker::StartPick(QString id)
//{
//    // Update app id.
//    appid = id;

//    // Show window.
//    showFullScreen();

//    // Update screenshot when start.
//    updateScreenshot();
//}
