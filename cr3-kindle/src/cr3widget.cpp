#include "lvdocview.h"
#include "crtrace.h"
#include "props.h"
#include "cr3widget.h"
#include "crqtutil.h"
#include "qpainter.h"
#include "settings.h"
#include <QtGui/QResizeEvent>
#include <QtGui/QScrollBar>
#include <QtGui/QMenu>
#include <QtGui/QStyleFactory>
#include <QtGui/QStyle>
#include <QtGui/QApplication>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QDesktopServices>

static int cr_interline_spaces[] = { 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150 };

/// to hide non-qt implementation, place all crengine-related fields here
class CR3View::DocViewData
{
    friend class CR3View;
    lString16 _settingsFileName;
    lString16 _historyFileName;
    CRPropRef _props;
};

static void replaceColor( char * str, lUInt32 color )
{
    // in line like "0 c #80000000",
    // replace value of color
    for ( int i=0; i<8; i++ ) {
        str[i+5] = toHexDigit((color>>28) & 0xF);
        color <<= 4;
    }
}

static LVRefVec<LVImageSource> getBatteryIcons( lUInt32 color )
{
    CRLog::debug("Making list of Battery icon bitmats");
    lUInt32 cl1 = 0x00000000|(color&0xFFFFFF);
    lUInt32 cl2 = 0x40000000|(color&0xFFFFFF);
    lUInt32 cl3 = 0x80000000|(color&0xFFFFFF);
    lUInt32 cl4 = 0xF0000000|(color&0xFFFFFF);

    static char color1[] = "0 c #80000000";
    static char color2[] = "X c #80000000";
    static char color3[] = "o c #80AAAAAA";
    static char color4[] = ". c #80FFFFFF";
#define BATTERY_HEADER \
    "28 15 5 1", \
    color1, \
    color2, \
    color3, \
    color4, \
    "  c None",

    static const char * battery8[] = {
        BATTERY_HEADER
        "   .........................",
        "   .00000000000000000000000.",
        "   .0.....................0.",
        "....0.XXXX.XXXX.XXXX.XXXX.0.",
        ".0000.XXXX.XXXX.XXXX.XXXX.0.",
        ".0..0.XXXX.XXXX.XXXX.XXXX.0.",
        ".0..0.XXXX.XXXX.XXXX.XXXX.0.",
        ".0..0.XXXX.XXXX.XXXX.XXXX.0.",
        ".0..0.XXXX.XXXX.XXXX.XXXX.0.",
        ".0..0.XXXX.XXXX.XXXX.XXXX.0.",
        ".0000.XXXX.XXXX.XXXX.XXXX.0.",
        "....0.XXXX.XXXX.XXXX.XXXX.0.",
        "   .0.....................0.",
        "   .00000000000000000000000.",
        "   .........................",
    };
    static const char * battery7[] = {
        BATTERY_HEADER
        "   .........................",
        "   .00000000000000000000000.",
        "   .0.....................0.",
        "....0.oooo.XXXX.XXXX.XXXX.0.",
        ".0000.oooo.XXXX.XXXX.XXXX.0.",
        ".0..0.oooo.XXXX.XXXX.XXXX.0.",
        ".0..0.oooo.XXXX.XXXX.XXXX.0.",
        ".0..0.oooo.XXXX.XXXX.XXXX.0.",
        ".0..0.oooo.XXXX.XXXX.XXXX.0.",
        ".0..0.oooo.XXXX.XXXX.XXXX.0.",
        ".0000.oooo.XXXX.XXXX.XXXX.0.",
        "....0.oooo.XXXX.XXXX.XXXX.0.",
        "   .0.....................0.",
        "   .00000000000000000000000.",
        "   .........................",
    };
    static const char * battery6[] = {
        BATTERY_HEADER
        "   .........................",
        "   .00000000000000000000000.",
        "   .0.....................0.",
        "....0......XXXX.XXXX.XXXX.0.",
        ".0000......XXXX.XXXX.XXXX.0.",
        ".0..0......XXXX.XXXX.XXXX.0.",
        ".0..0......XXXX.XXXX.XXXX.0.",
        ".0..0......XXXX.XXXX.XXXX.0.",
        ".0..0......XXXX.XXXX.XXXX.0.",
        ".0..0......XXXX.XXXX.XXXX.0.",
        ".0000......XXXX.XXXX.XXXX.0.",
        "....0......XXXX.XXXX.XXXX.0.",
        "   .0.....................0.",
        "   .00000000000000000000000.",
        "   .........................",
    };
    static const char * battery5[] = {
        BATTERY_HEADER
        "   .........................",
        "   .00000000000000000000000.",
        "   .0.....................0.",
        "....0......oooo.XXXX.XXXX.0.",
        ".0000......oooo.XXXX.XXXX.0.",
        ".0..0......oooo.XXXX.XXXX.0.",
        ".0..0......oooo.XXXX.XXXX.0.",
        ".0..0......oooo.XXXX.XXXX.0.",
        ".0..0......oooo.XXXX.XXXX.0.",
        ".0..0......oooo.XXXX.XXXX.0.",
        ".0000......oooo.XXXX.XXXX.0.",
        "....0......oooo.XXXX.XXXX.0.",
        "   .0.....................0.",
        "   .00000000000000000000000.",
        "   .........................",
    };
    static const char * battery4[] = {
        BATTERY_HEADER
        "   .........................",
        "   .00000000000000000000000.",
        "   .0.....................0.",
        "....0...........XXXX.XXXX.0.",
        ".0000...........XXXX.XXXX.0.",
        ".0..0...........XXXX.XXXX.0.",
        ".0..0...........XXXX.XXXX.0.",
        ".0..0...........XXXX.XXXX.0.",
        ".0..0...........XXXX.XXXX.0.",
        ".0..0...........XXXX.XXXX.0.",
        ".0000...........XXXX.XXXX.0.",
        "....0...........XXXX.XXXX.0.",
        "   .0.....................0.",
        "   .00000000000000000000000.",
        "   .........................",
    };
    static const char * battery3[] = {
        BATTERY_HEADER
        "   .........................",
        "   .00000000000000000000000.",
        "   .0.....................0.",
        "....0...........oooo.XXXX.0.",
        ".0000...........oooo.XXXX.0.",
        ".0..0...........oooo.XXXX.0.",
        ".0..0...........oooo.XXXX.0.",
        ".0..0...........oooo.XXXX.0.",
        ".0..0...........oooo.XXXX.0.",
        ".0..0...........oooo.XXXX.0.",
        ".0000...........oooo.XXXX.0.",
        "....0...........oooo.XXXX.0.",
        "   .0.....................0.",
        "   .00000000000000000000000.",
        "   .........................",
    };
    static const char * battery2[] = {
        BATTERY_HEADER
        "   .........................",
        "   .00000000000000000000000.",
        "   .0.....................0.",
        "....0................XXXX.0.",
        ".0000................XXXX.0.",
        ".0..0................XXXX.0.",
        ".0..0................XXXX.0.",
        ".0..0................XXXX.0.",
        ".0..0................XXXX.0.",
        ".0..0................XXXX.0.",
        ".0000................XXXX.0.",
        "....0................XXXX.0.",
        "   .0.....................0.",
        "   .00000000000000000000000.",
        "   .........................",
    };
    static const char * battery1[] = {
        BATTERY_HEADER
        "   .........................",
        "   .00000000000000000000000.",
        "   .0.....................0.",
        "   .0................oooo.0.",
        ".0000................oooo.0.",
        ".0..0................oooo.0.",
        ".0..0................oooo.0.",
        ".0..0................oooo.0.",
        ".0..0................oooo.0.",
        ".0..0................oooo.0.",
        ".0000................oooo.0.",
        "   .0................oooo.0.",
        "   .0.....................0.",
        "   .00000000000000000000000.",
        "   .........................",
    };
    static const char * battery0[] = {
        BATTERY_HEADER
        "   .........................",
        "   .00000000000000000000000.",
        "   .0.....................0.",
        "   .0.....................0.",
        ".0000.....................0.",
        ".0..0.....................0.",
        ".0..0.....................0.",
        ".0..0.....................0.",
        ".0..0.....................0.",
        ".0..0.....................0.",
        ".0000.....................0.",
        "....0.....................0.",
        "   .0.....................0.",
        "   .00000000000000000000000.",
        "   .........................",
    };
    //#endif

    static const char * battery_charge[] = {
        BATTERY_HEADER
        "   .........................",
        "   .00000000000000000000000.",
        "   .0.....................0.",
        "....0.....................0.",
        ".0000............XX.......0.",
        ".0..0...........XXXX......0.",
        ".0..0..XX......XXXXXX.....0.",
        ".0..0...XXX...XXXX..XX....0.",
        ".0..0....XXX..XXXX...XX...0.",
        ".0..0.....XXXXXXX.....XX..0.",
        ".0000.......XXXX..........0.",
        "....0........XX...........0.",
        "   .0.....................0.",
        "   .00000000000000000000000.",
        "   .........................",
    };
    static const char * battery_frame[] = {
        BATTERY_HEADER
        "   .........................",
        "   .00000000000000000000000.",
        "   .0.....................0.",
        "....0.....................0.",
        ".0000.....................0.",
        ".0..0.....................0.",
        ".0..0.....................0.",
        ".0..0.....................0.",
        ".0..0.....................0.",
        ".0..0.....................0.",
        ".0000.....................0.",
        "....0.....................0.",
        "   .0.....................0.",
        "   .00000000000000000000000.",
        "   .........................",
    };

    const char * * icon_bpm[] = {
        battery_charge,
        battery0,
        battery1,
        battery2,
        battery3,
        battery4,
        battery5,
        battery6,
        battery7,
        battery8,
        battery_frame,
        NULL
    };

    replaceColor( color1, cl1 );
    replaceColor( color2, cl2 );
    replaceColor( color3, cl3 );
    replaceColor( color4, cl4 );

    LVRefVec<LVImageSource> icons;
    for ( int i=0; icon_bpm[i]; i++ )
        icons.add( LVCreateXPMImageSource( icon_bpm[i] ) );

    return icons;
}

