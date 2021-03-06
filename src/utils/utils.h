#ifndef UTILS_H
#define UTILS_H

#include "qglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "utils/utils.h"
#include "utils/fileutils.h"

class Utils
{
private:
    Utils() {}

public:
    static bool checkIfCommandExists(const QString &currentPath);
    static QStringList mimeTypesOfArchives();
    static QStringList getDrives();

    static QString getCacheDir();
    static QString getTempFile();

    static int ramdomInt(int low, int high);
    static QString randomString(int charlen);
};

#endif // UTILS_H
