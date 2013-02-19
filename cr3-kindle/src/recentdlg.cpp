#include "recentdlg.h"
#include "ui_recentdlg.h"

#include <QDir>
#include <QKeyEvent>
#include <QShortcut>

RecentBooksDlg::RecentBooksDlg(QWidget *parent, CR3View * docView ) :
    QDialog(parent),
    m_ui(new Ui::RecentBooksDlg),
    m_docview(docView)
{
    m_ui->setupUi(this);

    m_ui->tableWidget->setItemDelegate(new RecentBooksListDelegate());

    QAction *actionRemoveBook = m_ui->actionRemoveBook;
    QShortcut* kbd = new QShortcut(Qt::Key_AltGr, this); // quick hack to delete recent books on K4NT with KBD key
    connect(kbd, SIGNAL(activated()), actionRemoveBook, SLOT(trigger()));
    addAction(actionRemoveBook);

    addAction(m_ui->actionRemoveAll);
    QAction *actionSelect = m_ui->actionSelectBook;
    actionSelect->setShortcut(Qt::Key_Select);
    addAction(actionSelect);
    addAction(m_ui->actionNextPage);
    addAction(m_ui->actionPrevPage);

    m_ui->tableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    m_ui->tableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);

    isCyclic = m_docview->getOptions()->getIntDef(PROP_CYCLIC_LIST_PAGES, 1) == 1;

    int rc = m_docview->rowCount*2;
    int h  = m_docview->height() -2 - (qApp->font().pointSize() + rc);
    m_ui->tableWidget->verticalHeader()->setResizeMode(QHeaderView::Custom);
    m_ui->tableWidget->verticalHeader()->setDefaultSectionSize(h/rc);
    h = h%rc;
    int t, b;
    if(h%2) {
        t = h/2;
        b = t + h%2;
    } else
        t = b = h/2;
    m_ui->verticalLayout->setContentsMargins(0, t, 0, b);
    // fill rows
    QFont fontBold = m_ui->tableWidget->font();
    fontBold.setBold(true);

    QFont fontItalic = m_ui->tableWidget->font();
    fontItalic.setItalic(true);

    QTableWidgetItem *str11Widget = new QTableWidgetItem();
    str11Widget->setFlags(Qt::NoItemFlags);
    str11Widget->setFont(fontBold);
    str11Widget->setData(Qt::UserRole, QVariant(2));

    QTableWidgetItem *str12Widget = new QTableWidgetItem();
    str12Widget->setFont(fontItalic);
    str12Widget->setData(Qt::UserRole, QVariant(3));

    QTableWidgetItem *str22Widget = new QTableWidgetItem();
    str22Widget->setFont(fontItalic);
    str22Widget->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    str22Widget->setData(Qt::UserRole, QVariant(4));

    rc = m_docview->rowCount;
    m_ui->tableWidget->setRowCount(rc*2);
    for(int i=0; i<rc*2; i+=2) {
        m_ui->tableWidget->setItem(i, 0, str11Widget->clone());
        m_ui->tableWidget->setItem(i+1, 0, str12Widget->clone());
        m_ui->tableWidget->setItem(i+1, 1, str22Widget->clone());

        m_ui->tableWidget->setSpan(i, 0, 1, 2);
    }

    m_ui->tableWidget->installEventFilter(this);

    titleMask = windowTitle();
    docView->getDocView()->savePosition(); // to move current file to top
    SetPageCount();
    curPage=0;

    ShowPage(1);
}

void RecentBooksDlg::SetPageCount()
{
    LVPtrVector<CRFileHistRecord> & files = m_docview->getDocView()->getHistory()->getRecords();
    int count = files.length() - (m_docview->getDocView()->isDocumentOpened() ? 1 : 0);
    int rc = m_docview->rowCount;
    pageCount = 1;
    if(count>rc) {
        pageCount = count/rc;
        if(count%rc) pageCount+=1;
    }
}

