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

QStringList Utils::getDrives() {
    QStringList mountpoints;;

    QFile file("/etc/mtab");
    if(file.open(QFile::ReadOnly)) {
        while(true) {
            QStringList parts = QString::fromLocal8Bit(file.readLine()).trimmed().split(" ");
            if (parts.count() > 1) {
                if(parts[1].contains("/media"))
                    mountpoints << parts[1];
            }
            else
                break;
        }
    }
    return mountpoints;
}

QString Utils::getCacheDir() {
    QString path = FileUtils::combine(QDir::homePath(), ".cache/sloth");
    if(QFile::exists(path))
        QDir::root().mkdir(path);
    return path;
}

QString Utils::getTempFile() {
    return FileUtils::combine(getCacheDir(), "temp.temp");
}

int Utils::ramdomInt(int low, int high) {
    return qrand() % ((high + 1) - low) + low;
}

QString Utils::randomString(int charlen) {
    QString str;
    static QString alphanumeric = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < charlen; ++i)
        str[i] = alphanumeric[rand() / (RAND_MAX / alphanumeric.count())]; //it's hard to get a really random thing

    return str;
}
