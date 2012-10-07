#ifndef OPENFILEDLG_H
#define OPENFILEDLG_H

// code added 28.11.2011
#include <QMessageBox>

#include <QDialog>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QItemDelegate>
#include <QPainter>
#include <QProxyStyle>

#include "lvstream.h"
#include "cr3widget.h"
#include "crqtutil.h"

namespace Ui {
class OpenFileDlg;
}

class CR3View;

class FileListDelegate : public QItemDelegate {
public:
    FileListDelegate(QObject *pobj = 0) : QItemDelegate(pobj) { }
    void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QPixmap &pixmap) const
    {
        if(option.state & QStyle::State_Selected) {
            QPoint p = QStyle::alignedRect(option.direction, option.decorationAlignment, pixmap.size(), rect).topLeft();
            painter->drawPixmap(p, pixmap);
            return;
        }
        QItemDelegate::drawDecoration(painter, option, rect, pixmap);
    }
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {;
        if(option.state & QStyle::State_Selected) {
            painter->save();

            QVariant value;
            QStyleOptionViewItemV4 opt = setOptions(index, option);
            value = index.data(Qt::DisplayRole);
            QString text;
            if(value.isValid() && !value.isNull())
                text = value.toString();
            value = index.data(Qt::DecorationRole);
            QPixmap pixmap = qvariant_cast<QIcon>(value).pixmap(option.decorationSize, QIcon::Normal, QIcon::On);
            QRect decorationRect = QRect(QPoint(0, 0), pixmap.size());

            QRect checkRect;
            QRect displayRect;
            doLayout(opt, &checkRect, &decorationRect, &displayRect, false);

            drawDecoration(painter, opt, decorationRect, pixmap);
            drawDisplay(painter, opt, displayRect, text);

            QPen pen(Qt::black, 2, Qt::SolidLine);
            painter->setPen(pen);
            painter->drawLine(displayRect.bottomLeft(), displayRect.bottomRight());

            painter->restore();
            return;
        }
        QItemDelegate::paint(painter, option, index);
    }
};

class OpenFileDlg : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(OpenFileDlg)
public:
    explicit OpenFileDlg(QWidget *parent, CR3View * docView);
    virtual ~OpenFileDlg();
    QString CurrentDir;

    static bool showDlg(QWidget *parent, CR3View * docView);
private slots:
    void on_actionSelectFile_triggered();
    void on_actionGoToBegin_triggered();
    void on_actionNextPage_triggered();
    void on_actionPrevPage_triggered();
    void on_actionGoToLastPage_triggered();

    void on_actionGoToFirstPage_triggered();

    // code added 28.11.2011
    void on_actionRemoveFile_triggered();

private:
    void FillFileList();
    void ShowPage(int updown);
    Ui::OpenFileDlg *m_ui;
    CR3View * m_docview;
    QIcon folder, file, arrowUp;
    QStringList curFileList;
    int curPage, pageCount;
    QString titleMask;
    int dirCount;
    int rowCount;
};

#endif // OPENFILEDLG_H
