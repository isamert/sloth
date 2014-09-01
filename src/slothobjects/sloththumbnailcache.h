#ifndef SLOTHTHUMBNAILCACHE_H
#define SLOTHTHUMBNAILCACHE_H

#include <QObject>
#include <QFile>

#include "utils/fileutils.h"
#include "utils/utils.h"

class SlothThumbnailCache : public QObject
{
    Q_OBJECT

private:
    QString cacheDir(const QString &size);
    QString cacheDir(const QSize &size);
    QString iconCacheIndexFile(const QSize &size);

public:
    explicit SlothThumbnailCache(QObject *parent = 0);

    QIcon addToCache(const QString &path, const QPixmap &pixmap, const QSize &size,
                     Qt::AspectRatioMode aspectRatio = Qt::IgnoreAspectRatio);
    QIcon getFromCache(const QString &path, const QSize &size);
    bool removeFromCache(const QString &path, const QSize &size);
    bool clearCache();

signals:

public slots:

};

#endif // SLOTHTHUMBNAILCACHE_H
