#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

#include <QDir>
#include <QComboBox>

#include "Languages.h"

class QSettings;
class QTreeWidgetItem;


#include "Finder.h"
#include "FindingDialog.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
	Ui::MainWindow*	ui;

	Languages		languages;
	QMenu*			menuLanguages;

	Finder*			finderThread;
	FindingDialog*	findingDialog;
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
private:
	bool eventFilter(QObject* obj, QEvent* evt);
private:
	void initMenu(QSettings& s);
	void initComboBoxes(QSettings& s);
	void checkDirectory();
	void updateCombobox(QComboBox* cbo);
private:
	void changeEvent(QEvent* e);
private slots:
	void find();
	void browse();
	void slotMenu();
	void slotAbout();
	void slotComboBox_currentTextChanged(const QString& text);
	void slotTreeWidget_itemActivated(QTreeWidgetItem *item, int column);
	void slotTreeWidget_customContextMenuRequested(const QPoint& pos);

	void slotFinder_start();
	void slotFinder_finished();
	void slotFinder_fileFound(const QString& fname);
	void slotFinder_fileChanged(const QString& fname, int idx);
	void slotFinder_directoryChanged(const QString& directory, int max);
	void slotFinderDialog_cancelRequest();
};


#endif // MAINWINDOW_H
