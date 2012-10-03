# -------------------------------------------------
# Project created by QtCreator 2009-12-17T16:35:12
# -------------------------------------------------
QT += core \
    gui
TARGET = KindleFiveWay
TEMPLATE = lib
CONFIG += plugin
DESTDIR = $$(QTDIR)/plugins/mousedrivers
SOURCES += fivewayplugin.cpp \
    kindlefiveway.cpp
HEADERS += fivewayplugin.h \
    kindlefiveway.h \
    keycodes.h

INCLUDEPATH += $$(QTDIR)/mkspecs/qws/linux-arm-g++
INCLUDEPATH += $$(QTDIR)/include/QtCore
INCLUDEPATH += $$(QTDIR)/include/QtNetwork
INCLUDEPATH += $$(QTDIR)/include/QtGui
INCLUDEPATH += $$(QTDIR)/include
INCLUDEPATH += /opt/ELDK-ARM/arm/include
INCLUDEPATH += /opt/ELDK-ARM/arm/usr/include