void RecentBooksDlg::ShowPage(int updown, int selectRow)
{
    Device::forceFullScreenUpdate();

    if (updown > 0) {
        if (curPage+1 > pageCount) curPage = isCyclic ? 0 : curPage - 1;
        curPage+=1;
    } else {
        if (curPage-1 <= 0) curPage = isCyclic ? pageCount+1 : 2;
        curPage-=1;
    }
    setWindowTitle(titleMask + " (" + QString::number(curPage) + "/" + QString::number(pageCount) + ")");

    int rc = m_docview->rowCount;
    int firstItem = m_docview->getDocView()->isDocumentOpened() ? 1 : 0;
    int startPos = ((curPage-1)*rc)+firstItem;
    LVPtrVector<CRFileHistRecord> & files = m_docview->getDocView()->getHistory()->getRecords();
    for(int k=startPos, index=0; index<rc*2; ++k, index+=2) {
        if(k<files.length()) {
            CRFileHistRecord * book = files.get(k);

            lString16 title = book->getTitle();
            lString16 author = book->getAuthor();
            lString16 series = book->getSeries();
            lString16 filename = book->getFileName();
            if(title.empty()) title = filename;
            QString fileExt = cr2qt(filename);
            fileExt = fileExt.mid(fileExt.lastIndexOf(".")+1);

            int fileSize = book->getFileSize();
            CRBookmark *bm = book->getLastPos();
            int percent = bm->getPercent();

            if(author.empty()) author = "-";
            if(title.empty()) title = "-";
            if(!series.empty()) series = "(" + series + ")";

            QTableWidgetItem *item = m_ui->tableWidget->item(index, 0);
            item->setText(cr2qt(title));

            item = m_ui->tableWidget->item(index+1, 0);
            item->setText(cr2qt(author)+"\n"+cr2qt(series));

            item = m_ui->tableWidget->item(index+1, 1);
            item->setText(crpercent(percent) + "\n" + fileExt+" / "+crFileSize(fileSize));

            m_ui->tableWidget->showRow(index);
            m_ui->tableWidget->showRow(index+1);
        } else {
            m_ui->tableWidget->hideRow(index);
            m_ui->tableWidget->hideRow(index+1);
        }
    }
    // select first row
    if(m_ui->tableWidget->rowCount()>0)
        m_ui->tableWidget->selectRow(selectRow);
}

RecentBooksDlg::~RecentBooksDlg()
{
    delete m_ui;
}

bool RecentBooksDlg::showDlg( QWidget * parent,  CR3View * docView )
{
    RecentBooksDlg *dlg = new RecentBooksDlg(parent, docView);
    dlg->showMaximized();
    return true;
}

void RecentBooksDlg::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool RecentBooksDlg::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        QString text;
        switch(keyEvent->key()) {
        case Qt::Key_Up:
            if(obj == m_ui->tableWidget) {

                QScrollBar * scrollBar = m_ui->tableWidget->verticalScrollBar();
                int pageStrCount = scrollBar->pageStep();
                int fullStrCount = scrollBar->maximum()-scrollBar->minimum()+pageStrCount;

                if(fullStrCount==pageStrCount) {
                    pageCount = 1;
                    //return;
                }
                if(pageStrCount==1) {
                    scrollBar->setMaximum(fullStrCount*2);
                    pageStrCount = scrollBar->pageStep();
                }
                pageCount = ceil((double)fullStrCount/pageStrCount);

                if(((m_ui->tableWidget->currentRow()+1)/2 == 1) && (pageStrCount/2>1)){
                }


            }
            break;
            return true;
        }
    }
    return false;
}

void RecentBooksDlg::openBook(int row)
{
    int firstItem = m_docview->getDocView()->isDocumentOpened() ? 1 : 0;
    row+=firstItem;
    LVPtrVector<CRFileHistRecord> & files = m_docview->getDocView()->getHistory()->getRecords();
    if(row>files.length()) return;
    // go to file
    m_docview->loadDocument(cr2qt(files[row-1]->getFilePathName()));
    close();
}

