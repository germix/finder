#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "utils.h"

#include <QSettings>
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QKeyEvent>
#include <QTextStream>
#include <QDesktopServices>
#include <QMimeDatabase>

#include "AboutDialog.h"
#include "RenameDialog.h"

#define SETTINGS_APPLICATION "Finder"
#define SETTINGS_ORGANIZATION "Germix"

#define COLUMN_FILE			0
#define COLUMN_SIZE			1
#define COLUMN_MIME			2
#define COLUMN_FULLNAME		3

void saveComboBoxState(QSettings& s, QComboBox* cbo, const QString& key)
{
	QStringList list;
	for(int i = 0; i < cbo->count(); i++)
		list << cbo->itemText(i);
	s.setValue(key, list);
}
void loadComboBoxState(QSettings& s, QComboBox* cbo, const QString& key)
{
	QStringList list = s.value(key).toStringList();
	for(int i = 0; i < list.size(); i++)
	{
		if(cbo->findText(list.at(i)) == -1)
			cbo->addItem(list.at(i));
	}
}
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	QSettings s(SETTINGS_ORGANIZATION, SETTINGS_APPLICATION);

	ui->chkDirs->setChecked(s.value("FindDirs", true).toBool());
	ui->chkFiles->setChecked(s.value("FindFiles", true).toBool());
	ui->chkHidden->setChecked(s.value("FindHidden", false).toBool());
	ui->chkSubDirs->setChecked(s.value("FindSubDirs", false).toBool());
	ui->chkSystem->setChecked(s.value("FindSystem", false).toBool());

	connect(ui->btnFind, SIGNAL(clicked()), this, SLOT(find()));
	connect(ui->btnBrowse, SIGNAL(clicked()), this, SLOT(browse()));

	//
	// TreeWidget
	//
	connect(ui->treeWidget,
			SIGNAL(itemActivated(QTreeWidgetItem*,int)),
			this,
			SLOT(slotTreeWidget_itemActivated(QTreeWidgetItem*,int)));
	connect(ui->treeWidget,
			SIGNAL(customContextMenuRequested(QPoint)),
			this,
			SLOT(slotTreeWidget_customContextMenuRequested(QPoint)));
	ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->treeWidget->header()->restoreState(s.value("TreeHeaderState").toByteArray());
	ui->treeWidget->setSortingEnabled(true);
	ui->treeWidget->sortByColumn(0, Qt::AscendingOrder);

	// ...
	initMenu(s);
	initComboBoxes(s);

	//
	// Restaurar estado anterior
	//
	restoreGeometry(s.value("WindowGeometry").toByteArray());
	restoreState(s.value("WindowState").toByteArray());
}

