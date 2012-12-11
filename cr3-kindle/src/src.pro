TARGET = cr3
VERSION = 0.1.4
TEMPLATE = app

target.path = /mnt/us/cr3
INSTALLS += target

trans.files = $$OUT_PWD/data/i18n/*.qm
trans.path = /mnt/us/cr3/data/i18n
INSTALLS += trans

UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj
RCC_DIR = .rc

CONFIG += TARGET_KINDLE
TARGET_KINDLE:DEFINES += QT_KEYPAD_NAVIGATION

DEFINES += USE_FREETYPE=1 \
    LDOM_USE_OWN_MEM_MAN=1 \
    COLOR_BACKBUFFER=1 \
    USE_DOM_UTF8_STORAGE=1 \
    NDEBUG=1

debug:DEFINES += _DEBUG=1

QMAKE_CFLAGS_WARN_ON = -Wall -Wno-unused-parameter
QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-unused-parameter
QMAKE_POST_LINK = $(STRIP) -s $(TARGET)

INCLUDEPATH += $$(QTDIR)/include/freetype2 \
    ../../crengine/crengine/include \
    ../../crengine/thirdparty/antiword \
    ./device

SOURCES += main.cpp \
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
    touchscreen.cpp \
    brightnesscontrol.cpp

HEADERS += mainwindow.h \
    cr3widget.h \
    crqtutil.h \
    tocdlg.h \
    recentdlg.h \
    settings.h \
    bookmarklistdlg.h \
    searchdlg.h \
    filepropsdlg.h \
    openfiledlg.h \
    touchscreen.h \
    brightnesscontrol.h

FORMS += mainwindow.ui \
    tocdlg.ui \
    recentdlg.ui \
    settings.ui \
    bookmarklistdlg.ui \
    searchdlg.ui \
    filepropsdlg.ui \
    openfiledlg.ui \
    brightnesscontrol.ui

TRANSLATIONS += i18n/Russian.ts \
    i18n/Ukrainian.ts \
    i18n/French.ts \
    i18n/Italian.ts \
    i18n/Hungarian.ts \
    i18n/German.ts

QT_TRANSLATIONS += i18n/qt_ru.ts \
    i18n/qt_uk.ts \
    i18n/qt_fr.ts \
    i18n/qt_hu.ts \
    i18n/qt_de.ts

RESOURCES += cr3res.qrc

lrelease.input = TRANSLATIONS + QT_TRANSLATIONS
lrelease.output = data/i18n/${QMAKE_FILE_BASE}.qm
lrelease.commands = $$[QT_INSTALL_BINS]/lrelease -silent -compress ${QMAKE_FILE_IN} -qm data/i18n/${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease
DEPENDPATH += i18n

LIBS += -lQtGui -lQtCore -lQtNetwork -lQtDBus -lpthread -ldl

# depends on crengine + antiword (precompiled)
LIBS += -L$$OUT_PWD/../../crengine/crengine -lcrengine -lantiword
DEPENDPATH += $$PWD/../../crengine/crengine
PRE_TARGETDEPS += $$OUT_PWD/../../crengine/crengine/libcrengine.a

# depends on device (Kindle model detection library)
LIBS += -L$$OUT_PWD/device/ -ldevice
DEPENDPATH += $$PWD/device

arm {
    LIBS += -L$$PWD/../libs/kindle -lz -lpng -lfreetype -lfontconfig -ljpeg
} else {
    LIBS += -L$$PWD/../libs/desktop `pkg-config --libs zlib libpng freetype2 fontconfig jpeg`
}
