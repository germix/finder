////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IconFromFile
//
// Germán Martínez - 29/05/2018
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <QIcon>
#include <QString>

#if ((QT_VERSION_MAJOR > 5) || (QT_VERSION_MAJOR == 5 && QT_VERSION_MINOR >= 2))

#include <QFileIconProvider>

QIcon IconFromFile(const QString& fileName)
{
	return QFileIconProvider().icon(QFileInfo(fileName));
}

#else

#include <QPixmap>
#if defined(Q_OS_WIN)
#include <windows.h>

#include <QFileInfo>
#include <QtWinExtras/QtWin>

QIcon IconFromFile(const QString& fileName)
{
	QIcon qIcon;
	HICON hIcon;

	hIcon = ExtractIconW(NULL, (const WCHAR*)fileName.constData(), 0);
	if(hIcon == NULL)
	{
		SHFILEINFO shfi;
		DWORD dwFileAttributes;

		memset(&shfi, 0, sizeof(shfi));
		if(QFileInfo(fileName).isDir())
			dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		else
			dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;

		SHGetFileInfoW((const WCHAR*)fileName.constData(),
					   dwFileAttributes,
					   &shfi,
					   sizeof(SHFILEINFO),
					   SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON);

		hIcon = shfi.hIcon;
	}
	if(hIcon != NULL)
	{
#if (QT_VERSION_MAJOR == 4)
		qIcon = QIcon(QPixmap::fromWinHICON(hIcon));
#else
		qIcon = QtWin::fromHICON(hIcon);
#endif
		DestroyIcon(hIcon);
	}
	return qIcon;
}
#elif defined(Q_OS_LINUX)
#include <QMimeData>
#include <QMimeDatabase>
#include <QApplication>
#include <QStyle>
#include <QFileInfo>

QIcon IconFromFile(const QString& fileName)
{
	QMimeDatabase db;
	QIcon icon;
	QList<QMimeType> types = db.mimeTypesForFileName(fileName);
	for(int i = 0; i < types.count(); i++)
	{
		icon = QIcon::fromTheme(types[i].iconName());
		if(!icon.isNull())
			return icon;
	}
	if(QFileInfo(fileName).isDir())
		return QApplication::style()->standardIcon(QStyle::SP_DirIcon);
	return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
}

#endif

#endif
