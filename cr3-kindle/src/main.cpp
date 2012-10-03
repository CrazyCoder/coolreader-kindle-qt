// CoolReader3 / Qt
// main.cpp - entry point
#include "mainwindow.h"
#ifndef i386
#include "../../drivers/QKindleFb/qkindlefb.h"
#endif

MyApplication *pMyApp;

int main(int argc, char *argv[])
{
    int res = 0;
    {
        lString16 exedir = LVExtractPath(LocalToUnicode(lString8(argv[0])));
        LVAppendPathDelimiter(exedir);
        lString16 datadir = exedir + L"data/";
        lString16 exefontpath = exedir + L"fonts";

        lString16Collection fontDirs;
        fontDirs.add(exefontpath);
        #ifndef i386
        fontDirs.add(lString16(L"/usr/java/lib/fonts"));
        fontDirs.add(lString16(L"/mnt/us/fonts"));
        #endif
        CRPropRef props = LVCreatePropsContainer();
        {
            LVStreamRef cfg = LVOpenFileStream(UnicodeToUtf8(datadir + L"cr3.ini").data(), LVOM_READ);
            if(!cfg.isNull()) props->loadFromStream(cfg.get());
        }

        lString16 lang = props->getStringDef(PROP_WINDOW_LANG, "");
//		InitCREngineLog("./data/cr3.ini");
        InitCREngineLog(props);
        CRLog::info("main()");

        if(!InitCREngine(argv[0], fontDirs)) {
            printf("Cannot init CREngine - exiting\n");
            return 2;
        }
        #ifndef i386
        PrintString(1, 1, "crengine version: " + QString(CR_ENGINE_VERSION), "-c");
        // open framebuffer device and read out info
        int fd = open("/dev/fb0", O_RDONLY);
        struct fb_var_screeninfo screeninfo;
        ioctl(fd, FBIOGET_VSCREENINFO, &screeninfo);
        int width = screeninfo.xres;
        int height = screeninfo.yres;
        QString message = "Please wait while application is loading...";
        int xpos = ((width/12-1)-message.length())/2;
        int ypos = (height/20-2)/2;
        PrintString(xpos, ypos, message);
        #endif

        // set row count
        int rc = props->getIntDef(PROP_WINDOW_ROW_COUNT, 0);
        if(!rc) {
            #ifndef i386
            props->setInt(PROP_WINDOW_ROW_COUNT, height==800 ? 10 : 16);
            #else
            props->setInt(PROP_WINDOW_ROW_COUNT, 10);
            #endif
            LVStreamRef cfg = LVOpenFileStream(UnicodeToUtf8(datadir + L"cr3.ini").data(), LVOM_WRITE);
            props->saveToStream(cfg.get());
        }


        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        MyApplication a(argc, argv);
        pMyApp = &a;
        // set app stylesheet
        #ifndef i386
        QFile qss(QDir::toNativeSeparators(cr2qt(datadir)) + (width==600 ? "stylesheet_k3.qss" : "stylesheet_dx.qss"));
        // set up full update interval for the graphics driver
        QKindleFb *pscreen = static_cast<QKindleFb*>(QScreen::instance());
        pscreen->setFullUpdateEvery(props->getIntDef(PROP_DISPLAY_FULL_UPDATE_INTERVAL, 1));
        #else
        QFile qss(QDir::toNativeSeparators(cr2qt(datadir)) + "stylesheet.qss");
        #endif
        qss.open(QFile::ReadOnly);
        if(qss.error() == QFile::NoError) {
            a.setStyleSheet(qss.readAll());
            qss.close();
        }
        QString translations = cr2qt(datadir) + "i18n";
        QTranslator myappTranslator;
        if(!lang.empty() && lang.compare(L"English")) {
            if(myappTranslator.load(cr2qt(lang), translations))
                QApplication::installTranslator(&myappTranslator);
            else
                qDebug("Can`t load translation file %s from dir %s", UnicodeToUtf8(lang).c_str(), UnicodeToUtf8(qt2cr(translations)).c_str());
        }

        MainWindow mainWin;
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

bool getDirectoryFonts( lString16Collection & pathList, lString16 ext, lString16Collection & fonts, bool absPath )
{
    int foundCount = 0;
    lString16 path;
    for (unsigned di=0; di<pathList.length();di++ ) {
        path = pathList[di];
        LVContainerRef dir = LVOpenDirectory(path.c_str());
        if(!dir.isNull()) {
            CRLog::trace("Checking directory %s", UnicodeToUtf8(path).c_str() );
            for(int i=0; i < dir->GetObjectCount(); i++ ) {
                const LVContainerItemInfo * item = dir->GetObjectInfo(i);
                lString16 fileName = item->GetName();
                if ( !item->IsContainer() && fileName.length()>4 && lString16(fileName, fileName.length()-4, 4)==ext ) {
                    lString16 fn;
                    if ( absPath ) {
                        fn = path;
                        if (!fn.empty() && fn[fn.length()-1]!=PATH_SEPARATOR_CHAR)
                            fn << PATH_SEPARATOR_CHAR;
                    }
                    fn << fileName;
                    foundCount++;
                    fonts.add(fn);
                }
            }
        }
    }
    return foundCount > 0;
}

bool InitCREngine( const char * exename, lString16Collection & fontDirs)
{
    CRLog::trace("InitCREngine(%s)", exename);

    InitFontManager(lString8());

    // Load font definitions into font manager
    // fonts are in files font1.lbf, font2.lbf, ... font32.lbf
    // use fontconfig

    lString16 fontExt = L".ttf";
    lString16Collection fonts;

    getDirectoryFonts( fontDirs, fontExt, fonts, true );
    // load fonts from file
    CRLog::debug("%d font files found", fonts.length());
    if (!fontMan->GetFontCount()) {
        for ( unsigned fi=0; fi<fonts.length(); fi++ ) {
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
    if ( loglevelstr==L"OFF" ) {
        level = CRLog::LL_FATAL;
        logfname.clear();
    } else if ( loglevelstr==L"FATAL" ) {
        level = CRLog::LL_FATAL;
    } else if ( loglevelstr==L"ERROR" ) {
        level = CRLog::LL_ERROR;
    } else if ( loglevelstr==L"WARN" ) {
        level = CRLog::LL_WARN;
    } else if ( loglevelstr==L"INFO" ) {
        level = CRLog::LL_INFO;
    } else if ( loglevelstr==L"DEBUG" ) {
        level = CRLog::LL_DEBUG;
    } else if ( loglevelstr==L"TRACE" ) {
        level = CRLog::LL_TRACE;
    }
    if ( !logfname.empty() ) {
        if ( logfname==L"stdout" )
            CRLog::setStdoutLogger();
        else if ( logfname==L"stderr" )
            CRLog::setStderrLogger();
        else
            CRLog::setFileLogger(UnicodeToUtf8( logfname ).c_str(), autoFlush);
    }
    CRLog::setLogLevel(level);
    CRLog::trace("Log initialization done.");
}

bool myEventFilter(void *message, long *)
{
    QWSEvent * pev = (QWSEvent *) message;

    if(pev->type == QWSEvent::Key)
    {
        QWSKeyEvent *pke = (QWSKeyEvent *) message;
        QWidget *active, *modal, *popup;

        active = qApp->activeWindow();
        modal = qApp->activeModalWidget();
        popup = qApp->activePopupWidget();
//		qDebug("act=%d, modal=%d, pup=%d", (int)active, (int)modal, (int)popup);
        #ifdef i386
        if(pke->simpleData.keycode == Qt::Key_Return) {
            pke->simpleData.keycode = Qt::Key_Select;
            return false;
        }
        #endif
        if(pke->simpleData.keycode == Qt::Key_Sleep) {
            pMyApp->disconnectPowerDaemon();
            return true;
        }
        if(pke->simpleData.keycode == Qt::Key_WakeUp) {
            if(!active->isFullScreen() && !active->isMaximized()) {
                QWidget *mainwnd = qApp->widgetAt(0,0);
                mainwnd->repaint();
            }
            active->repaint();
            pMyApp->connectPowerDaemon();
            return true;
        }
//		qDebug("QWS key: key=%x, press=%x, uni=%x", pke->simpleData.keycode, pke->simpleData.is_press, pke->simpleData.unicode);
        return false;
    }
    return false;
}

void PrintString(int x, int y, const QString message, const QString opt) {
    QStringList list;
    QProcess *myProcess = new QProcess();

    if(!opt.isEmpty()) list << opt;
    list << QString().number(x) << QString().number(y) << message;
    myProcess->start("/usr/sbin/eips", list);
}
