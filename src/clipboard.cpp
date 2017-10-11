#include "clipboard.h"
#include "settings.h"
#include "utils.h"
#include <QApplication>
#include <QClipboard>
#include <QtDBus>
#include <QDebug>

Clipboard::Clipboard(QObject *parent) : QObject(parent)
{

}

Clipboard::~Clipboard()
{
}

void Clipboard::copyToClipboard(QColor color, QString colorType)
{
    // Save color type to config file.
    QString colorString;
    if (colorType == "HEX") {
        colorString = Utils::colorToHex(color);
    } else if (colorType == "RGB") {
        colorString = Utils::colorToRGB(color);
    } else if (colorType == "RGBA") {
        colorString = Utils::colorToRGBA(color);
    } else if (colorType == "Float_RGB") {
        colorString = Utils::colorToFloatRGB(color);
    } else if (colorType == "Float_RGBA") {
        colorString = Utils::colorToFloatRGBA(color);
    }
    
    Settings *settings = new Settings();
    settings->setOption("color_type", colorType);
    
    // Popup notify.
    QDBusInterface notification("org.freedesktop.Notifications",
                                "/org/freedesktop/Notifications",
                                "org.freedesktop.Notifications",
                                QDBusConnection::sessionBus());

    QStringList actions;
    QVariantMap hints;
    
    QList<QVariant> arg;
    arg << (QCoreApplication::applicationName())                             // appname
        << ((unsigned int) 0)                                                // id
        << QString("deepin-picker")                                          // icon
        << tr("Deepin Picker")                                               // summary
        << QString(tr("Copy color %1 to system clipboard")).arg(colorString) // body
        << actions                                                           // actions
        << hints                                                             // hints
        << (int) -1;                                                         // timeout
    notification.callWithArgumentList(QDBus::AutoDetect, "Notify", arg);
    
    // Copy to clipbard.
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(colorString);
    
    // Quit application.
    QApplication::quit();
}
