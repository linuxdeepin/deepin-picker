// SPDX-FileCopyrightText: 2011-2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include <DApplication>
#include <DMainWindow>
#include <DRegionMonitor>

#include <QDBusConnection>
#include <QApplication>
#include <QScreen>
#include <QCommandLineParser>
#include <DWidgetUtil>
#include <QProcess>
#include <DWindowManagerHelper>
#include <QDebug>
#include <QPointer>
#include <iostream>
#include "utils.h"
#include "clipboard.h"
#include "cpickermanager.h"

DWIDGET_USE_NAMESPACE


int main(int argc, char *argv[])
{
    qDebug() << "Starting Deepin Picker application";
    
    if (!QString(qgetenv("XDG_CURRENT_DESKTOP")).toLower().startsWith("deepin")) {
        qDebug() << "Setting XDG_CURRENT_DESKTOP to Deepin";
        setenv("XDG_CURRENT_DESKTOP", "Deepin", 1);
    }

    // Load DTK xcb plugin.
    auto e = QProcessEnvironment::systemEnvironment();

    // Init attributes.
    const char *descriptionText = QT_TRANSLATE_NOOP(
            "MainWindow",
            "Deepin Picker is a quick and easy screen color picking tool. RGB and HEX codes "
            "are obtained on click and auto saved to the clipboard.");

    const QString acknowledgementLink = "https://www.deepin.org/acknowledgments/deepin-picker";

    // Init dtk application's attrubites.
    DApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    qDebug() << "Initialized DApplication with high DPI pixmaps support";

    // 判断窗口特效是否开启
    //    if (!DWindowManagerHelper::instance()->hasComposite()) {
    //        Utils::warnNoComposite();
    //        return 0;
    //    }

    app.loadTranslator();
    qDebug() << "Loaded application translations";

    app.setOrganizationName("deepin");
    app.setApplicationName("deepin-picker");
    app.setApplicationVersion("1.2");
    qDebug() << "Set application info - name: deepin-picker, version: 1.2";

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
    qDebug() << "Application launch mode:" << (isLaunchByDBus ? "DBus" : "Direct");

    // Init modules.
    qDebug() << "Initializing application modules";
    Clipboard clipboard;
    QPointer<CPickerManager> picker = new CPickerManager;
    picker->setLanchFlag(isLaunchByDBus ? CPickerManager::ELanchedByOtherApp : CPickerManager::ELanchedBySelf);
    if (!isLaunchByDBus) {
        qDebug() << "Starting color picker in direct mode";
        picker->StartPick("");
    }
    QObject::connect(picker.data(), &CPickerManager::copyColor, &clipboard, &Clipboard::copyToClipboard, Qt::QueuedConnection);

    if (isLaunchByDBus) {
        qDebug() << "Registering DBus service";
        QDBusConnection dbus = QDBusConnection::sessionBus();
        if (dbus.registerService("com.deepin.Picker")) {
            qDebug() << "Successfully registered DBus service: com.deepin.Picker";
            dbus.registerObject("/com/deepin/Picker", picker.data(), QDBusConnection::ExportScriptableSlots | QDBusConnection::ExportScriptableSignals);
        } else {
            qWarning() << "Failed to register DBus service: com.deepin.Picker";
        }
    }
    
    qDebug() << "Entering application event loop";
    return app.exec();
}
