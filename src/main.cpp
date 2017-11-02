/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2017 Deepin, Inc.
 *               2011 ~ 2017 Wang Yong
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

#include <DApplication>
#include <DMainWindow>
#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <DWidgetUtil>
#include <QProcess>
#include <DWindowManagerHelper>
#include <QDebug>
#include <iostream>
#include "utils.h"
#include "clipboard.h"
#include "picker.h"
#include "eventmonitor.h"

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[]) 
{
    // Load DTK xcb plugin.
    DApplication::loadDXcbPlugin();
    
    // Init attributes.
    const char *descriptionText = QT_TRANSLATE_NOOP("MainWindow", 
                                                    "Deepin Picker is a fast screen color picking tool. RGB and HEX code can be obtained according color picked and auto saved to clipboard. The color picking area is where mouse clicked."
        );

    const QString acknowledgementLink = "https://www.deepin.org/acknowledgments/deepin-picker";

    // Init dtk application's attrubites.
    DApplication app(argc, argv);
    
    if (!DWindowManagerHelper::instance()->hasComposite()) {
        QProcess p;
        p.startDetached("/usr/lib/deepin-daemon/dde-warning-dialog");
        return 0;
    }

    app.loadTranslator();
        
    app.setOrganizationName("deepin");
    app.setApplicationName("deepin-picker");
    app.setApplicationVersion("1.0");
        
    app.setProductIcon(QPixmap::fromImage(QImage(Utils::getQrcPath("logo_96.svg"))));
    app.setProductName(DApplication::translate("MainWindow", "Deepin Picker"));
    app.setApplicationDescription(DApplication::translate("MainWindow", descriptionText) + "\n");
    app.setApplicationAcknowledgementPage(acknowledgementLink);
        
    app.setWindowIcon(QIcon(Utils::getQrcPath("logo_48.png")));
        
    // Init modules.
    Clipboard *clipboard = new Clipboard();
    Picker *picker = new Picker();
    EventMonitor eventMonitor;
    
    // Exit xrecord thread when copy color to system clipboard.
    QObject::connect(picker, &Picker::copyColor, clipboard, [&] () {
            eventMonitor.terminate();
        });
    
    // Exit application when module trigger exit signal.
    QObject::connect(picker, &Picker::exit, clipboard, [&] () {
            eventMonitor.terminate();
            QApplication::quit();
        });
    // Exit application when user press esc to cancel pick.
    QObject::connect(&eventMonitor, &EventMonitor::pressEsc, clipboard, [&] () {
            eventMonitor.terminate();
            QApplication::quit();
        });
    
    // Trigger copyToClipboard slot when got copyColor signal.
    QObject::connect(picker, &Picker::copyColor, clipboard, &Clipboard::copyToClipboard, Qt::QueuedConnection);
    
    // Binding handler to xrecord signal.
    QObject::connect(&eventMonitor, &EventMonitor::mouseMove, picker, &Picker::handleMouseMove, Qt::QueuedConnection);
    QObject::connect(&eventMonitor, &EventMonitor::leftButtonPress, picker, &Picker::handleLeftButtonPress, Qt::QueuedConnection);
    QObject::connect(&eventMonitor, &EventMonitor::rightButtonRelease, picker, &Picker::handleRightButtonRelease, Qt::QueuedConnection);

    // Start event monitor thread.
    eventMonitor.start();    
    
    return app.exec();

    return 0;
}
