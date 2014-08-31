#include "sloththumbnailcache.h"

SlothThumbnailCache::SlothThumbnailCache(QObject *parent) :
    QObject(parent)
{

}

QString SlothThumbnailCache::cacheDir(const QString &size) {
    QString cache = FileUtils::combine(QDir::homePath(), ".cache/sloth/thumbnails/" + size);
    if(!QFile::exists(cache))
        QDir::root().mkpath(cache);

    return cache;
}

QString SlothThumbnailCache::cacheDir(const QSize &size) {
    return this->cacheDir(QString::number(size.height()) + "x" + QString::number(size.width()));
}

QString SlothThumbnailCache::iconCacheIndexFile(const QSize &size) {
    return FileUtils::combine(this->cacheDir(size), "cache.index");
}

QIcon SlothThumbnailCache::addToCache(const QString &path, const QPixmap &pixmap, const QSize &size) {
    QString randstr = Utils::randomString(20);
    QString cacheFilePath = FileUtils::combine(this->cacheDir(size), randstr);

    QFile indexFile(this->iconCacheIndexFile(size));
    QFile cacheFile(cacheFilePath);

    if(!indexFile.open(QIODevice::Append) || !cacheFile.open(QIODevice::WriteOnly))
        return QIcon();

    QTextStream appendIndexFile(&indexFile);
    appendIndexFile << randstr + "=" + QDir::cleanPath(path) + "\n";

    pixmap.scaled(size, Qt::KeepAspectRatioByExpanding).save(&cacheFile, "PNG");

    indexFile.close();
    cacheFile.close();

    return QIcon(cacheFilePath);
}

QIcon SlothThumbnailCache::getFromCache(const QString &path, const QSize &size) {
    QFile file(this->iconCacheIndexFile(size));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QIcon();
    }

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        QStringList splittedLine = QString(line).split("=");

        if(QDir::cleanPath(path) == QDir::cleanPath(splittedLine[1].remove('\n'))) {
            file.close();
            return QIcon(FileUtils::combine(this->cacheDir(size), splittedLine[0]));
        }
    }
    return QIcon();
}


bool SlothThumbnailCache::removeFromCache(const QString &path, const QSize &size) {
    //TODO: todo
}

bool SlothThumbnailCache::clearCache() {
    //TODO: todo
}
