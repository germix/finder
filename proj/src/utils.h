#ifndef UTILS_H
#define UTILS_H
#include <QIcon>

QIcon IconFromFile(const QString& fileName);
void ShowInContainerFolder(const QString& fileName);

class QFileInfo;
QString GetFileTypeFromFileInfo(const QFileInfo& fi);

#endif // UTILS_H
