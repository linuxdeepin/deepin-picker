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

#include <DApplication>
#include <DMainWindow>
#include <dregionmonitor.h>

#include <QDBusConnection>
#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QCommandLineParser>
#include <DWidgetUtil>
#include <QProcess>
#include <DWindowManagerHelper>
#include <QDebug>
#include <QPointer>
#include <iostream>
#include "utils.h"
#include "clipboard.h"
#include "picker.h"

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    // Load DTK xcb plugin.
    DApplication::loadDXcbPlugin();


    // Init attributes.
    const char *descriptionText = QT_TRANSLATE_NOOP(
                                      "MainWindow",
                                      "Deepin Picker is a quick and easy screen color picking tool. RGB and HEX codes "
                                      "are obtained on click and auto saved to the clipboard.");

    const QString acknowledgementLink = "https://www.deepin.org/acknowledgments/deepin-picker";

    // Init dtk application's attrubites.
    DApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    if (!DWindowManagerHelper::instance()->hasComposite()) {
        Utils::warnNoComposite();
        return 0;
    }

    app.loadTranslator();

    app.setOrganizationName("deepin");
    app.setApplicationName("deepin-picker");
    app.setApplicationVersion("1.2");

    app.setProductIcon(QIcon(Utils::getQrcPath("logo_96.svg")));
    app.setProductName(DApplication::translate("MainWindow", "Deepin Picker"));
    app.setApplicationDescription(DApplication::translate("MainWindow", descriptionText) + "\n");
    app.setApplicationAcknowledgementPage(acknowledgementLink);

    app.setWindowIcon(QIcon(Utils::getQrcPath("logo_48.png")));

    QCommandLineParser parser;

    const QCommandLineOption appidOption("i", "Id for app caller, suggestion add current time in app id");
    parser.addOption(appidOption);

    parser.process(app);

    bool isLaunchByDBus = parser.isSet(appidOption);

    // Init modules.
    Clipboard clipboard;
    QPointer<Picker> picker = new Picker(isLaunchByDBus);
    if (!isLaunchByDBus) {
        picker->StartPick("");
    }

    DRegionMonitor eventMonitor;

    // Exit application when user press esc to cancel pick.
    QObject::connect(&eventMonitor, &DRegionMonitor::keyPress, &clipboard, [&] (const QString & name) {
        if (name == "Escape")
            QApplication::quit();
    });

    // Trigger copyToClipboard slot when got copyColor signal.
    QObject::connect(picker.data(), &Picker::copyColor, &clipboard, &Clipboard::copyToClipboard, Qt::QueuedConnection);

    // Binding handler to xrecord signal.
    QObject::connect(&eventMonitor, &DRegionMonitor::cursorMove, picker.data(), &Picker::handleMouseMove, Qt::QueuedConnection);
    QObject::connect(&eventMonitor, &DRegionMonitor::buttonPress, picker.data(), &Picker::handleLeftButtonPress, Qt::QueuedConnection);
//    QObject::connect(&eventMonitor, &DRegionMonitor::buttonRelease, picker.data(), &Picker::handleRightButtonRelease, Qt::QueuedConnection);

    // Start event monitor thread.
    eventMonitor.setCoordinateType(DRegionMonitor::Original);
    eventMonitor.registerRegion();

    if (!eventMonitor.registered()) {
        qWarning() << "Failed on register monitor";
        return -1;
    }

    if (isLaunchByDBus) {
        QDBusConnection dbus = QDBusConnection::sessionBus();
        if (dbus.registerService("com.deepin.Picker")) {
            dbus.registerObject("/com/deepin/Picker", picker.data(), QDBusConnection::ExportScriptableSlots | QDBusConnection::ExportScriptableSignals);
        }
    }

    return app.exec();
}
