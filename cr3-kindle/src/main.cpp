// CoolReader3 / Qt
// main.cpp - entry point
#include "mainwindow.h"

#ifndef i386
#include "touchscreen.h"
TouchScreen *pTouch;
#endif

MyApplication *pMyApp;

int main(int argc, char *argv[])
{
    int res = 0;
    {
        Device::instance(); // initialize device
#ifndef i386
        QProcess::execute("eips -c");
        pTouch = new TouchScreen();
#endif
        lString16 exedir = LVExtractPath(LocalToUnicode(lString8(argv[0])));
        LVAppendPathDelimiter(exedir);
        lString16 datadir = exedir + "data/";
        lString16 exefontpath = exedir + "fonts";

        lString16Collection fontDirs;
        fontDirs.add(exefontpath);
#ifndef i386
        fontDirs.add("/usr/java/lib/fonts");
        fontDirs.add("/mnt/us/fonts");
#endif
        CRPropRef props = LVCreatePropsContainer();
        {
            LVStreamRef cfg = LVOpenFileStream(UnicodeToUtf8(datadir + "cr3.ini").data(), LVOM_READ);
            if(!cfg.isNull()) props->loadFromStream(cfg.get());
        }

        lString16 lang = props->getStringDef(PROP_WINDOW_LANG, "");
        InitCREngineLog(props);
        CRLog::info("main()");

        if(!InitCREngine(argv[0], fontDirs)) {
            printf("Cannot init CREngine - exiting\n");
            return 2;
        }
#ifndef i386
        PrintString(1, 1, "crengine version: " + QString(CR_ENGINE_VERSION));
        PrintString(1, 2, QString("build date: %1 %2").arg(__DATE__).arg(__TIME__));
        if (!Device::isTouch()) {
            QString message = "Please wait while application is loading...";
            int xpos = ((Device::getWidth()/12-1)-message.length())/2;
            int ypos = (Device::getHeight()/20-2)/2;
            PrintString(xpos, ypos, message);
        }
#endif
        // to catch crashes and remove current cache file on crash (SIGSEGV etc.)
        crSetSignalHandler();
        // set row count depending on device model (used in lists)
        int rc = props->getIntDef(PROP_WINDOW_ROW_COUNT, 0);
        if(!rc) {
#ifndef i386
            switch(Device::getModel()) {
            case Device::KDX:
                rc = 20;
                break;
            case Device::KT:
            case Device::KPW:
                rc = 8;
                break;
            default:
                rc = 10;
            }
            props->setInt(PROP_WINDOW_ROW_COUNT, rc);
#else
            props->setInt(PROP_WINDOW_ROW_COUNT, 10);
#endif
            LVStreamRef cfg = LVOpenFileStream(UnicodeToUtf8(datadir + "cr3.ini").data(), LVOM_WRITE);
            props->saveToStream(cfg.get());
        }

        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        MyApplication a(argc, argv);
        pMyApp = &a;
        // set app stylesheet
#ifndef i386
        QString style = Device::isTouch() ? "stylesheet_pw.qss" : "stylesheet_k3.qss";
        QFile qss(QDir::toNativeSeparators(cr2qt(datadir)) + style);
        // set up full update interval for the graphics driver
        Device::setFullScreenUpdateEvery(props->getIntDef(PROP_DISPLAY_FULL_UPDATE_INTERVAL, 1));
#else
        QFile qss(QDir::toNativeSeparators(cr2qt(datadir)) + "stylesheet_k3.qss");
#endif
        qss.open(QFile::ReadOnly);
        if(qss.error() == QFile::NoError) {
            a.setStyleSheet(qss.readAll());
            qss.close();
        }

        QMap<QString, QString> langToCode;
        langToCode["Russian"]   = "ru";
        langToCode["French"]    = "fr";
        langToCode["Hungarian"] = "hu";
        langToCode["Italian"]   = "it";
        langToCode["German"]    = "de";
        langToCode["Ukrainian"] = "uk";

        QString translations = cr2qt(datadir) + "i18n";
        QTranslator myappTranslator, qtr;
        if (!lang.empty() && lang.compare("English")) {
            QString lng = cr2qt(lang);
            if (myappTranslator.load(lng, translations)) {
                // default translator for Qt standard dialogs
                if (qtr.load("qt_" + langToCode[lng], translations)) {
                    QApplication::installTranslator(&qtr);
                } else {
                    qDebug() << "Failed to load Qt translation for " << lng;
                }
                // load after default to allow overriding translations
                QApplication::installTranslator(&myappTranslator);
            } else {
                qDebug() << "Can`t load translation file " << lng << " from dir " << translations;
            }
        }

        (void) signal(SIGUSR1, sigCatcher);

        MainWindow mainWin;
        a.setMainWindow(&mainWin);
        mainWin.showFullScreen();
        mainWin.doStartupActions();
        res = a.exec();
    }
    ShutdownCREngine();
    return res;
}

