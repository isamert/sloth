#include "slothfilesystemmodel.h"

SlothFileSystemModel::SlothFileSystemModel(QObject *parent) :
    QFileSystemModel(parent)
{
    this->desktopFile = new DesktopFile(this);
}
QVariant SlothFileSystemModel::data(const QModelIndex & index, int role) const {
    //FIXME: use a diffrent role to send exec commands
    QFileInfo info = this->fileInfo(index);
    QString suffix = info.suffix();

    //handle desktop files
    if(suffix == "desktop") {
        this->desktopFile->setPath(info.absoluteFilePath());
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
    else if(suffix == "png" || suffix == "jpg") {
        if(role == Qt::DecorationRole) {
            if(info.size() < 1024 * 150) // loading big files is very laggy
                return QPixmap(info.absoluteFilePath()).scaled(48, 48);
        }
    }

    return QFileSystemModel::data(index, role);
}
