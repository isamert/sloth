#ifndef SLOTHFILESYSTEMMODEL_H
#define SLOTHFILESYSTEMMODEL_H

#include <QObject>
#include <QFileSystemModel>

#include "utils/desktopfile.h"

class SlothFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit SlothFileSystemModel(QObject *parent = 0);
    DesktopFile *desktopFile;

signals:

public slots:

protected:
    QVariant data(const QModelIndex & index, int role) const;

};

#endif // SLOTHFILESYSTEMMODEL_H
