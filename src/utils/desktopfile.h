#ifndef DESKTOPFILE_H
#define DESKTOPFILE_H

#include <QObject>
#include <QIcon>
#include <QTextStream>
#include <QFile>

#include "utils/fileutils.h"

class DesktopFile : public QObject
{
    Q_OBJECT

private:
    QString desktopFile;

    QString name;
    QString exec;
    QString icon;
    QString mimeTypes;

public:
    explicit DesktopFile(QObject *parent = 0, const QString &filePath = "");

    static QString getDefaultDesktopFileFromMimeInfo(const QString &mimeType);
    static QStringList getDesktopFilesFromMimeInfo(const QString &mimeType);

    void setPath(const QString &filePath);
    bool load();
    QString getName();
    QIcon getIcon();
    QStringList getMimeTypes();
    QString getExec();

    void clear();

signals:

public slots:

};

#endif // DESKTOPFILE_H