MainWindow::~MainWindow()
{
	QSettings s(SETTINGS_ORGANIZATION, SETTINGS_APPLICATION);

	s.setValue("Language", languages.language());
	s.setValue("WindowState", saveState());
	s.setValue("WindowGeometry", saveGeometry());
	s.setValue("TreeHeaderState", ui->treeWidget->header()->saveState());
	saveComboBoxState(s, ui->cboFile, "ComboBoxFile");
	saveComboBoxState(s, ui->cboText, "ComboBoxText");
	saveComboBoxState(s, ui->cboPath, "ComboBoxPath");

	s.setValue("FindDirs", ui->chkDirs->isChecked());
	s.setValue("FindFiles",ui->chkFiles->isChecked());
	s.setValue("FindHidden",ui->chkHidden->isChecked());
	s.setValue("FindSubDirs",ui->chkSubDirs->isChecked());
	s.setValue("FindSystem",ui->chkSystem->isChecked());

	delete ui;
}
bool MainWindow::eventFilter(QObject* obj, QEvent* evt)
{
	if(evt->type() == QKeyEvent::KeyPress)
	{
		QKeyEvent* e = (QKeyEvent*)(evt);
		if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
		{
			find();
		}
		else if(e->key() == Qt::Key_Delete && e->modifiers() & Qt::ShiftModifier)
		{
			QComboBox* cbo = qobject_cast<QComboBox*>(obj);
			QAbstractItemView* view = qobject_cast<QAbstractItemView*>(obj);
			if(cbo)
			{
				int index = cbo->currentIndex();
				cbo->removeItem(index);
			}
			else if(view)
			{
				QModelIndex index = view->currentIndex();
				view->model()->removeRow(index.row(), index.parent());
			}
			e->accept();
			return true;
		}
	}
	return QMainWindow::eventFilter(obj, evt);
}
void MainWindow::initMenu(QSettings& s)
{
	connect(ui->btnMenu, SIGNAL(clicked()), this, SLOT(slotMenu()));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));

	menuLanguages = new QMenu(tr("Languages"));
	menuLanguages->setIcon(QIcon(":/images/menu-languages.png"));
	languages.init(menuLanguages, "translations", "finder", s.value("Language").toString());
}
void MainWindow::initComboBoxes(QSettings& s)
{
	ui->cboFile->installEventFilter(this);
	ui->cboFile->view()->installEventFilter(this);
	ui->cboText->installEventFilter(this);
	ui->cboText->view()->installEventFilter(this);
	ui->cboPath->installEventFilter(this);
	ui->cboPath->view()->installEventFilter(this);
	loadComboBoxState(s, ui->cboFile, "ComboBoxFile");
	loadComboBoxState(s, ui->cboText, "ComboBoxText");
	loadComboBoxState(s, ui->cboPath, "ComboBoxPath");
	if(!ui->cboFile->count()) ui->cboFile->addItem("*");
	if(!ui->cboPath->count()) ui->cboPath->addItem(QDir::currentPath());

	checkDirectory();
	connect(ui->cboPath, SIGNAL(currentTextChanged(QString)), this, SLOT(slotComboBox_currentTextChanged(QString)));
}
void MainWindow::checkDirectory()
{
	QString path = ui->cboPath->lineEdit()->text();
	QPalette palette = ui->cboPath->palette();
	if(path.isEmpty())
	{
		palette.setColor(QPalette::Base, QColor(150, 255, 150));
	}
	else
	{
		if(!QFileInfo(path).exists())
			palette.setColor(QPalette::Base, QColor(255, 205, 200));
		else
			palette.setColor(QPalette::Base, QColor(150, 255, 150));
	}
	ui->cboPath->lineEdit()->setPalette(palette);
}
void MainWindow::updateCombobox(QComboBox* cbo)
{
#if 0
	if(cbo->findText(cbo->currentText()) == -1)
	{
		cbo->insertItem(0, cbo->currentText());
	}
#else
	int index;
	QString text = cbo->currentText();
	if(-1 != (index = cbo->findText(text)))
	{
		cbo->removeItem(index);
	}
	cbo->insertItem(0, text);
	cbo->setCurrentIndex(0);
#endif
}
void MainWindow::changeEvent(QEvent* e)
{
	if(e != NULL)
	{
		switch(e->type())
		{
#if 1
			case QEvent::LocaleChange:
				{
					QString locale = QLocale::system().name();
					locale.truncate(locale.lastIndexOf('_'));
					languages.load(locale);
				}
				break;
#endif
			case QEvent::LanguageChange:
				ui->retranslateUi(this);
				menuLanguages->setTitle(tr("Languages"));
				ui->treeWidget->setHeaderLabels(QStringList() << tr("File") << tr("Size") << tr("Type") << tr("Full name"));
				break;
			default:
				break;
		}
	}
	QMainWindow::changeEvent(e);
}
void MainWindow::find()
{
	ui->treeWidget->clear();

	QString path = ui->cboPath->currentText();
	if(!QFileInfo(path).exists())
	{
		QMessageBox::information(0, "", tr("The directory does not exists"), QMessageBox::Ok);
		return;
	}
	QString fileName = ui->cboFile->currentText();
	QString textInFile = ui->cboText->currentText();

	updateCombobox(ui->cboFile);
	updateCombobox(ui->cboText);
	updateCombobox(ui->cboPath);

	if(fileName.isEmpty())
	{
		fileName = "*";
	}
	QDir::Filters filters = 0;
	if(ui->chkDirs->isChecked()) filters |= QDir::Dirs;
	if(ui->chkFiles->isChecked()) filters |= QDir::Files;
	if(ui->chkHidden->isChecked()) filters |= QDir::Hidden;
	if(ui->chkSystem->isChecked()) filters |= QDir::System;

	// ...
	finderThread = new Finder();
	connect(finderThread, SIGNAL(onStart()), this, SLOT(slotFinder_start()));
	connect(finderThread, SIGNAL(onFinished()), this, SLOT(slotFinder_finished()));
	connect(finderThread, SIGNAL(onFileFound(QString)), this, SLOT(slotFinder_fileFound(QString)));
	connect(finderThread, SIGNAL(onFileChanged(QString,int)), this, SLOT(slotFinder_fileChanged(QString,int)));
	connect(finderThread, SIGNAL(onDirectoryChanged(QString,int)), this, SLOT(slotFinder_directoryChanged(QString,int)));
	finderThread->start(path, fileName, textInFile, filters | QDir::NoSymLinks,  ui->chkSubDirs);
}
void MainWindow::browse()
{
	QString path = QFileDialog::getExistingDirectory(this, tr("Choose directory"), ui->cboPath->currentText());

	if(!path.isEmpty())
	{
#if 1
		if(ui->cboPath->findText(path) == -1)
		{
			ui->cboPath->insertItem(0, path);
		}
		ui->cboPath->setCurrentIndex(ui->cboPath->findText(path));
#else
		ui->cboPath->setCurrentText(path);
#endif
	}
}
void MainWindow::slotMenu()
{
	QMenu menu;

	menu.addMenu(menuLanguages);
	menu.addSeparator();
	menu.addAction(ui->actionAbout);

	menu.exec(QCursor::pos());
}
void MainWindow::slotAbout()
{
	AboutDialog().exec();
}
void MainWindow::slotComboBox_currentTextChanged(const QString& text)
{
	Q_UNUSED(text);
	checkDirectory();
}
void MainWindow::slotTreeWidget_itemActivated(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(item->text(COLUMN_FULLNAME))))
	{
		QMessageBox::information(0, tr("Open file"), tr("Can't open file"), QMessageBox::Ok);
	}
}
void MainWindow::slotTreeWidget_customContextMenuRequested(const QPoint& pos)
{
	QTreeWidgetItem* item = ui->treeWidget->itemAt(pos);
	if(item)
	{
		QMenu menu;
		QAction* action;

		menu.addAction(ui->actionRename);
		menu.addSeparator();
		menu.addAction(ui->actionOpenContainerFolder);
		if(nullptr != (action = menu.exec(QCursor::pos())))
		{
			if(action == ui->actionRename)
			{
				RenameDialog dlg(item->text(COLUMN_FILE));
				if(dlg.exec() == RenameDialog::Accepted)
				{
					QString oldFullName = item->text(COLUMN_FULLNAME);
					QDir dir = QFileInfo(oldFullName).absoluteDir();
					QString newFullName = dir.absoluteFilePath(dlg.getName());

					if(QFile::rename(oldFullName, newFullName))
					{
						ui->treeWidget->clear();
					}
					else
					{
						QMessageBox::information(this, tr("Rename"), tr("Could not be renamed"), QMessageBox::Ok);
					}
				}
			}
			else if(action == ui->actionOpenContainerFolder)
			{
				ShowInContainerFolder(item->text(COLUMN_FULLNAME));
			}
		}
	}
}
void MainWindow::slotFinder_start()
{
	findingDialog = new FindingDialog(this);
	connect(findingDialog, SIGNAL(onCancelRequest()), this, SLOT(slotFinderDialog_cancelRequest()));
	findingDialog->setWindowModality(Qt::WindowModal);
	findingDialog->show();
}
void MainWindow::slotFinder_finished()
{
	delete findingDialog;
	finderThread->deleteLater();
	finderThread->wait();
}
void MainWindow::slotFinder_fileFound(const QString& fname)
{
	QFileInfo fi(fname);
	qint64 size = fi.size();
	QString szSize;
	QString szType;

	if(fi.isDir())
	{
		// Nada
	}
	else if(fi.isFile())
	{
		if(size >= 1024)
			szSize = QString("%1 KB").arg(int((size + 1023) / 1024));
		else
			szSize = QString("%1 bytes").arg(size);

		szType = GetFileTypeFromFileInfo(fi);
	}

	QTreeWidgetItem* item = new QTreeWidgetItem
	(
		QStringList()
		<< fi.fileName()
		<< szSize
		<< szType
		<< fname
	);
#if 0
	item->setIcon(0, IconFromFile(fi.fileName()));
#else
	item->setIcon(0, IconFromFile(fname));
#endif
	ui->treeWidget->addTopLevelItem(item);
}
void MainWindow::slotFinder_fileChanged(const QString& fname, int idx)
{
	findingDialog->fileChanged(fname, idx);
}
void MainWindow::slotFinder_directoryChanged(const QString& directory, int max)
{
	findingDialog->directoryChanged(directory, max);
}
void MainWindow::slotFinderDialog_cancelRequest()
{
	finderThread->cancel();
}



