#include "tocdlg.h"
#include "ui_tocdlg.h"
#include <QKeyEvent>
#include <math.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"


class TocItem : public QTreeWidgetItem
{
    LVTocItem * _item;
public:
    LVTocItem * getItem() { return _item; }
    TocItem(LVTocItem * item, int currPage, int & nearestPage, TocItem * & nearestItem)
        : QTreeWidgetItem(QStringList() << (item ? cr2qt(item->getName()) : "No TOC items")
                                        << (item ? cr2qt(lString16::itoa(item->getPage()+1)) : "")) , _item(item)
    {
        setTextAlignment(1, Qt::AlignRight|Qt::AlignVCenter);
        int page = item->getPage();
        if (!nearestItem || (page <= currPage && page > nearestPage)) {
            nearestItem = this;
            nearestPage = page;
        }
        setData(0, Qt::UserRole, QVariant(cr2qt(item->getXPointer().toString())));
        for (int i = 0; i < item->getChildCount(); i++) {
            addChild(new TocItem(item->getChild(i), currPage, nearestPage, nearestItem));
        }
    }
};

bool TocDlg::showDlg(QWidget * parent, CR3View * docView)
{
    TocDlg * dlg = new TocDlg(parent, docView);
    dlg->showMaximized();
    return true;
}

TocDlg::TocDlg(QWidget *parent, CR3View * docView) :
    QDialog(parent),
    m_ui(new Ui::TocDlg), m_docview(docView)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    m_ui->setupUi(this);
    addAction(m_ui->actionNextPage);
    addAction(m_ui->actionPrevPage);
    addAction(m_ui->actionUpdatePage);

    QAction *actionSelect = m_ui->actionGotoPage;
    actionSelect->setShortcut(Qt::Key_Select);
    addAction(actionSelect);

    m_ui->treeWidget->setColumnCount(2);
    m_ui->treeWidget->header()->setResizeMode(0, QHeaderView::Stretch);
    m_ui->treeWidget->header()->setResizeMode(1, QHeaderView::ResizeToContents);

    int nearestPage = -1;
    int currPage = docView->getCurPage();
    TocItem * nearestItem = NULL;
    LVTocItem * root = m_docview->getToc();
    for (int i=0; i<root->getChildCount(); i++ )
        m_ui->treeWidget->addTopLevelItem(new TocItem(root->getChild(i), currPage, nearestPage, nearestItem));

    m_ui->treeWidget->expandAll();
    if(nearestItem)
        m_ui->treeWidget->setCurrentItem(nearestItem);

    m_ui->pageNumEdit->setValidator(new QIntValidator(1, 999999999, this));
    m_ui->pageNumEdit->installEventFilter(this);
    m_ui->treeWidget->installEventFilter(this);

    // code added 28.11.2011
    countItems = 0;
    countItemsTotal = getMaxItemPosFromBegin(m_docview->getToc(),m_ui->treeWidget->currentItem(),0);
    isPageUpdated = false;
}

TocDlg::~TocDlg()
{
    delete m_ui;
}

void TocDlg::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TocDlg::showEvent(QShowEvent *e)
{
    fillOpts();
    curPage = 1;

    // code added 28.11.2011
    isPageUpdated = false;
    setCurrentItemPage();

    titleMask = tr("Table of Contents");
    updateTitle();
}

void TocDlg::updateTitle()
{
    int pagenum = m_docview->getCurPage()+1;
    setWindowTitle(titleMask + " (" + QString::number(curPage) + "/" + QString::number(pageCount) + ") [" + QString::number(pagenum) + "]");
}

void TocDlg::on_actionGotoPage_triggered()
{
    int pagenum;
    if(m_ui->pageNumEdit->text().lastIndexOf('%')>0){
        int percentNum = m_ui->pageNumEdit->text().left(m_ui->pageNumEdit->text().length()-1).toInt();
        pagenum = ceil((double)m_docview->getPageCount()*percentNum/100);
        if(percentNum == 0) pagenum = 1;
    } else

        pagenum = m_ui->pageNumEdit->text().toInt();

    if(pagenum && pagenum <= m_docview->getPageCount()) {
        m_docview->getDocView()->savePosToNavigationHistory();
        m_docview->GoToPage(pagenum-1);
        close();
    } else {
        // code added 28.11.2011
        qDebug() << focusWidget()->objectName().toAscii().data();
        QString s = m_ui->treeWidget->currentIndex().data(Qt::UserRole).toString();
        m_docview->goToXPointer(s);
        close();
    }
}