int RecentBooksDlg::getBookNum()
{
    int cr = m_ui->tableWidget->currentRow();
    if(cr<0) cr=0;

    if(cr==0)
        cr=1;
    else
        ++cr/=2;
    cr+=(curPage-1)*m_docview->rowCount;

    qDebug("%i", cr);

    return cr;
}

void RecentBooksDlg::on_actionSelectBook_triggered()
{
    openBook(getBookNum());
}

void RecentBooksDlg::removeFile(LVPtrVector<CRFileHistRecord> & files, int num)
{

    QDir Dir0( cr2qt(files[num-1]->getFilePath()) );
    // Удаляемый файл ещё существует
    if((Dir0.exists( cr2qt(files.get(num-1)->getFileName()) )) && (files.length() > 1)){
        // Нужно чтобы в истории было больше одной книжки, чтобы можно было загрузить удяляемую запись а потом удалить
        m_docview->loadDocument(cr2qt(files[num-1]->getFilePathName()));

        // remove cache file
        lUInt32 crc = m_docview->getDocView()->getDocProps()->getIntDef(DOC_PROP_FILE_CRC32, 0);
        QString cachePattern;
        cachePattern.sprintf("*.%08x*.cr3", crc);

        // Возвращаем активным первоначально просматриваемый документ (он сейчас первым в списке истории стоит)
        m_docview->loadDocument(cr2qt(files[1]->getFilePathName()));

        QDir Dir(qApp->applicationDirPath() + QDir::toNativeSeparators(QString("/data/cache/")));
        QStringList fileList = Dir.entryList(QStringList() << cachePattern, QDir::Files);
        if(fileList.count())
            Dir.remove(fileList.at(0));

        files.remove(1);
    } else {
        // Известно лишь название архива и если его название не совпадает с названием файла то кеш файл не будет удалён
        // remove cache file
        QString filename = cr2qt(files.get(num-1)->getFileName());
        // trim file extension, need for archive files
        int pos = filename.lastIndexOf(".");
        if(pos != -1) filename.resize(pos);
        QDir Dir(qApp->applicationDirPath() + QDir::toNativeSeparators(QString("/data/cache/")));
        QStringList fileList = Dir.entryList(QStringList() << filename + "*.cr3", QDir::Files);
        if(fileList.count())
            Dir.remove(fileList.at(0));
        files.remove(num-1);
    }
}

void RecentBooksDlg::on_actionRemoveAll_triggered()
{
    if(QMessageBox::question(this, tr("Remove all history items"), tr("Do you really want to remove all history records?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) == QMessageBox::Yes) {
        int firstItem = m_docview->getDocView()->isDocumentOpened() ? 1 : 0;
        LVPtrVector<CRFileHistRecord> & files = m_docview->getDocView()->getHistory()->getRecords();
        for(int r=files.length(); r>=firstItem; r--)
            removeFile(files, r);
        close();
    }
}

void RecentBooksDlg::on_actionRemoveBook_triggered()
{
    int cr = m_ui->tableWidget->currentRow();
    if(cr<0) cr=0;

    int firstItem = m_docview->getDocView()->isDocumentOpened() ? 1 : 0;
    LVPtrVector<CRFileHistRecord> & files = m_docview->getDocView()->getHistory()->getRecords();

    int row = getBookNum() + firstItem;
    if(row>files.length()) return;


    removeFile(files, row);

    SetPageCount();
    if(curPage>pageCount)
        curPage-=1;
    curPage-=1;


    // select row
    cr = (row-firstItem)*2-1;
    if((cr+firstItem)/2 >= files.length())
        cr = files.length()*2-3;
    if(cr<0) cr=1;

    ShowPage(1, cr);
}

void RecentBooksDlg::on_actionNextPage_triggered()
{
    ShowPage(1);
}

void RecentBooksDlg::on_actionPrevPage_triggered()
{
    ShowPage(-1);
}
