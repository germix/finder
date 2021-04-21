#include "Finder.h"
#include <QTextStream>
#include <QApplication>

#define PROCESS_OR_QUIT				\
	if(hasStopRequest()) return;	\
	msleep(2);

Finder::Finder() : stopRequest(false)
{
}
Finder::~Finder()
{
}
void Finder::start(const QString& dirPath, const QString& fileName, const QString& textInFile, QDir::Filters filters, bool subDirs)
{
	this->dir = QDir(dirPath);
	this->filters = filters;
	this->subDirs = subDirs;
	this->textInFile = textInFile;
	this->nameFilters = fileName.split(";");

	//
	// Eliminar filtros vacios
	//
	if(!nameFilters.isEmpty())
	{
		for(int i = nameFilters.size()-1; i >= 0; i--)
		{
			if(nameFilters.at(i).isEmpty())
				nameFilters.removeAt(i);
		}
	}
	//
	// Iniciar
	//
	QThread::start();
}
void Finder::cancel()
{
	if(!stopRequest)
	{
		mutex.lock();
		stopRequest = true;
		mutex.unlock();
	}
}
void Finder::run()
{
	emit onStart();
	findInFiles();
	emit onFinished();

}

void Finder::findInFiles()
{
#if 0
	qDebug("dir: %ls", dir.absolutePath().constData());
#endif
	QStringList files = dir.entryList(nameFilters, (QDir::NoSymLinks|QDir::NoDotAndDotDot)|filters);
	{
		QMutexLocker lock(&mutex);
		emit onDirectoryChanged(dir.absolutePath(), files.size());
	}
	for(int i = 0; i < files.size(); i++)
	{
		if(files[i] == "." || files[i] == "..")
			continue;
		{
			QMutexLocker lock(&mutex);
			emit onFileChanged(files[i], i);
		}

		PROCESS_OR_QUIT

		QFile file(dir.absoluteFilePath(files[i]));
		QFileInfo fi(file);
		if(!fi.isFile())
		{
			QMutexLocker lock(&mutex);
			emit onFileFound(dir.absoluteFilePath(files[i]));
		}
		else if(file.open(QIODevice::ReadOnly))
		{
			QString line;
			QTextStream in(&file);
			while(!in.atEnd())
			{
				PROCESS_OR_QUIT
				// ...
				line = in.readLine();
				if(line.contains(textInFile))
				{
					QMutexLocker lock(&mutex);
					emit onFileFound(dir.absoluteFilePath(files[i]));
					break;
				}
			}
		}
		PROCESS_OR_QUIT
	}
	if(subDirs)
	{
		QStringList directories = dir.entryList(QStringList("*"), (QDir::Dirs|QDir::NoSymLinks|QDir::NoDotAndDotDot)|(filters&~QDir::Files));

		for(int i = 0; i < directories.size(); i++)
		{
			QDir tmp = dir;
			dir.cd(directories[i]);
			findInFiles();
			dir = tmp;
			// ...
			PROCESS_OR_QUIT
		}
	}
}
bool Finder::hasStopRequest()
{
	QMutexLocker lock(&mutex);
	return stopRequest;
}


