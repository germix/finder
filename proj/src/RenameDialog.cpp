#include "RenameDialog.h"
#include "ui_RenameDialog.h"

RenameDialog::RenameDialog(const QString& name, QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::RenameDialog)
{
	ui->setupUi(this);

	ui->txtName->setText(name);
}

RenameDialog::~RenameDialog()
{
	delete ui;
}

QString RenameDialog::getName() const
{
	return ui->txtName->text();
}
