#ifndef FINDINGDIALOG_H
#define FINDINGDIALOG_H

#include <QDialog>

namespace Ui {
class FindingDialog;
}

class FindingDialog : public QDialog
{
	Q_OBJECT
	Ui::FindingDialog* ui;
public:
	explicit FindingDialog(QWidget *parent = 0);
	~FindingDialog();

public:
	void fileChanged(const QString& fname, int idx);
	void directoryChanged(const QString& directory, int max);
signals:
	void onCancelRequest();
private slots:
	void slotCancel();
};

#endif // FINDINGDIALOG_H