//DECL_DEF_CR_FONT_SIZES

CR3View::CR3View( QWidget *parent)
    : QWidget( parent, Qt::WindowFlags() ), _scroll(NULL), _propsCallback(NULL)
    , _normalCursor(Qt::ArrowCursor), _linkCursor(Qt::PointingHandCursor)
    , _selCursor(Qt::IBeamCursor), _waitCursor(Qt::WaitCursor)
    , _selecting(false), _selected(false), _editMode(false), _lastBatteryState(CR_BATTERY_STATE_NO_BATTERY)
{
#if WORD_SELECTOR_ENABLED==1
    _wordSelector = NULL;
#endif
    _data = new DocViewData();
    _data->_props = LVCreatePropsContainer();
    _docview = new LVDocView();
    _docview->setCallback(this);
    _selStart = ldomXPointer();
    _selEnd = ldomXPointer();
    _selText.clear();
    ldomXPointerEx p1;
    ldomXPointerEx p2;
    _selRange.setStart(p1);
    _selRange.setEnd(p2);

    int cr_font_sizes[MAX_CR_FONT_SIZE - MIN_CR_FONT_SIZE + 1];
    for(unsigned int i = 0; i < sizeof(cr_font_sizes)/sizeof(int); i++)
        cr_font_sizes[i] = MIN_CR_FONT_SIZE + i;
    LVArray<int> sizes(cr_font_sizes, sizeof(cr_font_sizes)/sizeof(int));
    _docview->setFontSizes(sizes, false);

    _docview->setBatteryIcons( getBatteryIcons(0x000000) );
    //	_docview->setBatteryState(CR_BATTERY_STATE_NO_BATTERY); // don't show battery
    updateDefProps();
    //	setMouseTracking(true);
    setFocusPolicy(Qt::NoFocus);
    //	setFocusPolicy(Qt::StrongFocus);
}

