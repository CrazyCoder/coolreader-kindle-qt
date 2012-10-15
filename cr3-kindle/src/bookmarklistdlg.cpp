#include "bookmarklistdlg.h"
#include "ui_bookmarklistdlg.h"

#include <QShortcut>

#define MAX_ITEM_LEN 50

static QString limit(QString s)
{
    if(s.length() < MAX_ITEM_LEN) return s;
    s = s.left( MAX_ITEM_LEN );
    return s + "...";
}

BookmarkListDialog::BookmarkListDialog(QWidget *parent, CR3View * docView ) :
    QDialog(parent),
    m_ui(new Ui::BookmarkListDialog),
    _docview(docView)
{
    m_ui->setupUi(this);

    QAction *actionRemoveBookmark = m_ui->actionRemoveBookmark;
    QShortcut* kbd = new QShortcut(Qt::Key_AltGr, this); // quick hack to delete bookmarks on K4NT with KBD key
    connect(kbd, SIGNAL(activated()), actionRemoveBookmark, SLOT(trigger()));
    addAction(actionRemoveBookmark);

    addAction(m_ui->actionRemoveAllBookmarks);
    QAction *actionSelect = m_ui->actionSelectBookmark;
    actionSelect->setShortcut(Qt::Key_Select);
    addAction(actionSelect);

    int i = 0;
    m_ui->tableWidget->horizontalHeader()->setResizeMode( i++, QHeaderView::ResizeToContents);
    m_ui->tableWidget->horizontalHeader()->setResizeMode( i++, QHeaderView::Stretch );
    m_ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    m_ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    CRFileHistRecord * rec = _docview->getDocView()->getCurrentFileHistRecord();
    if (!rec) return;
    LVPtrVector<CRBookmark> & list( rec->getBookmarks() );
    int curpercent = _docview->getDocView()->getPosPercent();
    int bestdiff = -1;
    int bestindex = -1;
    int y = 0;
    for ( int i=0; i<list.length(); i++ ) {
        CRBookmark * bm = list[i];
        if ( bm->getType() == bmkt_lastpos ) continue;
        int diff = bm->getPercent() - curpercent;
        if ( diff < 0 )
            diff = -diff;
        if ( bestindex==-1 || diff < bestdiff ) {
            bestindex = i;
            bestdiff = diff;
        }
        m_ui->tableWidget->setRowCount(y+1);
        {
            int i=0;
            _list.append( bm );
            m_ui->tableWidget->setItem( y, i++, new QTableWidgetItem(crpercent( bm->getPercent())));
            m_ui->tableWidget->setItem( y, i++, new QTableWidgetItem(limit(cr2qt(bm->getPosText()))));
            m_ui->tableWidget->verticalHeader()->setResizeMode(y, QHeaderView::ResizeToContents);
        }
        y++;
    }
    if (bestindex>=0) m_ui->tableWidget->selectRow(bestindex);

    m_ui->tableWidget->resizeColumnsToContents();
    m_ui->tableWidget->resizeRowsToContents();
}

BookmarkListDialog::~BookmarkListDialog()
{
    delete m_ui;
}

bool BookmarkListDialog::showDlg( QWidget * parent, CR3View * docView )
{
    BookmarkListDialog *dlg = new BookmarkListDialog(parent, docView);
    dlg->showMaximized();
    return true;
}

void BookmarkListDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

CRBookmark * BookmarkListDialog::selectedBookmark()
{
    int index = m_ui->tableWidget->currentRow();
    if ( index<0 || index>=_list.length() || index>=m_ui->tableWidget->rowCount() )
        return NULL;
    return _list[index];
}

void BookmarkListDialog::on_actionRemoveBookmark_triggered()
{
    CRBookmark * bm = selectedBookmark();
    if ( bm ) {
        int index = m_ui->tableWidget->currentRow();
        m_ui->tableWidget->removeRow(index);
        _list.removeAt(index);
        _docview->getDocView()->removeBookmark(bm);
    }
}

void BookmarkListDialog::on_actionRemoveAllBookmarks_triggered()
{
    int rowcount = m_ui->tableWidget->rowCount();
    while(rowcount) {
        CRBookmark * bm = _list[rowcount-1];
        _docview->getDocView()->removeBookmark(bm);
        m_ui->tableWidget->removeRow(rowcount-1);
        --rowcount;
    }
    _list.clear();
}

void BookmarkListDialog::on_actionSelectBookmark_triggered()
{
    CRBookmark * bm = selectedBookmark();
    if(bm) {
        _docview->goToBookmark(bm);
        close();
    }
}
