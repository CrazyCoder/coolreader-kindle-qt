TEMPLATE = lib
CONFIG += sharedlib

HEADERS += device.h
SOURCES += device.cpp

# symlinks do not work on Kindle, install the real file
QMAKE_POST_LINK += rm -f $$OUT_PWD/libdevice.so.1; cp -f $$OUT_PWD/libdevice.so.1.0.0 $$OUT_PWD/libdevice.so.1
dlib.files = $$OUT_PWD/libdevice.so.1
dlib.path = /mnt/us/cr3/lib
INSTALLS += dlib