void ShutdownCREngine()
{
    HyphMan::uninit();
    ShutdownFontManager();
    CRLog::setLogger(NULL);
}

bool getDirectoryFonts( lString16Collection & pathList, lString16Collection & ext, lString16Collection & fonts, bool absPath )
{
    int foundCount = 0;
    lString16 path;
    for ( int di=0; di<pathList.length();di++ ) {
        path = pathList[di];
        LVContainerRef dir = LVOpenDirectory(path.c_str());
        if ( !dir.isNull() ) {
            CRLog::trace("Checking directory %s", UnicodeToUtf8(path).c_str() );
            for ( int i=0; i < dir->GetObjectCount(); i++ ) {
                const LVContainerItemInfo * item = dir->GetObjectInfo(i);
                lString16 fileName = item->GetName();
                lString8 fn = UnicodeToLocal(fileName);
                    //printf(" test(%s) ", fn.c_str() );
                if ( !item->IsContainer() ) {
                    bool found = false;
                    lString16 lc = fileName;
                    lc.lowercase();
                    for ( int j=0; j<ext.length(); j++ ) {
                        if ( lc.endsWith(ext[j]) ) {
                            found = true;
                            break;
                        }
                    }
                    if ( !found )
                        continue;
                    lString16 fn;
                    if ( absPath ) {
                        fn = path;
                        if ( !fn.empty() && fn[fn.length()-1]!=PATH_SEPARATOR_CHAR)
                            fn << PATH_SEPARATOR_CHAR;
                    }
                    fn << fileName;
                    foundCount++;
                    fonts.add( fn );
                }
            }
        }
    }
    return foundCount > 0;
}

bool InitCREngine( const char * exename, lString16Collection & fontDirs)
{
    CRLog::trace("InitCREngine(%s)", exename);

    InitFontManager(lString8::empty_str);

    // Load font definitions into font manager
    // fonts are in files font1.lbf, font2.lbf, ... font32.lbf
    // use fontconfig

    lString16Collection fontExt;
    fontExt.add(cs16(".ttf"));
    fontExt.add(cs16(".otf"));
    fontExt.add(cs16(".pfa"));
    fontExt.add(cs16(".pfb"));

    lString16Collection fonts;

    getDirectoryFonts( fontDirs, fontExt, fonts, true );
    // load fonts from file
    CRLog::debug("%d font files found", fonts.length());
    if (!fontMan->GetFontCount()) {
        for (int fi=0; fi<fonts.length(); fi++ ) {
            lString8 fn = UnicodeToLocal(fonts[fi]);
            CRLog::trace("loading font: %s", fn.c_str());
            if ( !fontMan->RegisterFont(fn) )
                CRLog::trace("    failed\n");
        }
    }

    if (!fontMan->GetFontCount()) {
        printf("Fatal Error: Cannot open font file(s) .ttf \nCannot work without font\n" );
        return false;
    }
    printf("%d fonts loaded.\n", fontMan->GetFontCount());
    return true;
}

