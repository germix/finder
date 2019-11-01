#ifndef FINDER_H
#define FINDER_H
#include <QThread>
#include <QDir>
#include <QMutex>
#include <QStringList>

class Finder : public QThread
{
	Q_OBJECT
	QDir			dir;
	QDir::Filters	filters;
	bool			subDirs;
	QString			textInFile;
	QStringList		nameFilters;

	QMutex			mutex;
	bool			stopRequest;
public:
	Finder();
	~Finder();
public:
	void start(const QString& dirPath, const QString& fileName, const QString& textInFile, QDir::Filters filters, bool subDirs);
	void cancel();
signals:
	void onStart();
	void onFinished();
	void onFileFound(const QString& fname);
	void onFileChanged(const QString& fname, int idx);
	void onDirectoryChanged(const QString& directory, int max);
private:
	void run();
	void findInFiles();
	bool hasStopRequest();
};

#endif // FINDER_H
