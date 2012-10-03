#include "virtualkeysdlg.h"
#include "ui_virtualkeysdlg.h"

#include <QKeyEvent>
#include <QSettings>

VirtualKeysDialog::VirtualKeysDialog(QWidget *parent, QLineEdit * editView) :
	QDialog(parent),
	ui(new Ui::VirtualKeysDialog),
	_editline( editView )
{
	ui->setupUi(this);

	addAction(ui->actionNextTab);
	addAction(ui->actionPrevTab);

	ui->tabWidget->setCurrentIndex(0);
	ui->tableWidget->setFocus();
}

bool VirtualKeysDialog::showDlg( QWidget * parent, QLineEdit * editView )
{
	VirtualKeysDialog *dlg = new VirtualKeysDialog(parent, editView);
	dlg->setWindowFlags(dlg->windowType() | Qt::FramelessWindowHint);


	dlg->show();

	//editView->setText(QString('123'));
	return true;
}

VirtualKeysDialog::~VirtualKeysDialog()
{
//	delete ui;
}

void VirtualKeysDialog::on_actionNextTab_triggered()
{
	int CurrInd = ui->tabWidget->currentIndex();
	if(CurrInd+1 == ui->tabWidget->count())
		ui->tabWidget->setCurrentIndex(0);
	else
		ui->tabWidget->setCurrentIndex(CurrInd+1);
}

void VirtualKeysDialog::on_actionPrevTab_triggered()
{
	int CurrInd = ui->tabWidget->currentIndex();
	if(CurrInd == 0)
		ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
	else
		ui->tabWidget->setCurrentIndex(CurrInd-1);
}
