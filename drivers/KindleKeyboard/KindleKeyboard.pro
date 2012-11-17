TARGET = KindleKeyboard
TEMPLATE = lib
CONFIG += plugin
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

target.path = /mnt/us/qtKindle/plugins/kbddrivers
INSTALLS += target
