#ifndef SLOTHFILESYSTEMMODEL_H
#define SLOTHFILESYSTEMMODEL_H

#include <QObject>
#include <QFileSystemModel>

#include "utils/desktopfile.h"
#include "slothobjects/sloththumbnailcache.h"

class SlothFileSystemModel : public QFileSystemModel
{
    Q_OBJECT

private:
    QSize iconSize;
    bool thumbnailsEnabled;

public:
    explicit SlothFileSystemModel(QObject *parent = 0, const QSize &iconSize = QSize(48, 48));
    DesktopFile *desktopFile;
    SlothThumbnailCache *thumbnailCache;

    void setThumbnailsEnabled(bool enabled);
    bool isThumbnailsEnabled();

    void setIconSize(const QSize &size);
    QSize getIconSize();
signals:

public slots:

protected:
    QVariant data(const QModelIndex & index, int role) const;

};

#endif // SLOTHFILESYSTEMMODEL_H