void CR3View::updateDefProps()
{
    _data->_props->setIntDef(PROP_APP_START_ACTION, 0);
    _data->_props->setIntDef(PROP_REPLACE_SCREENSAVER, 0);
    _data->_props->setIntDef(PROP_LAST_TAB, 0);
    _data->_props->setIntDef(PROP_FONT_SIZE, DEF_FONT_SIZE);
    _data->_props->setIntDef(PROP_STATUS_FONT_SIZE, DEF_HEADER_FONT_SIZE);
    _data->_props->setIntDef(PROP_LANDSCAPE_PAGES, 1);
    _data->_props->setIntDef(PROP_PAGE_VIEW_MODE, 1);
    _data->_props->setIntDef(PROP_FLOATING_PUNCTUATION, 0);
    _data->_props->setIntDef(PROP_SHOW_TIME, 1);
    _data->_props->setStringDef(PROP_FONT_GAMMA, "1.5");
}

CR3View::~CR3View()
{
#if WORD_SELECTOR_ENABLED==1
    if ( _wordSelector )
        delete _wordSelector;
#endif
    _docview->savePosition();
    saveHistory( QString() );
    saveSettings( QString() );
    delete _docview;
    delete _data;
}

#if WORD_SELECTOR_ENABLED==1
void CR3View::startWordSelection() {
    if ( isWordSelection() )
        endWordSelection();
    _wordSelector = new LVPageWordSelector(_docview);
    update();
}

QString CR3View::endWordSelection() {
    QString text;
    if ( isWordSelection() ) {
        ldomWordEx * word = _wordSelector->getSelectedWord();
        if ( word )
            text = cr2qt(word->getText());
        delete _wordSelector;
        _wordSelector = NULL;
        _docview->clearSelection();
        update();
    }
    return text;
}
#endif

void CR3View::setHyphDir(QString dirname)
{
    HyphMan::initDictionaries(qt2cr(dirname));
    _hyphDicts.clear();
    for ( int i=0; i<HyphMan::getDictList()->length(); i++ ) {
        HyphDictionary * item = HyphMan::getDictList()->get( i );
        QString fn = cr2qt( item->getId() );
        _hyphDicts.append( fn );
    }
}

const QStringList & CR3View::getHyphDicts()
{
    return _hyphDicts;
}

LVTocItem * CR3View::getToc()
{
    return _docview->getToc();
}

/// go to position specified by xPointer string
void CR3View::goToXPointer(QString xPointer)
{
    ldomXPointer p = _docview->getDocument()->createXPointer(qt2cr(xPointer));
    _docview->savePosToNavigationHistory();

    doCommand( DCMD_GO_POS, p.toPoint().y );
}

/// returns current page
int CR3View::getCurPage()
{
    return _docview->getCurPage();
}

void CR3View::setDocumentText(QString text)
{
    _docview->savePosition();
    clearSelection();
    _docview->createDefaultDocument( lString16::empty_str, qt2cr(text) );
}

bool CR3View::loadLastDocument()
{
    CRFileHist * hist = _docview->getHistory();
    if ( !hist || hist->getRecords().length()<=0 )
        return false;
    return loadDocument( cr2qt(hist->getRecords()[0]->getFilePathName()) );
}

bool CR3View::loadDocument(QString fileName)
{
    _docview->savePosition();
    clearSelection();

    bool res = _docview->LoadDocument( qt2cr(fileName).c_str() );
    if ( res ) {
        _docview->swapToCache();
        QByteArray utf8 = fileName.toUtf8();
        CRLog::debug( "Trying to restore position for %s", utf8.constData() );
        _docview->restorePosition();
    } else
        _docview->createDefaultDocument( lString16::empty_str, qt2cr(tr("Error while opening document ") + fileName) );
    update();
    return res;
}

