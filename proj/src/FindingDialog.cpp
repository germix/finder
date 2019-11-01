#include "FindingDialog.h"
#include "ui_FindingDialog.h"

FindingDialog::FindingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FindingDialog)
{
	ui->setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

	connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
}

FindingDialog::~FindingDialog()
{
	delete ui;
}
void FindingDialog::fileChanged(const QString& fname, int idx)
{
	ui->lblCurrentFileValue->setText(fname);
	ui->progressBar->setValue(idx);
}
void FindingDialog::directoryChanged(const QString& directory, int max)
{
	ui->lblDirectoryValue->setText(directory);
	ui->progressBar->setValue(0);
	ui->progressBar->setMaximum(max);
}
void FindingDialog::slotCancel()
{
	emit onCancelRequest();
}
