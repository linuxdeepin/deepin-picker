// SPDX-FileCopyrightText: 2011-2026 UnionTech Software Technology Co., Ltd.
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
#include "ddlog.h"
#include "logger.h"
#include <QPointer>
#include <iostream>
#include "utils.h"
#include "clipboard.h"
#include "cpickermanager.h"

DWIDGET_USE_NAMESPACE


int main(int argc, char *argv[])
{
    // 日志处理必须放在最前（Application 实例化与任何日志打印之前），
    // 否则 QT_LOGGING_RULES 环境变量清理与 DConfig 动态日志级别均不生效
    MLogger logger;

    qCDebug(appLog) << "Starting Deepin Picker application";
    
    if (!QString(qgetenv("XDG_CURRENT_DESKTOP")).toLower().startsWith("deepin")) {
        qCDebug(appLog) << "Setting XDG_CURRENT_DESKTOP to Deepin";
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
    qCDebug(appLog) << "Initialized DApplication with high DPI pixmaps support";

    // 判断窗口特效是否开启
    //    if (!DWindowManagerHelper::instance()->hasComposite()) {
    //        Utils::warnNoComposite();
    //        return 0;
    //    }

    app.loadTranslator();
    qCDebug(appLog) << "Loaded application translations";

    app.setOrganizationName("deepin");
    app.setApplicationName("deepin-picker");
    app.setApplicationVersion("1.2");
    qCDebug(appLog) << "Set application info - name: deepin-picker, version: 1.2";

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
    qCDebug(appLog) << "Application launch mode:" << (isLaunchByDBus ? "DBus" : "Direct");

    // Init modules.
    qCDebug(appLog) << "Initializing application modules";
    Clipboard clipboard;
    QPointer<CPickerManager> picker = new CPickerManager;
    picker->setLanchFlag(isLaunchByDBus ? CPickerManager::ELanchedByOtherApp : CPickerManager::ELanchedBySelf);
    if (!isLaunchByDBus) {
        qCDebug(appLog) << "Starting color picker in direct mode";
        picker->StartPick("");
    }
    QObject::connect(picker.data(), &CPickerManager::copyColor, &clipboard, &Clipboard::copyToClipboard, Qt::QueuedConnection);

    if (isLaunchByDBus) {
        qCDebug(appLog) << "Registering DBus service";
        QDBusConnection dbus = QDBusConnection::sessionBus();
        if (dbus.registerService("com.deepin.Picker")) {
            qCDebug(appLog) << "Successfully registered DBus service: com.deepin.Picker";
            dbus.registerObject("/com/deepin/Picker", picker.data(), QDBusConnection::ExportScriptableSlots | QDBusConnection::ExportScriptableSignals);
        } else {
            qCWarning(appLog) << "Failed to register DBus service: com.deepin.Picker";
        }
    }
    
    qCDebug(appLog) << "Entering application event loop";
    return app.exec();
}