void CR3View::resizeEvent( QResizeEvent * event)
{
    QSize sz = event->size();
    _docview->Resize( sz.width(), sz.height() );
}

int getBatteryState()
{
    return CR_BATTERY_STATE_NO_BATTERY;
}

void CR3View::paintEvent( QPaintEvent * event)
{
    QPainter painter(this);
    QRect rc = rect();
    int newBatteryState = getBatteryState();
    if (_lastBatteryState != newBatteryState) {
        _docview->setBatteryState( newBatteryState );
        _lastBatteryState = newBatteryState;
    }
    LVDocImageRef ref = _docview->getPageImage(0);
    if ( ref.isNull() ) {
        return;
    }
    LVDrawBuf * buf = ref->getDrawBuf();
    int dx = buf->GetWidth();
    int dy = buf->GetHeight();
    if ( buf->GetBitsPerPixel()==16 ) {
        QImage img(dx, dy, QImage::Format_RGB16 );
        for ( int i=0; i<dy; i++ ) {
            unsigned char * dst = img.scanLine( i );
            unsigned char * src = buf->GetScanLine(i);
            for ( int x=0; x<dx; x++ ) {
                *dst++ = *src++;
                *dst++ = *src++;
            }
        }
        painter.drawImage( rc, img );
    } else if ( buf->GetBitsPerPixel()==32 ) {
        QImage img(dx, dy, QImage::Format_RGB32 );
        for ( int i=0; i<dy; i++ ) {
            unsigned char * dst = img.scanLine( i );
            unsigned char * src = buf->GetScanLine(i);
            for ( int x=0; x<dx; x++ ) {
                *dst++ = *src++;
                *dst++ = *src++;
                *dst++ = *src++;
                *dst++ = 0xFF;
                src++;
            }
        }
        painter.drawImage( rc, img );
    }
    if ( _editMode ) {
        // draw caret
        lvRect cursorRc;
        if ( _docview->getCursorRect( cursorRc, false ) ) {
            if ( cursorRc.left<0 )
                cursorRc.left = 0;
            if ( cursorRc.top<0 )
                cursorRc.top = 0;
            if ( cursorRc.right>dx )
                cursorRc.right = dx;
            if ( cursorRc.bottom > dy )
                cursorRc.bottom = dy;
            if ( !cursorRc.isEmpty() ) {
                painter.setPen(QColor(255,255,255));
                painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
                painter.drawRect( cursorRc.left, cursorRc.top, cursorRc.width(), cursorRc.height() );
            }
        }
    }
}

void CR3View::scrollTo( int value )
{
    int currPos = _docview->getScrollInfo()->pos;
    if ( currPos != value ) {
        doCommand( DCMD_GO_SCROLL_POS, value );
    }
}

void CR3View::doCommand( int cmd, int param )
{
    _docview->doCommand( (LVDocCmd)cmd, param );
    update();
}

void CR3View::togglePageScrollView()
{
    if(_editMode) return;
    doCommand( DCMD_TOGGLE_PAGE_SCROLL_VIEW, 1);
    refreshPropFromView( PROP_PAGE_VIEW_MODE);
}

void CR3View::setEditMode(bool flgEdit)
{
    if ( _editMode == flgEdit )
        return;

    if ( flgEdit && _data->_props->getIntDef( PROP_PAGE_VIEW_MODE, 0 ) )
        togglePageScrollView();
    _editMode = flgEdit;
    update();
}

void CR3View::refreshPropFromView(const char * propName)
{
    _data->_props->setString( propName, _docview->propsGetCurrent()->getStringDef(propName, "" ));
}

void CR3View::zoomFont(int param)
{
    if(param>0)
        doCommand(DCMD_ZOOM_IN, 1);
    else
        doCommand(DCMD_ZOOM_OUT, 1);
    refreshPropFromView(PROP_FONT_SIZE);
}

void CR3View::zoomHeaderFont(int param)
{
    int size = _data->_props->getIntDef(PROP_STATUS_FONT_SIZE, DEF_HEADER_FONT_SIZE);
    if(param>0) {
        size+=1;
        if(size>MAX_CR_HEADER_FONT_SIZE) return;
    } else {
        size-=1;
        if(size<MIN_CR_HEADER_FONT_SIZE) return;
    }
    PropsRef props = Props::create();
    props->setInt(PROP_STATUS_FONT_SIZE, size);
    setOptions(props);
}

QScrollBar * CR3View::scrollBar() const
{
    return _scroll;
}

void CR3View::setScrollBar( QScrollBar * scroll )
{
    _scroll = scroll;
    if ( _scroll!=NULL ) {
        QObject::connect(_scroll, SIGNAL(valueChanged(int)), this, SLOT(scrollTo(int)));
    }
}

