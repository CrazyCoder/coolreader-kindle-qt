#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>

#include <QApplication>
#include <QMainWindow>
#include <QtDBus/QDBusConnection>
#include <QProcess>
#include <QDebug>
#include <QDecorationDefault>
#include <QTranslator>
#include <QTextCodec>
#include <QClipboard>

#include "cr3widget.h"
#include "crqtutil.h"
#include "crgui.h"

#include "crengine.h"
#include "lvstring.h"
#include "lvtinydom.h"

#include "settings.h"
#include "searchdlg.h"
#include "openfiledlg.h"
#include "tocdlg.h"
#include "recentdlg.h"
#include "bookmarklistdlg.h"
#include "filepropsdlg.h"

#define CMD_REFRESH					1001
#define CMD_ZOOM_FONT				1002
#define CMD_ZOOM_HEADER_FONT		1003
#define CMD_TOGGLE_HEADER			1004
#define CMD_JUMP_FROM_PAGE			1005
#define CMD_CHANGE_FONT				1006
#define CMD_CHANGE_HEADER_FONT		1007
#define CMD_CHANGE_FONT_GAMMA		1008
#define CMD_INTERLINE_SPACE			1009

// prototypes
void InitCREngineLog(CRPropRef props);
bool InitCREngine(const char * exename, lString16Collection & fontDirs);
void ShutdownCREngine();
bool getDirectoryFonts( lString16Collection & pathList, lString16 ext, lString16Collection & fonts, bool absPath );
bool myEventFilter(void *message, long *result);
#ifndef i386
void PrintString(int x, int y, const QString message, const QString opt = "");
#endif

class MyDecoration : public QDecorationDefault
{
private:
    int titleHeight;
public:
    MyDecoration() : QDecorationDefault() { titleHeight = qApp->font().pointSize() + 20; }
    QRegion region(const QWidget *widget, const QRect &rect, int decorationRegion = All)
    {
        // all
        QRegion region;
        QRect r(rect.left(), rect.top() - titleHeight, rect.width(), rect.height() + titleHeight);
        region = r;
        region -= rect;

        return region;
    }
    bool paint(QPainter *painter, const QWidget *widget, int decorationRegion = All, DecorationState state = Normal)
    {
        if(decorationRegion == None) return false;

        const QRect titleRect = QDecoration::region(widget, Title).boundingRect();

        int titleWidth = titleRect.width();

        Qt::WindowFlags flags = widget->windowFlags();
        bool hasTitle = flags & Qt::WindowTitleHint;

        bool paintAll = (decorationRegion == int(All));
        bool handled = false;

        if ((paintAll || decorationRegion & Title && titleWidth > 0) && state == Normal && hasTitle) {
            painter->save();
            painter->fillRect(titleRect, QBrush(Qt::black));
            painter->setPen(QPen(Qt::white));
            QFont font = qApp->font();
            font.setBold(true);
            painter->setFont(font);
            painter->drawText(titleRect.x() + 4, titleRect.y(), titleRect.width(), titleRect.height(), Qt::AlignVCenter, windowTitleFor(widget));
            painter->restore();

            handled |= true;
        }
        return handled;
    }
};

class MyApplication : public QApplication {
    Q_OBJECT
public:
    MyApplication(int &argc, char **argv) : QApplication(argc, argv) {
        connectPowerDaemon();

        QPalette palette;
        palette.setColor(QPalette::Window, Qt::white);
        palette.setColor(QPalette::WindowText, Qt::black);
        palette.setColor(QPalette::Base, Qt::white);
        palette.setColor(QPalette::AlternateBase, Qt::white);
        palette.setColor(QPalette::Text, Qt::black);
        palette.setColor(QPalette::Button, Qt::white);
        palette.setColor(QPalette::ButtonText, Qt::black);
        palette.setColor(QPalette::Highlight, Qt::white);
        palette.setColor(QPalette::HighlightedText, Qt::black);
        setPalette(palette);

        setStyle("cleanlooks");
        qwsSetDecoration(new MyDecoration());

        setNavigationMode(Qt::NavigationModeKeypadTabOrder);
        setEventFilter(myEventFilter);
    }
    void connectPowerDaemon() {
#ifndef i386
        QDBusConnection::systemBus().connect(QString(), QString(), "com.lab126.powerd", "outOfScreenSaver", this, SLOT(quitscreensaver()));
        //		QDBusConnection::systemBus().connect(QString(), QString(), "com.lab126.powerd", "goingToScreenSaver", this, SLOT(screensaver()));
#endif
    }
    void disconnectPowerDaemon() {
#ifndef i386
        QDBusConnection::systemBus().disconnect(QString(), QString(), "com.lab126.powerd", "outOfScreenSaver", this, SLOT(quitscreensaver()));
        //		QDBusConnection::systemBus().disconnect(QString(), QString(), "com.lab126.powerd", "goingToScreenSaver", this, SLOT(MainWindow::screensaver()));
#endif
    }
private slots:
    void quitscreensaver() {
        sleep(3);
        if(!activeWindow()->isFullScreen() && !activeWindow()->isMaximized()) {
            QWidget *mainwnd = widgetAt(0,0);
            mainwnd->repaint();
        }
        activeWindow()->repaint();
    }
};

namespace Ui {
class MainWindowClass;
}

class MainWindow : public QMainWindow, public PropsChangeCallback
{
    Q_OBJECT

public:
    virtual void onPropsChange(PropsRef props);
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void doStartupActions();
private:
    Ui::MainWindowClass *ui;
    QString _filenameToOpen;
    CRGUIAcceleratorTableRef wndkeys;
    bool loadKeymaps(CRGUIWindowManager & winman, const char * locations[]);
    void doCommand(int cmd, int param = 0);
    bool isReservedKey(int key);
protected:
public slots:
    void contextMenu( QPoint pos );
    void on_actionFindText_triggered();
private slots:
    void on_actionFileProperties_triggered();
    void on_actionShowBookmarksList_triggered();
    void on_actionAddBookmark_triggered();
    void on_actionSettings_triggered();
    void on_actionRecentBooks_triggered();
    void on_actionTOC_triggered();
    void on_actionPrevPage_triggered();
    void on_actionNextPage_triggered();
    void on_actionClose_triggered();
    void on_actionOpen_triggered();
    void on_actionShowMenu_triggered();
    void on_actionEmpty_triggered();
private slots:
    void battLevelChanged(int fparam);
};

#endif // MAINWINDOW_H
