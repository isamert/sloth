#include "slothfilesystemmodel.h"

SlothFileSystemModel::SlothFileSystemModel(QObject *parent, const QSize &iconSize /* = QSize(48, 48) */) :
    QFileSystemModel(parent)
{
    this->desktopFile = new DesktopFile(this);
    this->iconSize = iconSize;
    this->thumbnailsEnabled = true; //FIXME: = false yap
}

void SlothFileSystemModel::setThumbnailsEnabled(bool enabled) {
    this->thumbnailsEnabled = enabled;
}

bool SlothFileSystemModel::isThumbnailsEnabled() {
    return this->thumbnailsEnabled;
}

void SlothFileSystemModel::setIconSize(const QSize &size) {
    this->iconSize = size;
}

QSize SlothFileSystemModel::getIconSize() {
    return this->iconSize;
}

QVariant SlothFileSystemModel::data(const QModelIndex & index, int role) const {
    //FIXME: use a diffrent role to send exec commands
    QFileInfo info = this->fileInfo(index);
    QString fullFilePath = info.absoluteFilePath();
    QString suffix = info.suffix();

    //handle desktop files
    if(suffix == "desktop") {
        this->desktopFile->clear();
        this->desktopFile->setPath(fullFilePath);

        if(!this->desktopFile->load())
            return QFileSystemModel::data(index, role);

        if(role == Qt::DisplayRole) {
            QString name = this->desktopFile->getName();
            return name.isEmpty() || name.isNull() ? QFileSystemModel::data(index, role) : name;
        }
        else if(role == Qt::DecorationRole) {
            QIcon icon = this->desktopFile->getIcon();
            return icon.isNull() ? QFileSystemModel::data(index, role) : icon;
        }
    }

    if(role == Qt::DecorationRole) {
        if(this->thumbnailsEnabled && FileUtils::getMimeType(fullFilePath).startsWith("image/")) {

            if(info.size() < 51200) // 1024 * 50 (50KB) doesn't need to cache.
                return QPixmap(info.absoluteFilePath()).scaled(this->iconSize);

            QIcon icon = this->thumbnailCache->getFromCache(fullFilePath, this->iconSize);

            if(icon.isNull()) {
                QPixmap pixmap(fullFilePath);
                return this->thumbnailCache->addToCache(fullFilePath, pixmap, this->iconSize);
            }
            else
                return icon;
        }
    }

    return QFileSystemModel::data(index, role);
}