/// load fb2.css file
bool CR3View::loadCSS( QString fn )
{
    lString16 filename( qt2cr(fn) );
    lString8 css;
    if ( LVLoadStylesheetFile( filename, css ) ) {
        if ( !css.empty() ) {
            QFileInfo f( fn );
            CRLog::info( "Using style sheet from %s", fn.toUtf8().constData() );
            _cssDir = f.absolutePath() + "/";
            _docview->setStyleSheet( css );
            return true;
        }
    }
    return false;
}

/// load settings from file
bool CR3View::loadSettings( QString fn )
{
    lString16 filename(qt2cr(fn));
    _data->_settingsFileName = filename;
    LVStreamRef stream = LVOpenFileStream( filename.c_str(), LVOM_READ );
    bool res = false;
    if ( !stream.isNull() && _data->_props->loadFromStream( stream.get() ) ) {
        CRLog::error("Loading settings from file %s", fn.toUtf8().data() );
        res = true;
    } else {
        CRLog::error("Cannot load settings from file %s", fn.toUtf8().data() );
    }
    _docview->propsUpdateDefaults(_data->_props);
    updateDefProps();
    CRPropRef r = _docview->propsApply(_data->_props);
    PropsRef unknownOptions = cr2qt(r);
    if (_propsCallback != NULL)
        _propsCallback->onPropsChange(unknownOptions);

    rowCount = _data->_props->getIntDef(PROP_WINDOW_ROW_COUNT, 10);
    return res;
}

/// toggle boolean property
void CR3View::toggleProperty( const char * name )
{
    int state = _data->_props->getIntDef( name, 0 )!=0 ? 0 : 1;
    PropsRef props = Props::create();
    props->setString(name, state?"1":"0");
    setOptions(props);
}

/// set new option values
void CR3View::propsApply(PropsRef props)
{
    //	CRPropRef changed = _data->_props ^ qt2cr(props);
    CRPropRef newProps = qt2cr(props);
    _docview->propsApply(newProps);
    update();
}

PropsRef CR3View::setOptions(PropsRef props)
{
    CRPropRef changed = _data->_props ^ qt2cr(props);
    _data->_props = changed | _data->_props;
    CRPropRef r = _docview->propsApply(changed);
    PropsRef unknownOptions = cr2qt(r);
    if(_propsCallback != NULL) _propsCallback->onPropsChange(unknownOptions);
    saveSettings(QString());
    update();
    return unknownOptions;
}

/// get current option values
PropsRef CR3View::getOptions()
{
    return Props::clone(cr2qt( _data->_props ));
}

/// save settings from file
bool CR3View::saveSettings( QString fn )
{
    lString16 filename( qt2cr(fn) );
    crtrace log;
    if ( filename.empty() )
        filename = _data->_settingsFileName;
    if ( filename.empty() )
        return false;
    _data->_settingsFileName = filename;
    log << "V3DocViewWin::saveSettings(" << filename << ")";
    LVStreamRef stream = LVOpenFileStream( filename.c_str(), LVOM_WRITE );
    if ( !stream ) {
        lString16 path16 = LVExtractPath( filename );
        lString8 path = UnicodeToUtf8(path16);
        if ( !LVCreateDirectory( path16 ) ) {
            CRLog::error("Cannot create directory %s", path.c_str() );
        } else {
            stream = LVOpenFileStream( filename.c_str(), LVOM_WRITE );
        }
    }
    if ( stream.isNull() ) {
        lString8 fn = UnicodeToUtf8( filename );
        CRLog::error("Cannot save settings to file %s", fn.c_str() );
        return false;
    }
    return _data->_props->saveToStream( stream.get() );
}

/// load history from file
bool CR3View::loadHistory( QString fn )
{
    lString16 filename( qt2cr(fn) );
    CRLog::trace("V3DocViewWin::loadHistory( %s )", UnicodeToUtf8(filename).c_str());
    _data->_historyFileName = filename;
    LVStreamRef stream = LVOpenFileStream( filename.c_str(), LVOM_READ );
    if ( stream.isNull() ) {
        return false;
    }
    if ( !_docview->getHistory()->loadFromStream( stream ) )
        return false;
    return true;
}

/// save history to file
bool CR3View::saveHistory( QString fn )
{
    lString16 filename( qt2cr(fn) );
    crtrace log;
    if ( filename.empty() )
        filename = _data->_historyFileName;
    if ( filename.empty() ) {
        CRLog::info("Cannot write history file - no file name specified");
        return false;
    }
    //CRLog::debug("Exporting bookmarks to %s", UnicodeToUtf8(_bookmarkDir).c_str());
    //_docview->exportBookmarks(_bookmarkDir); //use default filename
    lString16 bmdir = qt2cr(_bookmarkDir);
    LVAppendPathDelimiter( bmdir );
    _docview->exportBookmarks( bmdir ); //use default filename
    _data->_historyFileName = filename;
    log << "V3DocViewWin::saveHistory(" << filename << ")";
    LVStreamRef stream = LVOpenFileStream( filename.c_str(), LVOM_WRITE );
    if ( !stream ) {
        lString16 path16 = LVExtractPath( filename );
        lString8 path = UnicodeToUtf8(path16);
        if ( !LVCreateDirectory( path16 ) ) {
            CRLog::error("Cannot create directory %s", path.c_str() );
        } else {
            stream = LVOpenFileStream( filename.c_str(), LVOM_WRITE );
        }
    }
    if ( stream.isNull() ) {
        CRLog::error("Error while creating history file %s - position will be lost", UnicodeToUtf8(filename).c_str() );
        return false;
    }
    return _docview->getHistory()->saveToStream( stream.get() );
}