bool TocDlg::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        QString text;
        switch(keyEvent->key()) {
        case Qt::Key_Q:
            text = "1";
            break;
        case Qt::Key_W:
            text = "2";
            break;
        case Qt::Key_E:
            text = "3";
            break;
        case Qt::Key_R:
            text = "4";
            break;
        case Qt::Key_T:
            text = "5";
            break;
        case Qt::Key_Y:
            text = "6";
            break;
        case Qt::Key_U:
            text = "7";
            break;
        case Qt::Key_I:
            text = "8";
            break;
        case Qt::Key_O:
            text = "9";
            break;
        case Qt::Key_P:
            text = "0";
            break;
        case 0x2e:
            text = "%";
            break;
        case 0x01001103:
            text = "%";
            break;
        case Qt::Key_Up:
        {
            if(isPageUpdated) {
                on_actionUpdatePage_triggered();
                isPageUpdated = false;
            }
            int cur_num = getCurrentItemPosFromBegin(m_docview->getToc(),m_ui->treeWidget->currentItem(), 0);
            if((cur_num<=((curPage-1)*pageStrCount)+1) && (curPage==pageCount) && (pageCount>2)){
                moveUpPage();
                moveDownPage();
            }
            moveUp();
        }
            break;
        case Qt::Key_Down:
            moveDown();
            break;
        case Qt::Key_Select:
            qDebug() << focusWidget()->objectName().toAscii().data();
        {

            QString s = m_ui->treeWidget->currentIndex().data(Qt::UserRole).toString();
            m_docview->goToXPointer(s);
            close();
            return true;
        }
            break;
        }

        if(keyEvent->key() >= Qt::Key_0 && keyEvent->key() <= Qt::Key_9) text = keyEvent->text();
        if(!text.isEmpty()) {
            if(m_ui->pageNumEdit->text().lastIndexOf('%')>0) return true;
            if(text == QString('%')) {
                if(m_ui->pageNumEdit->text().toDouble()>100) return true;
            }

            m_ui->pageNumEdit->setText(m_ui->pageNumEdit->text() + text);
            return true;
        }
    }
    return false;
}

void TocDlg::moveUp(){
    if(isPageUpdated) {
        on_actionUpdatePage_triggered();
        isPageUpdated = false;
    }
    int cur_num = getCurrentItemPosFromBegin(m_docview->getToc(),m_ui->treeWidget->currentItem(), 0);
    QScrollBar * scrollBar = m_ui->treeWidget->verticalScrollBar();

    // для отладки

    if((cur_num<=((curPage-1)*pageStrCount)+1) || (cur_num==1)){
        // last page
        if((curPage-1)==0){
            curPage=pageCount;
            scrollBar->setMaximum((pageCount-1) * pageStrCount);
            scrollBar->setValue(curPage*pageStrCount-pageStrCount);
            updateTitle();
            ////break;
            return;
        }
        // previous page
        if(curPage-1>=1) {
            curPage-=1;
            scrollBar->setValue(scrollBar->value()-pageStrCount);
            updateTitle();
        }
    }
}

void TocDlg::moveDown() {
    if(isPageUpdated) {
        on_actionUpdatePage_triggered();
        isPageUpdated = false;
    }
    int cur_num = getCurrentItemPosFromBegin(m_docview->getToc(),m_ui->treeWidget->currentItem(), 0);
    int r_num = getMaxItemPosFromBegin(m_docview->getToc(),m_ui->treeWidget->currentItem(),0);

    QScrollBar * scrollBar = m_ui->treeWidget->verticalScrollBar();
    // для отладки

    if((cur_num>=(curPage*pageStrCount)) || (cur_num==r_num)){
        // first page
        if(curPage+1>pageCount){
            curPage=1;
            scrollBar->setValue(0);
            updateTitle();
            return;
        }
        // next page
        if(curPage+1<=pageCount) {
            curPage+=1;
            scrollBar->setMaximum((pageCount-1) * pageStrCount);
            scrollBar->setValue(curPage*pageStrCount-pageStrCount);
            updateTitle();
        }
    }
}

// code added 28.11.2011
int TocDlg::getCurrentItemPosFromBegin(LVTocItem * item_top,QTreeWidgetItem * LevelItem,int level)
{
    if(isPageUpdated) {
        on_actionUpdatePage_triggered();
        isPageUpdated = false;
    }
    // поиск позиции выделенного элемента в дереве
    int cur_num;
    if(level == 0){
        cur_num = 0;
        countItems = 0;
    }
    for (int i=0; i<item_top->getChildCount(); i++ ) {
        countItems+=1;
        QTreeWidgetItem * LevelItem1 = LevelItem->child(i);
        if(level==0) LevelItem1 = m_ui->treeWidget->topLevelItem(i);
        if(LevelItem1 == m_ui->treeWidget->currentItem())
            cur_num=countItems;
        if(cur_num>0) return cur_num;
        if(LevelItem1->isExpanded())
            cur_num = getCurrentItemPosFromBegin(item_top->getChild(i),LevelItem1, level+1);
        if(cur_num>0) return cur_num;
    }
    return cur_num;
}


// code added 28.11.2011
int TocDlg::getMaxItemPosFromBegin(LVTocItem * item_top,QTreeWidgetItem * LevelItem,int level)
{
    if(isPageUpdated) {
        on_actionUpdatePage_triggered();
        isPageUpdated = false;
    }
    // подсчёт всего сколько элементов в дереве
    int r_num = item_top->getChildCount();
    if(level == 0) countItems = 0;
    for (int i=0; i<item_top->getChildCount(); i++ ) {
        countItems+=1;
        QTreeWidgetItem * LevelItem1 = LevelItem->child(i);
        if(level==0) LevelItem1 = m_ui->treeWidget->topLevelItem(i);
        if(LevelItem1->isExpanded()) {
            r_num = r_num + getMaxItemPosFromBegin(item_top->getChild(i),LevelItem1, level+1);
        } else {
        }
    }
    if((level==0) && (r_num!=countItems)) r_num = countItems;
    return r_num;
}