void InitCREngineLog(CRPropRef props)
{
    if(props.isNull())
    {
        CRLog::setStdoutLogger();
        CRLog::setLogLevel( CRLog::LL_FATAL);
        return;
    }
    lString16 logfname = props->getStringDef(PROP_LOG_FILENAME, "stdout");
#ifdef _DEBUG
    //	lString16 loglevelstr = props->getStringDef(PROP_LOG_LEVEL, "DEBUG");
    lString16 loglevelstr = props->getStringDef(PROP_LOG_LEVEL, "OFF");
#else
    lString16 loglevelstr = props->getStringDef(PROP_LOG_LEVEL, "OFF");
#endif
    bool autoFlush = props->getBoolDef(PROP_LOG_AUTOFLUSH, false);

    CRLog::log_level level = CRLog::LL_INFO;
    if ( loglevelstr=="OFF" ) {
        level = CRLog::LL_FATAL;
        logfname.clear();
    } else if ( loglevelstr=="FATAL" ) {
        level = CRLog::LL_FATAL;
    } else if ( loglevelstr=="ERROR" ) {
        level = CRLog::LL_ERROR;
    } else if ( loglevelstr=="WARN" ) {
        level = CRLog::LL_WARN;
    } else if ( loglevelstr=="INFO" ) {
        level = CRLog::LL_INFO;
    } else if ( loglevelstr=="DEBUG" ) {
        level = CRLog::LL_DEBUG;
    } else if ( loglevelstr=="TRACE" ) {
        level = CRLog::LL_TRACE;
    }
    if ( !logfname.empty() ) {
        if ( logfname=="stdout" )
            CRLog::setStdoutLogger();
        else if ( logfname=="stderr" )
            CRLog::setStderrLogger();
        else
            CRLog::setFileLogger(UnicodeToUtf8( logfname ).c_str(), autoFlush);
    }
    CRLog::setLogLevel(level);
    CRLog::trace("Log initialization done.");
}

void wakeUp()
{
    Device::suspendFramework();
    QWSServer::instance()->refresh();
    pMyApp->connectSystemBus();
}

void gotoSleep() {
    pMyApp->disconnectSystemBus();
}

bool myEventFilter(void *message, long *)
{
    QWSEvent* pev = static_cast<QWSEvent*>(message);
    QWSKeyEvent* pke = static_cast<QWSKeyEvent*>(message);
#ifndef i386
    QWSMouseEvent* pme = pev->asMouse();
#endif
    if(pev->type == QWSEvent::Key)
    {
#ifdef i386
        if(pke->simpleData.keycode == Qt::Key_Return) {
            pke->simpleData.keycode = Qt::Key_Select;
            return false;
        }
#endif
        if(pke->simpleData.keycode == Qt::Key_Sleep) {
            gotoSleep();
            return true;
        }
        if(pke->simpleData.keycode == Qt::Key_WakeUp) {
            wakeUp();
            return true;
        }
        // qDebug("QWS key: key=%x, press=%x, uni=%x", pke->simpleData.keycode, pke->simpleData.is_press, pke->simpleData.unicode);
        return false;
    }
#ifndef i386
    else if (pev->type == QWSEvent::Mouse) {
        bool isFocusInReader = pMyApp->getMainWindow() && pMyApp->getMainWindow()->isFocusInReader();
        return pTouch->filter(pme, isFocusInReader);
    }
#endif
    return false;
}

void PrintString(int x, int y, const QString message, const QString opt) {
    QStringList args;
    if(!opt.isEmpty()) args << opt;
    args << QString().number(x) << QString().number(y) << message;
    QProcess::execute("eips", args);
}

void sigCatcher(int sig) {
  if(sig == SIGUSR1) {
      Device::enableInput(true);
      wakeUp();
  }
}
