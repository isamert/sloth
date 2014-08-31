#include "utils/desktopfile.h"

DesktopFile::DesktopFile(QObject *parent, const QString &filePath /* = "" */) :
    QObject(parent)
{
    this->setPath(filePath);
}

void DesktopFile::setPath(const QString &filePath) {
    this->desktopFile = filePath;
}

QStringList DesktopFile::getDesktopFileFromMimeInfo(const QString &mimeType) {
    QStringList mimeInfos;
    mimeInfos << "/usr/share/applications/mimeinfo.cache";
    mimeInfos << FileUtils::combine(QDir::homePath(), "/.local/share/applications/mimeinfo.cache");
    mimeInfos << FileUtils::combine(QDir::homePath(), "/.local/share/applications/mimeapps.list");

    foreach (QString mimeInfo, mimeInfos) {
        QFile file(mimeInfo);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return QStringList();

        QTextStream ts(&file);
        while (!ts.atEnd()) {
            QStringList line = ts.readLine().trimmed().split("=");
            if(line[0] == mimeType) {
                file.close();

                QStringList desktopFiles;
                foreach (QString d, line[1].split(";")) {
                    if(!d.isEmpty())
                        desktopFiles << FileUtils::combine(FileUtils::getUpperPath(mimeInfo), d);
                }

                return desktopFiles;
            }
        }
    }
    return QStringList();
}

bool DesktopFile::load() {
    QFile file(this->desktopFile);

    if (this->desktopFile.isEmpty())
        return false;
    else if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    bool desktopEntry = false; // [Desktop Entry]
    QTextStream ts(&file);

    while (!ts.atEnd()) {
        QString line = ts.readLine().trimmed();

        if(line.startsWith("[")) {
            if(line == "[Desktop Entry]")
                desktopEntry = true;
            else
                desktopEntry = false;
        }

        if(desktopEntry) {
            QStringList lineSplitted = QString(line).split("=");

            if(lineSplitted[0] == "Name")
                this->name = lineSplitted[1];
            else if(lineSplitted[0] == "Exec")
                this->exec = lineSplitted[1];
            else if(lineSplitted[0] == "Icon")
                this->icon = lineSplitted[1];
            else if(lineSplitted[0] == "MimeType")
                this->mimeTypes = lineSplitted[1];
        }
    }

    file.close();
    return true;

}

QString DesktopFile::getName() {
    return this->name;
}

QIcon DesktopFile::getIcon() {
    if(!this->icon.isEmpty()) {
        if(QIcon::hasThemeIcon(this->icon)) {
            return QIcon::fromTheme(this->icon);
        }
        else if(QFile::exists(this->icon))
            return QIcon(this->icon);
        else {
            QString share = "/usr/share/pixmaps"; //TODO: add /usr/local/share, ~/.local/share
            QString shareIcon = FileUtils::combine(share, this->icon + ".png");
            QString shareIcon2 = FileUtils::combine(share, this->icon);

            if(QFile::exists(shareIcon))
                return QIcon(shareIcon);
            else if(QFile::exists(shareIcon2))
                return QIcon(shareIcon2);
        }
    }
    return QIcon();
}

QStringList DesktopFile::getMimeTypes() {
    return QString(this->mimeTypes).split(";");
}

QString DesktopFile::getExec() {
    return this->exec;
}

void DesktopFile::clear() {
    this->name = "";
    this->icon = "";
    this->exec = "";
    this->mimeTypes = "";
}
