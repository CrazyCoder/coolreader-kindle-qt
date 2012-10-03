#ifndef CR3WIDGET_H
#define CR3WIDGET_H

#include <qwidget.h>
#include <QScrollBar>
#include "crqtutil.h"

#define MIN_CR_FONT_SIZE			14
#define MAX_CR_FONT_SIZE			72
#define MIN_CR_HEADER_FONT_SIZE		8
#define MAX_CR_HEADER_FONT_SIZE		28

#define DEF_HEADER_FONT_SIZE	22
#define DEF_FONT_SIZE			32

static int cr_interline_spaces[] = { 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150 };
static int def_margin[] = { 0, 1, 2, 3, 4, 5, 8, 10, 12, 14, 15, 16, 20, 25, 30 };

#define PROP_WINDOW_ROW_COUNT		"window.row.count"

class LVDocView;
class LVTocItem;
class CRBookmark;

class PropsChangeCallback {
public:
	virtual void onPropsChange( PropsRef props ) = 0;
	virtual ~PropsChangeCallback() { }
};

#define WORD_SELECTOR_ENABLED 1

class CR3View : public QWidget, public LVDocViewCallback
{

		Q_OBJECT

		Q_PROPERTY( QScrollBar* scrollBar READ scrollBar WRITE setScrollBar )

		class DocViewData;

#if WORD_SELECTOR_ENABLED==1
		LVPageWordSelector * _wordSelector;

// fau+
//	protected:
	public:
// fau-
		void startWordSelection();
		QString endWordSelection();
		bool isWordSelection() { return _wordSelector!=NULL; }
		int rowCount;
#endif

	public:
		CR3View( QWidget *parent = 0 );
		virtual ~CR3View();

		bool loadDocument( QString fileName );
		bool loadLastDocument();
		void setDocumentText( QString text );

		QScrollBar * scrollBar() const;

		/// get document's table of contents
		LVTocItem * getToc();
		/// return LVDocView associated with widget
		LVDocView * getDocView() { return _docview; }
		/// go to position specified by xPointer string
		void goToXPointer(QString xPointer);

		/// returns current page
		int getCurPage();

		/// load settings from file
		bool loadSettings( QString filename );
		/// save settings from file
		bool saveSettings( QString filename );
		/// load history from file
		bool loadHistory( QString filename );
		/// save history to file
		bool saveHistory( QString filename );

		void setHyphDir( QString dirname );
		const QStringList & getHyphDicts();

		/// load fb2.css file
		bool loadCSS( QString filename );
		/// set bookmarks dir
		void setBookmarksDir( QString dirname );
		/// set new option values
		PropsRef setOptions( PropsRef props );
		/// get current option values
		PropsRef getOptions();
		/// turns on/off Edit mode (forces Scroll view)
		void setEditMode( bool flgEdit );
		/// returns true if edit mode is active
		bool getEditMode() { return _editMode; }

//		void saveWindowPos( QWidget * window, const char * prefix );
//		void restoreWindowPos( QWidget * window, const char * prefix, bool allowExtraStates = false );

		void setPropsChangeCallback ( PropsChangeCallback * propsCallback )
		{
			_propsCallback = propsCallback;
		}
		/// toggle boolean property
		void toggleProperty( const char * name );
		/// returns true if point is inside selected text
		bool isPointInsideSelection( QPoint pt );
		/// returns selection text
		QString getSelectionText() { return _selText; }
		/// create bookmark
		CRBookmark * createBookmark();
		/// go to bookmark and highlight it
		void goToBookmark( CRBookmark * bm );

		/// rotate view, +1 = 90` clockwise, -1 = 90` counterclockwise
//		void rotate( int angle );
		/// Override to handle external links
		virtual void OnExternalLink( lString16 url, ldomNode * node );
		/// format detection finished
		virtual void OnLoadFileFormatDetected( doc_format_t fileFormat );
		/// on starting file loading
		virtual void OnLoadFileStart( lString16 filename );
		/// first page is loaded from file an can be formatted for preview
		virtual void OnLoadFileFirstPagesReady();
		/// file load finiished with error
		virtual void OnLoadFileError( lString16 message );
		/// file loading is finished successfully - drawCoveTo() may be called there
		virtual void OnLoadFileEnd();
		/// document formatting started
		virtual void OnFormatStart();
		/// document formatting finished
		virtual void OnFormatEnd();
		/// file progress indicator, called with values 0..100
		virtual void OnLoadFileProgress( int percent );
		/// format progress, called with values 0..100
		virtual void OnFormatProgress( int percent );

		// fau+
		virtual void doCommand( int cmd, int param = 0);
		void GoToPage(int pagenum);
		int getPageCount();
		void startSelection();
		void updateSelection();
		void point_to_end(ldomXPointer& xp);
		void point_to_begin(ldomXPointer& xp);
		bool GetLastPathName(QString *lastpath);
		void Rotate(int param);
		void ChangeFont(int param);
		void ChangeHeaderFont(int param);
		void ChangeFontGamma(int param);
		void ChangeInterlineSpace(int param);
		void propsApply(PropsRef props);
		int GetArrayNextValue(int *array, int arraySize, int prevValue, int newValue);
	public slots:
		void contextMenu( QPoint pos );
		void setScrollBar( QScrollBar * scroll );
		/// on scroll
		void togglePageScrollView();
		void scrollTo( int value );
//		void historyBack();
//		void historyForward();
		void zoomFont(int param);
		void zoomHeaderFont(int param);
	signals:
		//void fileNameChanged( const QString & );
	protected:
		virtual void keyPressEvent(QKeyEvent * event);
		virtual void paintEvent(QPaintEvent * event);
		virtual void resizeEvent(QResizeEvent * event);
//		virtual void wheelEvent ( QWheelEvent * event );
//		virtual void updateScroll();
//		virtual void doCommand( int cmd, int param = 0 );
//		virtual void mouseMoveEvent ( QMouseEvent * event );
//		virtual void mousePressEvent ( QMouseEvent * event );
//		virtual void mouseReleaseEvent ( QMouseEvent * event );
		virtual void refreshPropFromView( const char * propName );

	private slots:

	private:
		void updateDefProps();
		void clearSelection();
		void startSelection( ldomXPointer p );
		bool endSelection( ldomXPointer p );
		bool updateSelection( ldomXPointer p );

		DocViewData * _data; // to hide non-qt implementation
		LVDocView * _docview;
		QScrollBar * _scroll;
		PropsChangeCallback * _propsCallback;
		QStringList _hyphDicts;
		QCursor _normalCursor;
		QCursor _linkCursor;
		QCursor _selCursor;
		QCursor _waitCursor;
		bool _selecting;
		bool _selected;
		ldomXPointer _selStart;
		ldomXPointer _selEnd;
		QString _selText;
		ldomXRange _selRange;
		QString _cssDir;
		QString _bookmarkDir;
		bool _editMode;
		int _lastBatteryState;
};

#endif // CR3WIDGET_H
