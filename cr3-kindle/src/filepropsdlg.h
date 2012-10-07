#ifndef FILEPROPSDLG_H
#define FILEPROPSDLG_H

#include <QtGui/QDialog>
#include <QItemDelegate>

#include "lvdocview.h"
#include "cr3version.h"

#include "cr3widget.h"

namespace Ui {
class FilePropsDialog;
}

class FilePropsListDelegate : public QItemDelegate {
public:
    FilePropsListDelegate(QObject *pobj = 0) : QItemDelegate(pobj) { }
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItem newOption;
        QVariant value;
        newOption = option;
        value = index.data(Qt::UserRole);
        if(value.isValid() && !value.isNull()) {
            newOption.font.setPointSize(newOption.font.pointSize()-value.toInt());
        }
        QItemDelegate::paint(painter, newOption, index);
    }
};

class CR3View;
class LVDocView;

class FilePropsDialog : public QDialog {
    Q_OBJECT
public:
    ~FilePropsDialog();

    static bool showDlg( QWidget * parent, CR3View * docView );

protected:
    QString getDocText( const char * path, const char * delim );
    QString getDocAuthors( const char * path, const char * delim );
    void fillItems();
    void addPropLine( QString name, QString value);
    void addInfoSection(QString name);
    explicit FilePropsDialog(QWidget *parent, CR3View * docView );

    void changeEvent(QEvent *e);
    QString _value;

private:
    Ui::FilePropsDialog *m_ui;
    CR3View * _cr3v;
    LVDocView * _docview;
};

#endif // FILEPROPSDLG_H
