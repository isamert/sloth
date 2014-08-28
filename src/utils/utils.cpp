#include "utils/utils.h"

bool Utils::checkIfCommandExists(const QString &command) {
    QStringList list;
    list << "/bin" << "/usr/bin" << "/usr/local/bin" << "/sbin"  << "/usr/sbin" << "/usr/local/sbin";
    foreach(QString path, list) {
        if(QFile::exists(FileUtils::combine(path, command)))
            return true;
    }
    return false;
}

QStringList Utils::mimeTypesOfArchives() {
    QStringList list;
    list << "application/x-tar" << "application/x-gzip" << "application/x-bzip" << "application/x-bzip2" <<
            "application/x-7z-compressed" << "application/zip" << "application/x-iso9660-image" <<
            "application/x-rar-compressed" << "application/x-rpm" << "application/vnd.debian.binary-package" <<
            "application/x-lzma" << "application/x-cd-image";
    return list;
}
