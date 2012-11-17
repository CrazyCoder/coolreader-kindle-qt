TARGET = KindleFiveWay
TEMPLATE = lib
CONFIG += plugin
DESTDIR = $$(QTDIR)/plugins/mousedrivers
SOURCES += fivewayplugin.cpp \
    kindlefiveway.cpp
HEADERS += fivewayplugin.h \
    kindlefiveway.h \
    keycodes.h
