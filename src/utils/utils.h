#ifndef UTILS_H
#define UTILS_H

#include <QtWidgets>

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