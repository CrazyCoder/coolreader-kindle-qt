# -------------------------------------------------
# Project created by QtCreator 2009-12-17T16:35:12
# -------------------------------------------------
QT += core \
    gui
TARGET = QKindleFb
TEMPLATE = lib
CONFIG += plugin
DESTDIR = $$(QTDIR)/plugins/gfxdrivers
SOURCES += qkindlefbplugin.cpp \
    qkindlefb.cpp \
    qkindlecursor.cpp

HEADERS += qkindlefb.h \
    linux/einkfb.h \
    qkindlecursor.h

INCLUDEPATH += $$(QTDIR)/mkspecs/qws/linux-arm-g++
INCLUDEPATH += /opt/ELDK-ARM/arm/include
INCLUDEPATH += /opt/ELDK-ARM/arm/usr/include
