#ifndef RecentBooksDlg_H
#define RecentBooksDlg_H

#include <QDialog>

#include <QMessageBox>
#include <QItemDelegate>
#include <QPainter>
#include <QTableWidget>

#include <math.h>

#include "cr3widget.h"
#include "crqtutil.h"
#include "lvdocview.h"
#include "settings.h"

#include "device/device.h"

namespace Ui {
class RecentBooksDlg;
}

class CR3View;

class RecentBooksDlg : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(RecentBooksDlg)
public:
    virtual ~RecentBooksDlg();

    static bool showDlg(QWidget * parent, CR3View * docView);
protected:
    explicit RecentBooksDlg(QWidget *parent, CR3View * docView);
    virtual void changeEvent(QEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);
private:
    Ui::RecentBooksDlg *m_ui;
    CR3View * m_docview;
    void openBook(int index);
    int getBookNum();

    int pageCount;
    int curPage;
    QString titleMask;
    void ShowPage(int updown, int selectRow = 1);
    void SetPageCount();
    // added 14.12.2011
    void removeFile(LVPtrVector<CRFileHistRecord> & files, int num);
    bool isCyclic;
private slots:
    void on_actionRemoveBook_triggered();
    void on_actionRemoveAll_triggered();
    void on_actionSelectBook_triggered();
    void on_actionNextPage_triggered();
    void on_actionPrevPage_triggered();
};

class RecentBooksListDelegate : public QItemDelegate {
public:
    RecentBooksListDelegate(QObject *pobj = 0) : QItemDelegate(pobj) { }
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItem newOption;
        QVariant value;
        newOption = option;
        newOption.state &= ~QStyle::State_HasFocus;

        value = index.data(Qt::UserRole);
        int id;
        if(value.isValid() && !value.isNull()) {
            id=value.toInt();
            if(id==3 || id==4)
                newOption.font.setPointSize(newOption.font.pointSize()-4);
        }

        QItemDelegate::paint(painter, newOption, index);

        painter->save();
        if(index.flags() & Qt::ItemIsSelectable) {
            Qt::GlobalColor lineColor;
            if(newOption.state & QStyle::State_Selected)
                lineColor = Qt::black;
            else
                lineColor = Qt::gray;
            QPen pen(lineColor, 2, Qt::SolidLine);
            painter->setPen(pen);
            painter->drawLine(newOption.rect.left(), newOption.rect.bottom(), newOption.rect.right(), newOption.rect.bottom());
        }
        painter->restore();
    }
};

#endif // RecentBooksDlg_H
