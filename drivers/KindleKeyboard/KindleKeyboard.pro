# -------------------------------------------------
# Project created by QtCreator 2009-12-17T16:35:12
# -------------------------------------------------
QT += core \
    gui
TARGET = KindleKeyboard
TEMPLATE = lib
CONFIG += plugin
DESTDIR = $$(QTDIR)/plugins/kbddrivers
SOURCES += keyboardplugin.cpp \
    kindlekeyboard.cpp \
    keymap/keymap.c \
    screenshot/screenshot.c

HEADERS += keyboardplugin.h \
    kindlekeyboard.h \
    keycodes.h \
    keymap/keymap.h \
    screenshot/screenshot.h

INCLUDEPATH += ./keymap
INCLUDEPATH += $$(QTDIR)/mkspecs/qws/linux-arm-g++
INCLUDEPATH += $$(QTDIR)/include/Qt
INCLUDEPATH += $$(QTDIR)/include/QtCore
INCLUDEPATH += $$(QTDIR)/include/QtNetwork
INCLUDEPATH += $$(QTDIR)/include/QtGui
INCLUDEPATH += $$(QTDIR)/include
INCLUDEPATH += /opt/ELDK-ARM/arm/include
INCLUDEPATH += /opt/ELDK-ARM/arm/usr/include
