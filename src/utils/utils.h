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

};

#endif // UTILS_H
