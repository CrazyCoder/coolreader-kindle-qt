TARGET = QKindleFb
TEMPLATE = lib
CONFIG += plugin
SOURCES += qkindlefbplugin.cpp \
    qkindlefb.cpp \
    qkindlecursor.cpp

HEADERS += qkindlefb.h \
    linux/einkfb.h \
    linux/mxcfb.h \
    qkindlecursor.h

QMAKE_LFLAGS += -rdynamic
LIBS += -L$$OUT_PWD/../../cr3-kindle/src/device/ -ldevice
INCLUDEPATH += $$PWD/../../cr3-kindle/src/device
DEPENDPATH += $$PWD/../../cr3-kindle/src/device

target.path = /mnt/us/qtKindle/plugins/gfxdrivers
INSTALLS += target
