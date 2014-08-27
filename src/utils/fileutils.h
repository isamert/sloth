#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "qglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#include <magic.h>
#endif

#include "utils/quick.h"

class FileUtils
{
private:
    FileUtils() { }

public:
    static QString join(const QStringList &list);
    static QString combine(const QString &path1, const QString &path2);
    static QString getName(const QString &path);
    static QString getUpperPath(const QString &path);
    static QString getFileNameDoesNotExists(const QString &fullFilePath, const QString &joinString,
                                            int i = 1, bool getFullPath = false);

    static bool moveItem(const QString &oldPath, const QString &newPath);
    static bool renameItem(const QString &oldPath, const QString &newPath);
    static bool rename(const QString &oldPath);
    static bool moveToTrash(const QString &path);
    static bool removeRecursively(const QString &itemPath);
    static bool copyRecursively(const QString &srcPath, const QString &tgtPath);

    static qint64 getDirSize(const QString &path);

    static QString getMimeType(const QString &fileName);
    static bool openDetached(const QString &path);

    static QString formatFileSize(const qint64 fileSize);

};

#endif // FILEUTILS_H
