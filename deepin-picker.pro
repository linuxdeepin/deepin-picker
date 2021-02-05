######################################################################
# Automatically generated by qmake (3.0) ?? 2? 4 17:49:37 2017
######################################################################

TEMPLATE = app
TARGET = deepin-picker

CONFIG += link_pkgconfig
CONFIG += c++11 
PKGCONFIG += dtkwidget dtkgui
RESOURCES = deepin-picker.qrc
# xcb xcb-util
# Input
HEADERS += src/picker.h \
    src/desktopinfo.h \
           src/colormenu.h \		   		   
           src/clipboard.h \		   		   
           src/animation.h \		   		   
           src/dbusnotify.h \		   		   
           src/settings.h \		   		   
           src/utils.h
SOURCES += src/picker.cpp \
    src/desktopinfo.cpp \
           src/colormenu.cpp \		   
           src/clipboard.cpp \		   
           src/animation.cpp \		   
           src/dbusnotify.cpp \		   
           src/settings.cpp \		   
           src/utils.cpp \		   
		   src/main.cpp		   
		   
QT += core
QT += widgets
QT += gui
QT += dbus

QMAKE_CXXFLAGS += -g

isEmpty(PREFIX){
    PREFIX = /usr
}

isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(ICONDIR):ICONDIR=/usr/share/icons/hicolor/scalable/apps
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-picker
isEmpty(DOCDIR):DOCDIR=/usr/share/dman/deepin-picker

target.path = $$INSTROOT$$BINDIR
icon.path = $$INSTROOT$$ICONDIR
desktop.path = $$INSTROOT$$APPDIR
translations.path = $$INSTROOT$$DSRDIR/translations
manual.path = $$INSTROOT$$DOCDIR

icon.files = image/deepin-picker.svg
desktop.files = deepin-picker.desktop
manual.files = manual/*

dbus_service.files = $$PWD/com.deepin.Picker.service
dbus_service.path = $$PREFIX/share/dbus-1/services

INSTALLS += target icon desktop manual dbus_service

isEmpty(TRANSLATIONS) {
     include(translations.pri)
}

TRANSLATIONS_COMPILED = $$TRANSLATIONS
TRANSLATIONS_COMPILED ~= s/\.ts/.qm/g

translations.files = $$TRANSLATIONS_COMPILED
INSTALLS += translations
CONFIG *= update_translations release_translations

CONFIG(update_translations) {
    isEmpty(lupdate):lupdate=lupdate
    system($$lupdate -no-obsolete -locations none $$_PRO_FILE_)
}
CONFIG(release_translations) {
    isEmpty(lrelease):lrelease=lrelease
    system($$lrelease $$_PRO_FILE_)
}

DSR_LANG_PATH += $$DSRDIR/translations
DEFINES += "DSR_LANG_PATH=\\\"$$DSR_LANG_PATH\\\""
