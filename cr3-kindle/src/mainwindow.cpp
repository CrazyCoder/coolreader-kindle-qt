#include "mainwindow.h"
#include "ui_mainwindow.h"

#define DOC_CACHE_SIZE 128 * 0x100000
#define ENABLE_BOOKMARKS_DIR 1

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    usbDriveMode = false;
    screenSaverMode = false;
    menuActive = false;

    ui->setupUi(this);

    addAction(ui->actionOpen);
    addAction(ui->actionRecentBooks);
    addAction(ui->actionTOC);
    addAction(ui->actionSettings);
    addAction(ui->actionAddBookmark);
    addAction(ui->actionShowBookmarksList);
    addAction(ui->actionFindText);
    addAction(ui->actionFileProperties);
    // addAction(ui->actionCopy);

    QAction *actionShowMenu = ui->actionShowMenu;
#ifdef i386
    actionShowMenu->setShortcut(Qt::Key_M);
#else
    actionShowMenu->setShortcut(Qt::Key_Menu);
#endif
    addAction(actionShowMenu);

    QAction *actionHide = ui->actionHide;
    actionHide->setText(tr("Minimize") + "\tAlt+Shift");
    addAction(actionHide);

    QAction *actionClose = ui->actionClose;
    actionClose->setShortcut(Qt::ALT + Qt::Key_Escape);
    actionClose->setText(tr("Close") + "\tAlt+Back");
    addAction(actionClose);

    if (Device::hasLight()) {
        QAction *actionAdjustBrightness = ui->actionAdjustBrightness;
        actionAdjustBrightness->setShortcut(Qt::Key_BrightnessAdjust);
        addAction(actionAdjustBrightness);
        brDlg = new BrightnessControl(this);
    }

    QString dataDir = qApp->applicationDirPath() + QDir::toNativeSeparators(QString("/data/"));

    QString cacheDir = dataDir + "cache";
    QString bookmarksDir = dataDir + "bookmarks";
    QString histFile = dataDir + "cr3hist.bmk";
    QString iniFile = dataDir + "cr3.ini";
    QString cssFile = dataDir + "fb2.css";
    QString hyphDir = dataDir + "hyph" + QDir::separator();
    ldomDocCache::init(qt2cr(cacheDir), DOC_CACHE_SIZE);
    ui->view->setPropsChangeCallback(this);

    ui->view->loadCSS(cssFile);
    ui->view->setHyphDir(hyphDir);
    ui->view->loadSettings(iniFile);
    ui->view->loadHistory(histFile);
#if ENABLE_BOOKMARKS_DIR==1
    ui->view->setBookmarksDir(bookmarksDir);
#endif

    QString def = dataDir + "keydefs.ini";
    QString map = dataDir + "keymaps.ini";
    CRGUIAcceleratorTableList tables;
    if(tables.openFromFile(def.toAscii(), map.toAscii())) {
        wndkeys = tables.get(lString16("mainwindow"));
        if(wndkeys->length()>0) {
            for(int i=0; i<wndkeys->length(); i++) {
                const CRGUIAccelerator * acc = wndkeys->get(i);
                int cmd = acc->commandId;
                int param = acc->commandParam;
                int key = acc->keyCode;
                int keyFlags = acc->keyFlags;

                if(isReservedKey(key + keyFlags)) continue;

                QString cmdstr;
                if(cmd) cmdstr = QString::number(cmd);
                if(param) cmdstr+= "|" + QString::number(param);
                if(!cmdstr.isEmpty()) {
                    QAction *newAct = new QAction(this);
                    newAct->setShortcut(key + keyFlags);
                    newAct->setText(cmdstr);
                    connect(newAct, SIGNAL(triggered()), this, SLOT(on_actionEmpty_triggered()));
                    addAction(newAct);
                }
                // qDebug("cmd %i param %i key %i keyflag %i", cmd, param, key, keyFlags);
            }
            const CRGUIAccelerator *acc1 = wndkeys->findKeyAccelerator(Qt::Key_PageDown, 0);
            const CRGUIAccelerator *acc2 = wndkeys->findKeyAccelerator(Qt::Key_PageUp, 0);
            if(!acc1 || !acc2) {
                addAction(ui->actionNextPage);
                addAction(ui->actionPrevPage);
            }
        }
    } else {
        addAction(ui->actionNextPage);
        addAction(ui->actionPrevPage);
    }
    ui->view->getDocView()->setBatteryState(100);
#ifndef i386
    connectSystemBus();

    QStringList list;
    QProcess *myProcess = new QProcess();

    list << "com.lab126.powerd" << "battLevel";
    myProcess->start("/usr/bin/lipc-get-prop", list);
    if(myProcess->waitForReadyRead(1000)) {
        QByteArray array = myProcess->readAll();
        array.truncate(array.indexOf("\n"));
        ui->view->getDocView()->setBatteryState(array.toInt());
    }
#endif
}

MainWindow::~MainWindow()
{
#ifndef i386
    disconnectSystemBus();
#endif
    delete ui;
}

