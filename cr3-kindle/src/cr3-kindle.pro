TARGET = cr3
VERSION = 0.1.4
TEMPLATE = app

CONFIG += TARGET_KINDLE

UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj
RCC_DIR = .rc

DEFINES += USE_FREETYPE=1 \
           LDOM_USE_OWN_MEM_MAN=1 \
           COLOR_BACKBUFFER=1 \
           USE_DOM_UTF8_STORAGE=1 \
           NDEBUG=1

TARGET_KINDLE:DEFINES += QT_KEYPAD_NAVIGATION

debug:DEFINES += _DEBUG=1

QMAKE_CFLAGS_WARN_ON = -Wall -Wno-unused-parameter
QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-unused-parameter

INCLUDEPATH += $$(QTDIR)/include \
               $$(QTDIR)/include/QtCore \
               $$(QTDIR)/include/QtGui \
               $$(QTDIR)/include/freetype2 \
               ../../crengine/crengine/include \
               ../../crengine/thirdparty/antiword

SOURCES +=  main.cpp \
            mainwindow.cpp \
            searchdlg.cpp \
            cr3widget.cpp \
            crqtutil.cpp \
            tocdlg.cpp \
            recentdlg.cpp \
            settings.cpp \
            bookmarklistdlg.cpp \
            filepropsdlg.cpp \
            openfiledlg.cpp \
            virtualkeysdlg.cpp \
            device.cpp \
            touchscreen.cpp \
            brightnesscontrol.cpp

HEADERS +=  mainwindow.h \
            cr3widget.h \
            crqtutil.h \
            tocdlg.h \
            recentdlg.h \
            settings.h \
            bookmarklistdlg.h \
            searchdlg.h \
            filepropsdlg.h \
            openfiledlg.h \
            virtualkeysdlg.h \
            device.h \
            touchscreen.h \
            brightnesscontrol.h

FORMS +=    mainwindow.ui \
            tocdlg.ui \
            recentdlg.ui \
            settings.ui \
            bookmarklistdlg.ui \
            searchdlg.ui \
            filepropsdlg.ui \
            openfiledlg.ui \
            virtualkeysdlg.ui \
            brightnesscontrol.ui

TRANSLATIONS += i18n/Russian.ts \
                i18n/Ukrainian.ts \
                i18n/French.ts \
                i18n/Italian.ts \
                i18n/Hungarian.ts

RESOURCES += cr3res.qrc

LIBS += -lQtGui -lQtCore -lQtNetwork -lQtDBus -lpthread -ldl

arm {
    LIBS += ../../crengine/crengine-build-kindle/libcrengine_kindle.a ../libs/kindle/libantiword.a -lz -lpng -lfreetype -lfontconfig -ljpeg
    QMAKE_POST_LINK=arm-linux-gnueabi-strip -s $(TARGET)
} else {
    LIBS += ../../crengine/crengine-build-desktop/libcrengine_desktop.a ../libs/desktop/libantiword.a `pkg-config --libs zlib libpng freetype2 fontconfig jpeg`
}