void CR3View::contextMenu( QPoint pos )
{
}

/// returns true if point is inside selected text
bool CR3View::isPointInsideSelection( QPoint pos )
{
    if ( !_selected )
        return false;
    lvPoint pt( pos.x(), pos.y() );
    ldomXPointerEx p( _docview->getNodeByPoint( pt ) );
    if ( p.isNull() )
        return false;
    return _selRange.isInside( p );
}

void CR3View::clearSelection()
{
    if ( _selected ) {
        _docview->clearSelection();
        update();
    }
    _selecting = false;
    _selected = false;
    _selStart = ldomXPointer();
    _selEnd = ldomXPointer();
    _selText.clear();
    ldomXPointerEx p1;
    ldomXPointerEx p2;
    _selRange.setStart(p1);
    _selRange.setEnd(p2);
}

void CR3View::startSelection( ldomXPointer p )
{
    clearSelection();
    _selecting = true;
    _selStart = p;
    updateSelection( p );
}

bool CR3View::endSelection( ldomXPointer p )
{
    if ( !_selecting )
        return false;
    updateSelection( p );
    if ( _selected ) {

    }
    _selecting = false;
    return _selected;
}

bool CR3View::updateSelection( ldomXPointer p )
{
    if ( !_selecting )
        return false;
    _selEnd = p;
    ldomXRange r( _selStart, _selEnd );
    if ( r.getStart().isNull() || r.getEnd().isNull() )
        return false;
    r.sort();
    if ( !_editMode ) {
        if ( !r.getStart().isVisibleWordStart() )
            r.getStart().prevVisibleWordStart();
        //lString16 start = r.getStart().toString();
        if ( !r.getEnd().isVisibleWordEnd() )
            r.getEnd().nextVisibleWordEnd();
    }
    if ( r.isNull() )
        return false;
    //lString16 end = r.getEnd().toString();
    //CRLog::debug("Range: %s - %s", UnicodeToUtf8(start).c_str(), UnicodeToUtf8(end).c_str());
    r.setFlags(1);
    _docview->selectRange( r );
    _selText = cr2qt( r.getRangeText( '\n', 10000 ) );
    _selected = true;
    _selRange = r;
    update();
    return true;
}

/// Override to handle external links
void CR3View::OnExternalLink( lString16 url, ldomNode * node )
{
    // TODO: add support of file links
    // only URL supported for now
    QUrl qturl( cr2qt(url) );
    QDesktopServices::openUrl( qturl );
}

/// create bookmark
CRBookmark * CR3View::createBookmark()
{
    CRBookmark * bm = NULL;
    if ( getSelectionText().length()>0 && !_selRange.isNull() ) {
        bm = getDocView()->saveRangeBookmark( _selRange, bmkt_comment, lString16::empty_str );
    } else {
        bm = getDocView()->saveCurrentPageBookmark(lString16::empty_str);
    }

    return bm;
}

void CR3View::goToBookmark( CRBookmark * bm )
{
    ldomXPointer start = _docview->getDocument()->createXPointer( bm->getStartPos() );
    ldomXPointer end = _docview->getDocument()->createXPointer( bm->getEndPos() );
    if ( start.isNull() )
        return;
    if ( end.isNull() )
        end = start;
    //	startSelection(start);
    //	endSelection(end);
    goToXPointer( cr2qt(bm->getStartPos()));
    update();
}

/// format detection finished
void CR3View::OnLoadFileFormatDetected( doc_format_t fileFormat )
{
    QString filename = "fb2.css";
    if ( _cssDir.length() > 0 ) {
        switch ( fileFormat ) {
        case doc_format_txt:
            filename = "txt.css";
            break;
        case doc_format_rtf:
            filename = "rtf.css";
            break;
        case doc_format_epub:
            filename = "epub.css";
            break;
        case doc_format_html:
            filename = "htm.css";
            break;
        case doc_format_doc:
            filename = "doc.css";
            break;
        case doc_format_chm:
            filename = "chm.css";
            break;
        default:
            // do nothing
            ;
        }
        CRLog::debug( "CSS file to load: %s", filename.toUtf8().constData() );
        if ( QFileInfo( _cssDir + filename ).exists() ) {
            loadCSS( _cssDir + filename );
        } else if ( QFileInfo( _cssDir + "fb2.css" ).exists() ) {
            loadCSS( _cssDir + "fb2.css" );
        }
    }
}

/// on starting file loading
void CR3View::OnLoadFileStart(lString16 filename)
{

}

/// file load finished with error
void CR3View::OnLoadFileError( lString16 message )
{

}

/// file loading is finished successfully - drawCoveTo() may be called there
void CR3View::OnLoadFileEnd()
{

}

/// document formatting started
void CR3View::OnFormatStart()
{

}

/// document formatting finished
void CR3View::OnFormatEnd()
{

}

/// set bookmarks dir
void CR3View::setBookmarksDir( QString dirname )
{
    _bookmarkDir = dirname;
}