void MainWindow::connectSystemBus() {
#ifndef i386
    qDebug("+++ connect system bus");
    QDBusConnection::systemBus().connect(QString(), QString(), "com.lab126.powerd", "battLevelChanged", this, SLOT(battLevelChanged(int)));
    QDBusConnection::systemBus().connect(QString(), QString(), "com.lab126.powerd", "goingToScreenSaver", this, SLOT(goingToScreenSaver()));
    QDBusConnection::systemBus().connect(QString(), QString(), "com.lab126.powerd", "outOfScreenSaver", this, SLOT(outOfScreenSaver()));
    QDBusConnection::systemBus().connect(QString(), QString(), "com.lab126.hal", "usbConfigured", this, SLOT(usbDriveConnected()));
    QDBusConnection::systemBus().connect(QString(), QString(), "com.lab126.hal", "usbUnconfigured",this, SLOT(usbDriveDisconnected()));

    // system bus is connected on startup and when restoring minimized app, good place to fix the light level
    if (Device::hasLight() && brDlg) {
        brDlg->fixZeroLevel();
    }
#endif
}
void MainWindow::disconnectSystemBus() {
#ifndef i386
    qDebug("--- disconnect system bus");
    QDBusConnection::systemBus().disconnect(QString(), QString(), "com.lab126.powerd", "battLevelChanged", this, SLOT(battLevelChanged(int)));
    QDBusConnection::systemBus().disconnect(QString(), QString(), "com.lab126.powerd", "goingToScreenSaver", this, SLOT(goingToScreenSaver()));
    QDBusConnection::systemBus().disconnect(QString(), QString(), "com.lab126.powerd", "outOfScreenSaver", this, SLOT(outOfScreenSaver()));
    QDBusConnection::systemBus().disconnect(QString(), QString(), "com.lab126.hal", "usbConfigured", this, SLOT(usbDriveConnected()));
    QDBusConnection::systemBus().disconnect(QString(), QString(), "com.lab126.hal", "usbUnconfigured", this, SLOT(usbDriveDisconnected()));
#endif
}

void MainWindow::battLevelChanged(int fparam)
{
#ifndef i386
    ui->view->getDocView()->setBatteryState(fparam);
#endif
}

void MainWindow::goingToScreenSaver()
{
#ifndef i386
    if (!usbDriveMode && !screenSaverMode) {
        qDebug("screensaver on");
        Device::resumeFramework();
    }
    screenSaverMode = true;
#endif
}

void MainWindow::outOfScreenSaver()
{
#ifndef i386
    if (screenSaverMode && !usbDriveMode) {
        qDebug("screensaver off");
        sleep(1);
        Device::suspendFramework();
        QWSServer::instance()->refresh();
        if (Device::hasLight()) {
            brDlg->fixZeroLevel();
        }
    }
    screenSaverMode = false;
#endif
}

void MainWindow::usbDriveConnected()
{
#ifndef i386
    screenSaverMode = false; // screensaver is disabled automatically when USB is connected
    if (!usbDriveMode) {
        qDebug("usb drive on");
        QWSServer::instance()->closeKeyboard();
        sleep(1);
        Device::resumeFramework();
    }
    usbDriveMode = true;
#endif
}

void MainWindow::usbDriveDisconnected()
{
#ifndef i386
    if (!screenSaverMode && usbDriveMode) {
        qDebug("usb drive off");
        sleep(1);
        Device::suspendFramework();
        QWSServer::instance()->openKeyboard();
        QWSServer::instance()->refresh();
    }
    usbDriveMode = false;
#endif
}

void MainWindow::on_actionOpen_triggered()
{
    OpenFileDlg::showDlg(this, ui->view);
}

void MainWindow::on_actionClose_triggered()
{
    QWSServer::instance()->enablePainting(false);
    close();
}

void MainWindow::on_actionNextPage_triggered()
{
    doCommand(DCMD_PAGEDOWN);
}

void MainWindow::on_actionPrevPage_triggered()
{
    doCommand(DCMD_PAGEUP);
}

void MainWindow::on_actionTOC_triggered()
{
    TocDlg::showDlg(this, ui->view);
}

void MainWindow::on_actionRecentBooks_triggered()
{
    RecentBooksDlg::showDlg(this, ui->view );
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDlg::showDlg(this, ui->view);
}

void MainWindow::onPropsChange(PropsRef props)
{
}

void MainWindow::contextMenu( QPoint pos )
{
    QMenu *menu = new QMenu;

    menu->addAction(ui->actionOpen);
    menu->addAction(ui->actionRecentBooks);
    menu->addAction(ui->actionTOC);
    menu->addAction(ui->actionFindText);
    menu->addAction(ui->actionSettings);
    menu->addAction(ui->actionFileProperties);
    menu->addAction(ui->actionShowBookmarksList);
    menu->addAction(ui->actionAddBookmark);
    menu->addAction(ui->actionHide);
    menu->addAction(ui->actionClose);
    menuActive = true;
    menu->exec(ui->view->mapToGlobal(pos));
    menuActive = false;
}

