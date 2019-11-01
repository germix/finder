////////////////////////////////////////////////////////////////////////////////////////////////////
//
// GetFileTypeFromFileInfo
//
// Germán Martínez - 29/05/2018
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <QFileInfo>
#if defined(Q_OS_WIN)
#include <windows.h>
QString GetFileTypeFromFileInfo(const QFileInfo& fi)
{
	static QHash<QString,QString> map;
	if(fi.isFile())
	{
		QString type = map[fi.suffix()];
		if(type.isEmpty())
		{
			QString     path = fi.absoluteFilePath();
			DWORD       attr = GetFileAttributesW((const WCHAR*)path.utf16());
			SHFILEINFOW shfi;

			SHGetFileInfoW((const WCHAR*)path.utf16(), attr, &shfi, sizeof(shfi), SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME);

			type = QString::fromWCharArray(shfi.szTypeName);
			if(!type.isEmpty())
			{
				map[fi.suffix()] = type;
			}
		}
		return type;
	}
	return QString();
}
#elif defined(Q_OS_LINUX)
#include <QMimeDatabase>
QString GetFileTypeFromFileInfo(const QFileInfo& fi)
{
	return QMimeDatabase().mimeTypeForFile(fi).name();
}
#endif

