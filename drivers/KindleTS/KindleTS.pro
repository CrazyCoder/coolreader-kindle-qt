QT += core

TARGET = KindleTS
TEMPLATE = lib
CONFIG += plugin
DESTDIR = $$(QTDIR)/plugins/mousedrivers
SOURCES += tsplugin.cpp \
    kindlets.cpp \
    ../../cr3-kindle/src/device.cpp
HEADERS += tsplugin.h \
    kindlets.h

INCLUDEPATH += $$(QTDIR)/mkspecs/qws/linux-arm-g++
INCLUDEPATH += /opt/ELDK-ARM/arm/usr/include