void CR3View::keyPressEvent(QKeyEvent * event)
{
#if 0
    // testing sentence navigation/selection
    switch ( event->key() ) {
    case Qt::Key_Z:
        _docview->doCommand(DCMD_SELECT_FIRST_SENTENCE);
        update();
        return;
    case Qt::Key_X:
        _docview->doCommand(DCMD_SELECT_NEXT_SENTENCE);
        update();
        return;
    case Qt::Key_C:
        _docview->doCommand(DCMD_SELECT_PREV_SENTENCE);
        update();
        return;
    }
#endif
#if WORD_SELECTOR_ENABLED==1
    if (isWordSelection()) {
        MoveDirection dir = DIR_ANY;
        switch ( event->key() ) {
        case Qt::Key_Left:
        case Qt::Key_A:
            dir = DIR_LEFT;
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            dir = DIR_RIGHT;
            break;
        case Qt::Key_W:
        case Qt::Key_Up:
            dir = DIR_UP;
            break;
        case Qt::Key_S:
        case Qt::Key_Down:
            dir = DIR_DOWN;
            break;
        case Qt::Key_Q:
        case Qt::Key_Enter:
        case Qt::Key_Escape:
        {
            QString text = endWordSelection();
            event->setAccepted(true);
            CRLog::debug("Word selected: %s", LCSTR(qt2cr(text)));
        }
            return;
        case Qt::Key_Backspace:
            _wordSelector->reducePattern();
            update();
            break;
        default:
        {
            int key = event->key();
            if ( key>=Qt::Key_A && key<=Qt::Key_Z ) {
                QString text = event->text();
                if ( text.length()==1 ) {
                    _wordSelector->appendPattern(qt2cr(text));
                    update();
                }
            }
        }
            event->setAccepted(true);
            return;
        }
        int dist = event->modifiers() & Qt::ShiftModifier ? 5 : 1;
        _wordSelector->moveBy(dir, dist);
        update();
        event->setAccepted(true);
    } else {
        if ( event->key()==Qt::Key_F3 && (event->modifiers() & Qt::ShiftModifier) ) {
            startWordSelection();
            event->setAccepted(true);
            return;
        }

    }
#endif
    if ( !_editMode )
        return;
    switch ( event->key() ) {
    case Qt::Key_Left:
        break;
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Home:
    case Qt::Key_End:
    case Qt::Key_PageUp:
    case Qt::Key_PageDown:
        break;
    }
}

/// file progress indicator, called with values 0..100
void CR3View::OnLoadFileProgress( int percent )
{

}

/// format progress, called with values 0..100
void CR3View::OnFormatProgress( int percent )
{

}

/// first page is loaded from file an can be formatted for preview
void CR3View::OnLoadFileFirstPagesReady()
{
#if 0 // disabled
    if ( !_data->_props->getBoolDef( PROP_PROGRESS_SHOW_FIRST_PAGE, 1 ) ) {
        CRLog::info( "OnLoadFileFirstPagesReady() - don't paint first page because " PROP_PROGRESS_SHOW_FIRST_PAGE " setting is 0" );
        return;
    }
    CRLog::info( "OnLoadFileFirstPagesReady() - painting first page" );
    _docview->setPageHeaderOverride(qt2cr(tr("Loading: please wait...")));
    //update();
    repaint();
    CRLog::info( "OnLoadFileFirstPagesReady() - painting done" );
    _docview->setPageHeaderOverride(lString16::empty_str);
    _docview->requestRender();
    // TODO: remove debug sleep
    //sleep(5);
#endif
}

// fau+
void CR3View::point_to_end(ldomXPointer& xp) {
    ldomNode * p = xp.getNode();
    if(p->isText()) {
        lString16 text = p->getText();
        xp.setOffset(text.length());
    };
    if(p->isElement()) {
        xp.setOffset(p->getChildCount());
    }
}

void CR3View::point_to_begin(ldomXPointer& xp) {
    ldomNode * p = xp.getNode();
    if(p->isText())
        xp.setOffset(0);
}

void CR3View::startSelection()
{
    clearSelection();
    _selecting = true;

    ldomXPointerEx middle = _docview->getCurrentPageMiddleParagraph();
    middle.prevVisibleText();
    _selStart = middle;
    point_to_begin(_selStart);
    _selEnd = middle;
    point_to_end(_selEnd);

    updateSelection();
}

void CR3View::updateSelection() {
    _docview->clearSelection();
    ldomXRange selected(_selStart, _selEnd, 1);
    _docview->selectRange(selected);
    _selected = true;
    update();
    _selRange = selected;
}

void CR3View::GoToPage(int pagenum) {
    _docview->goToPage(pagenum);
    update();
}
int CR3View::getPageCount() { return _docview->getPageCount(); }

bool CR3View::GetLastPathName(QString *lastpath) {
    LVPtrVector<CRFileHistRecord> & files = _docview->getHistory()->getRecords();
    if (files.length()>0) {
        *lastpath = cr2qt(files[0]->getFilePathName());
        return true;
    }
    return false;
}

void CR3View::Rotate(int param)
{
    _docview->doCommand(DCMD_ROTATE_BY, param);
    refreshPropFromView(PROP_ROTATE_ANGLE);
    update();
}