// code added 28.11.2011
int TocDlg::getCurrentItemPage()
{
    if(isPageUpdated) {
        on_actionUpdatePage_triggered();
        isPageUpdated = false;
    }
    QScrollBar * scrollBar = m_ui->treeWidget->verticalScrollBar();
    pageStrCount = scrollBar->pageStep();
    int page_num = 1;
    int iCurrentItemPos = getCurrentItemPosFromBegin(m_docview->getToc(),m_ui->treeWidget->currentItem(), 0);
    if(iCurrentItemPos>pageStrCount)
        page_num = ceil((double)iCurrentItemPos/pageStrCount);
    return page_num;
}

void TocDlg::setCurrentItemPage()
{
    if(isPageUpdated) {
        on_actionUpdatePage_triggered();
        isPageUpdated = false;
    }
    QScrollBar * scrollBar = m_ui->treeWidget->verticalScrollBar();
    int nearest_Page = getCurrentItemPage();
    if((curPage!=nearest_Page) && (nearest_Page>1)){
        if((nearest_Page==pageCount) && (pageCount>1)) {
            curPage=pageCount-1;
            scrollBar->setValue(scrollBar->value()-pageStrCount);
            updateTitle();
        }
        // тут ошибка непонятная возникает с позиционированием скролбара на последней странице
        curPage = nearest_Page;
        scrollBar->setMaximum((pageCount-1) * pageStrCount);
        scrollBar->setValue(curPage*pageStrCount-pageStrCount);
        updateTitle();
    }
}

// code added 28.11.2011
bool TocDlg::setCurrentItemFirstInPage(int page_num,LVTocItem * item_top,QTreeWidgetItem * LevelItem,int level)
{
    if(isPageUpdated) {
        on_actionUpdatePage_triggered();
        isPageUpdated = false;
    }
    // выбор первого на странице элемента при PageUp или PageDown
    int cur_num = page_num*pageStrCount-pageStrCount+1;
    bool found_flag = false;
    if(level == 0) countItems = 0;
    for (int i=0; i<item_top->getChildCount(); i++ ) {
        countItems+=1;
        if(countItems == cur_num) {
            if(level == 0)
                m_ui->treeWidget->setCurrentItem(m_ui->treeWidget->topLevelItem(i));
            else
                m_ui->treeWidget->setCurrentItem(LevelItem->child(i));
            return true;
        }
        QTreeWidgetItem * LevelItem1 = LevelItem->child(i);
        if(level==0) LevelItem1 = m_ui->treeWidget->topLevelItem(i);
        if(LevelItem1->isExpanded()) {
            found_flag = setCurrentItemFirstInPage(page_num,item_top->getChild(i),LevelItem1, level+1);
        } else {
            // page recount
        }
        if(found_flag)
            return true;
    }
    return found_flag;
}

void TocDlg::movePage()
{
    Device::forceFullScreenUpdate();

    QScrollBar * scrollBar = m_ui->treeWidget->verticalScrollBar();
    scrollBar->setMaximum((pageCount-1) * pageStrCount);
    scrollBar->setValue(curPage*pageStrCount-pageStrCount);

    // code added 28.11.2011
    setCurrentItemFirstInPage(curPage,m_docview->getToc(),m_ui->treeWidget->currentItem(),0);

    updateTitle();
}

void TocDlg::moveDownPage()
{
    if (curPage+1<=pageCount) {
        curPage += 1;
    } else {
        curPage = 1;
    }

    movePage();
}

void TocDlg::moveUpPage()
{
    if (curPage-1 >= 1) {
        curPage -= 1;
    } else {
        curPage = pageCount;
    }

    movePage();
}

void TocDlg::on_actionUpdatePage_triggered()
{
    isPageUpdated = true;

    // code added 28.11.2011
    // page recount
    fillOpts();
    updateTitle();
}

void TocDlg::on_actionNextPage_triggered()
{
    if(isPageUpdated) {
        on_actionUpdatePage_triggered();
        isPageUpdated = false;
    }
    moveDownPage();
}

void TocDlg::on_actionPrevPage_triggered()
{
    if(isPageUpdated) {
        on_actionUpdatePage_triggered();
        isPageUpdated = false;
    }
    moveUpPage();
}

void TocDlg::fillOpts()
{
    QScrollBar * scrollBar = m_ui->treeWidget->verticalScrollBar();

    pageStrCount = scrollBar->pageStep();
    fullStrCount = scrollBar->maximum()-scrollBar->minimum()+pageStrCount;

    if(fullStrCount==pageStrCount) {
        pageCount = 1;
        return;
    }
    if(pageStrCount==1) {
        scrollBar->setMaximum(fullStrCount*2);
        pageStrCount = scrollBar->pageStep();
    }
    pageCount = ceil((double)fullStrCount/pageStrCount);
}
