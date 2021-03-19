#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>

namespace Ui {
class RenameDialog;
}

class RenameDialog : public QDialog
{
	Q_OBJECT
private:
	Ui::RenameDialog* ui;
public:
	explicit RenameDialog(const QString& name, QWidget* parent = 0);
	~RenameDialog();
public:
	QString getName() const;
};

#endif // RENAMEDIALOG_H