void MainWindow::doStartupActions()
{
    switch(ui->view->getOptions()->getIntDef(PROP_APP_START_ACTION, 0)) {
    case 0:
        // open recent book
        ui->view->loadLastDocument();
        break;
    case 1:
        // show recent books dialog
        RecentBooksDlg::showDlg(this, ui->view);
        break;
    case 2:
        // show file open dialog
        OpenFileDlg::showDlg(this, ui->view);
    }
}

void MainWindow::on_actionAddBookmark_triggered()
{
    ui->view->createBookmark();
}

void MainWindow::on_actionShowBookmarksList_triggered()
{
    BookmarkListDialog::showDlg(this, ui->view);
}

void MainWindow::on_actionFileProperties_triggered()
{
    FilePropsDialog::showDlg(this, ui->view);
}

void MainWindow::on_actionFindText_triggered()
{
    SearchDialog::showDlg(this, ui->view);
}

void MainWindow::on_actionShowMenu_triggered()
{
    contextMenu(QPoint(5,5));
}

void MainWindow::on_actionEmpty_triggered()
{
    QAction *a = qobject_cast<QAction *>(sender());
    if(a) {
        QString cmdstr = a->text();
        int cmd = 0;
        int param = 0;
        int pos = cmdstr.indexOf("|");
        if(pos!=-1) {
            param = cmdstr.mid(pos+1).toInt();
            cmd = cmdstr.mid(0, pos).toInt();
        } else cmd = cmdstr.toInt();
        doCommand(cmd, param);
    }
}

bool MainWindow::isReservedKey(int key)
{
    switch(key) {
    case Qt::Key_Menu:
    case Qt::Key_O:
    case Qt::Key_L:
    case Qt::Key_C:
    case Qt::Key_F:
    case Qt::Key_S:
    case Qt::Key_I:
    case Qt::Key_B:
    case Qt::SHIFT + Qt::Key_B:
    case Qt::ALT + Qt::Key_Escape:
    case Qt::ALT + Qt::SHIFT:
        return true;
    default:
        return false;
    }
}

bool MainWindow::isFocusInReader()
{
    return hasFocus() && !menuActive;
}

void MainWindow::doCommand(int cmd, int param)
{
    switch(cmd) {
    case CMD_TOGGLE_HEADER:
        ui->view->toggleProperty(PROP_STATUS_LINE);
        break;
    case CMD_REFRESH:
        ui->view->repaint();
        break;
    case CMD_ZOOM_FONT:
        ui->view->zoomFont(param);
        break;
    case CMD_ZOOM_HEADER_FONT:
        ui->view->zoomHeaderFont(param);
        break;
    case CMD_JUMP_FROM_PAGE: {
        int pagenum = ui->view->getCurPage() + param;
        ui->view->doCommand(DCMD_GO_PAGE, pagenum);
        ui->view->update();
        break;
    }
    case CMD_CHANGE_FONT_GAMMA:
        ui->view->ChangeFontGamma(param);
        break;
    case CMD_CHANGE_FONT:
        ui->view->ChangeFont(param);
        break;
    case CMD_CHANGE_HEADER_FONT:
        ui->view->ChangeHeaderFont(param);
        break;
    case CMD_INTERLINE_SPACE:
        ui->view->ChangeInterlineSpace(param);
        break;
    case DCMD_ROTATE_BY:
        ui->view->Rotate(param);
        break;
    case DCMD_LINK_BACK:
    case DCMD_LINK_FORWARD:
        ui->view->doCommand(cmd, 1);
        break;
    case DCMD_PAGEDOWN:
    case DCMD_PAGEUP:
    case DCMD_BEGIN:
    case DCMD_END:
    case DCMD_MOVE_BY_CHAPTER:
    case DCMD_TOGGLE_BOLD:
        ui->view->doCommand(cmd, param);
    }
}

// small hack to make minimize work on both keyboard and K4NT devices
// - emulate Alt+Shift on KDX/K3 (the rest is handled by keyboard driver)
// - on K4NT we need to disconnect system bus, close keyboard, resume cvm and emulate HOME button press
void MainWindow::on_actionHide_triggered()
{
#ifndef i386
    if (Device::hasKeyboard()) {
        system("echo sendalt 42>/proc/keypad"); // Alt+Shift works only on keyboard devices
    } else {
        gotoSleep();
        if (!Device::isTouch()) {
            QWSServer::instance()->closeKeyboard();
            Device::resumeFramework();
            system("echo send 102 >/proc/keypad");
        } else {
            QWSServer::instance()->closeMouse();
            Device::resumeFramework();
            system("lipc-set-prop com.lab126.appmgrd start app://com.lab126.booklet.home");
        }
    }
#endif
}

#ifndef i386
#include "touchscreen.h"
extern TouchScreen * pTouch;
#endif

void MainWindow::on_actionAdjustBrightness_triggered()
{
#ifndef i386
    qDebug("brightness control");
    pTouch->enableGesture(false);
    menuActive = true;
    brDlg->exec();
    menuActive = false;
    pTouch->enableGesture(true);
#endif
}
