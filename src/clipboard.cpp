#include "clipboard.h"
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

void Clipboard::copyToClipboard(QColor color, QString colorString)
{
    
    // Popup notify.
    QDBusInterface notification("org.freedesktop.Notifications",
                                "/org/freedesktop/Notifications",
                                "org.freedesktop.Notifications",
                                QDBusConnection::sessionBus());

    QStringList actions;

    QVariantMap hints;

    QList<QVariant> arg;
    arg << (QCoreApplication::applicationName())                 // appname
        << ((unsigned int) 0)                                    // id
        << QString("deepin-picker")                              // icon
        << tr("Copy color")                                      // summary
        << QString("Has copy %1 to clipboard").arg(colorString)  // body
        << actions                                               // actions
        << hints                                                 // hints
        << (int) -1;                                             // timeout
    notification.callWithArgumentList(QDBus::AutoDetect, "Notify", arg);
    
    // Copy to clipbard.
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(colorString);
    
    QApplication::quit();
}