void CR3View::ChangeFont(int param)
{
    QStringList faceList;
    crGetFontFaceList(faceList);

    QString fontname;
    if(param) {
        if(param<1 || param>faceList.count()) return;
        fontname = faceList.at(param-1);
    } else {
        int index = faceList.indexOf(cr2qt(_data->_props->getStringDef(PROP_FONT_FACE, "Verdana")));
        if(index+2>faceList.count()) index= 0;
        else index+=1;
        fontname = faceList.at(index);
    }
    PropsRef props = Props::create();
    props->setString(PROP_FONT_FACE, fontname);
    setOptions(props);
}

void CR3View::ChangeHeaderFont(int param)
{
    QStringList faceList;
    crGetFontFaceList(faceList);

    QString fontname;
    if(param) {
        if(param<1 || param>faceList.count()) return;
        fontname = faceList.at(param-1);
    } else {
        int index = faceList.indexOf(cr2qt(_data->_props->getStringDef(PROP_STATUS_FONT_FACE, "Verdana")));
        if(index+2>faceList.count()) index= 0;
        else index+=1;
        fontname = faceList.at(index);
    }
    PropsRef props = Props::create();
    props->setString(PROP_STATUS_FONT_FACE, fontname);
    setOptions(props);
}

#define MIN_FONT_GAMMA	0.3
#define MAX_FONT_GAMMA	1.91
void CR3View::ChangeFontGamma(int param)
{
    QString fontgamma = cr2qt(_data->_props->getStringDef(PROP_FONT_GAMMA, "1"));
    double value = fontgamma.toDouble();
    if(param>0) {
        value+=0.1;
        if(value>MAX_FONT_GAMMA) return;
    } else {
        value-=0.1;
        if(value<MIN_FONT_GAMMA) return;
    }
    PropsRef props = Props::create();
    props->setDouble(PROP_FONT_GAMMA, value);
    setOptions(props);
}

#define MIN_INTERLINE_SPACE	70
#define MAX_INTERLINE_SPACE	150

void CR3View::ChangeInterlineSpace(int param)
{
    int prevValue = _data->_props->getIntDef(PROP_INTERLINE_SPACE, 100);
    if(param>0) {
        param=prevValue+5;
        if(param>MAX_INTERLINE_SPACE) return;
    } else {
        param=prevValue-5;
        if(param<MIN_INTERLINE_SPACE) return;
    }
    int newValue = GetArrayNextValue(cr_interline_spaces, sizeof(cr_interline_spaces)/sizeof(int), prevValue, param);

    PropsRef props = Props::create();
    props->setDouble(PROP_INTERLINE_SPACE, newValue);
    setOptions(props);
}

//bool CR3View::kbIvent(QWSKeyEvent *pke)
//{
//	// word selection
//	if((pke->simpleData.is_press) && isWordSelection()) {
//		MoveDirection dir = DIR_ANY;

//		switch(pke->simpleData.keycode)
//		{
//			case Qt::Key_PageUp:
//			case Qt::Key_PageDown:
//			case Qt::Key_Return:
//				return true;
//			case Qt::Key_Escape:
//			{
//				QString text = endWordSelection();

//				qDebug("selected word is: %s", LCSTR(qt2cr(text)));
//				return true;
//			}
//			case Qt::Key_Left:
//				dir = DIR_LEFT;
//				break;
//			case Qt::Key_Right:
//				dir = DIR_RIGHT;
//				break;
//			case Qt::Key_Up:
//				dir = DIR_UP;
//				break;
//			case Qt::Key_Down:
//				dir = DIR_DOWN;
//				break;
//			default:
////				if ((pke->simpleData.keycode) >= Qt::Key_A && (pke->simpleData.keycode) <= Qt::Key_Z ) {
////					QString text = event->text();
////					pke->simpleData.keycode
////					if ( text.length()==1 ) {
////						_wordSelector->appendPattern(qt2cr(text));
////						update();
////						return true;
////					}
////				}
//				return false;
//		}
////		int dist = event->modifiers() & Qt::ShiftModifier ? 5 : 1;

//		int dist = 1;
//		_wordSelector->moveBy(dir, dist);
//		update();
//		return true;
//	}
//	// cite selection?
//	if (pke->simpleData.is_press) {
//		switch(pke->simpleData.keycode)
//		{
////			case Qt::Key_PageUp:
////			case Qt::Key_PageDown:
////			case Qt::Key_Return:
////				return true;
//		}
//	} else {
//		switch (pke->simpleData.keycode)
//		{
//			case Qt::Key_Left:
//			case Qt::Key_Right:
//			case Qt::Key_Up:
//			case Qt::Key_Down:
//				return true;
//		}
//	}
//	qDebug("cr3view key event!");
//	return false;
//}

int CR3View::GetArrayNextValue(int *array, int arraySize, int prevValue, int newValue)
{
    if(prevValue<newValue) {
        for(int i=0; i<arraySize; i++) {
            if(array[i]>=newValue) {
                newValue=array[i];
                break;
            }
        }
    } else {
        for(int i=arraySize-1; i>0; i--) {
            if(array[i]<=newValue) {
                newValue=array[i];
                break;
            }
        };
    }
    return newValue;
}

