TARGET = KindleTS
TEMPLATE = lib
CONFIG += plugin
SOURCES += tsplugin.cpp \
    kindlets.cpp
HEADERS += tsplugin.h \
    kindlets.h

QMAKE_LFLAGS += -rdynamic
LIBS += -L$$OUT_PWD/../../cr3-kindle/src/device/ -ldevice
INCLUDEPATH += $$PWD/../../cr3-kindle/src/device
DEPENDPATH += $$PWD/../../cr3-kindle/src/device

target.path = /mnt/us/qtKindle/plugins/mousedrivers
INSTALLS += target
