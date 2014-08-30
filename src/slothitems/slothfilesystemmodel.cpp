#include "slothfilesystemmodel.h"

SlothFileSystemModel::SlothFileSystemModel(QObject *parent) :
    QFileSystemModel(parent)
{
    this->desktopFile = new DesktopFile(this);
}
QVariant SlothFileSystemModel::data(const QModelIndex & index, int role) const {
    QFileInfo info = this->fileInfo(index);

    //handle desktop files
    if(info.suffix() == "desktop") {
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

    return QFileSystemModel::data(index, role);
}
