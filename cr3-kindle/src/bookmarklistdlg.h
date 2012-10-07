#ifndef BOOKMARKLISTDLG_H
#define BOOKMARKLISTDLG_H

#include <QDialog>
#include <QModelIndex>
#include <QMenu>

#include "cr3widget.h"

namespace Ui {
    class BookmarkListDialog;
}

class CR3View;
class CRBookmark;

class BookmarkListDialog : public QDialog {
    Q_OBJECT
public:
    ~BookmarkListDialog();
    static bool showDlg( QWidget * parent, CR3View * docView);
protected:
    explicit BookmarkListDialog(QWidget *parent, CR3View * docView);
    void changeEvent(QEvent *e);
private:
    Ui::BookmarkListDialog *m_ui;
    CR3View * _docview;
    QList<CRBookmark*> _list;
    CRBookmark * selectedBookmark();
private slots:
    void on_actionRemoveBookmark_triggered();
    void on_actionRemoveAllBookmarks_triggered();
    void on_actionSelectBookmark_triggered();
};

#endif // BOOKMARKLISTDLG_H
