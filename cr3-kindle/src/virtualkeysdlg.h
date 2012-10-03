#ifndef VIRTUALKEYSDLG_H
#define VIRTUALKEYSDLG_H

#include <QDialog>
#include <QEvent>
#include <QKeyEvent>
#include <QtGui/QLineEdit>

#include "lvstring.h"
#include "cr3widget.h"

namespace Ui {
	class VirtualKeysDialog;
}

class VirtualKeysDialog : public QDialog
{
	Q_OBJECT
public:
	VirtualKeysDialog(QWidget *parent, QLineEdit * editView);
	~VirtualKeysDialog();
	static bool showDlg( QWidget * parent, QLineEdit * editView );
private slots:
	void on_actionNextTab_triggered();

	void on_actionPrevTab_triggered();

private:
	CR3View * _docview;
	QLineEdit * _editline;
	lString16 _lastPattern;
	Ui::VirtualKeysDialog *ui;
};

#endif // VIRTUALKEYSDLG_H
