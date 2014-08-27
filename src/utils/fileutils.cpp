#include "utils/fileutils.h"

QString FileUtils::combine(const QString &path1, const QString &path2) {
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}

QString FileUtils::join(const QStringList &list) {
    QString path = list[0];
    for(int i = 1; i < list.count(); ++i) {
        QString cpath = list[i];
        if(cpath.startsWith('/'))
            path = cpath;
        else if(path.isEmpty() || path.endsWith('/'))
            path += cpath;
        else
            path += '/' + cpath;
    }
    return QDir::cleanPath(path);
}

QString FileUtils::getName(const QString &path) {
    QStringList list = path.split('/');
    return (list.last().isEmpty() ? list[list.count() - 2] : list.last());
}

QString FileUtils::getUpperPath(const QString &path) {
    QStringList list = path.split('/');
    list.takeLast();
    return QDir::cleanPath(list.join("/"));
}

QString FileUtils::getFileNameDoesNotExists(const QString &fullFilePath, const QString &joinString,
                                              int i /* = 1 */, bool getFullPath /* = false */) {
    QString fileName = fullFilePath;

    if(QFile(fullFilePath).exists()) {
        fileName = fullFilePath + joinString + QString::number(i);
        while(QFile(fileName).exists()) {
            fileName.remove(fileName.count() - 1, 1);
            fileName += QString::number(i);
            ++i;
        }
    }

    if(getFullPath)
        return fileName;
    else
        return getName(fileName);
}

bool FileUtils::moveItem(const QString &oldPath, const QString &newPath) {
    QFileInfo info(oldPath);
    if(info.isDir())
        return QDir().rename(oldPath, newPath);
    else if(info.isFile())
        return QFile::rename(oldPath, newPath);

    return false;
}

bool FileUtils::renameItem(const QString &oldPath, const QString &newPath) {
    return moveItem(oldPath, newPath);
}

bool FileUtils::rename(const QString &oldPath) {
    QString _oldName = getName(oldPath);

    QFileInfo info(oldPath);
    if(info.isWritable()) {
        bool ok;
        QString newName = Quick::getText(QObject::trUtf8("Rename"),
                                         QObject::trUtf8("New name:"), &ok, _oldName);

        if (ok && !newName.isEmpty()) {
            if(!oldPath.isEmpty()) {
                QStringList newPathSplitted = oldPath.split('/');
                newPathSplitted.last().replace(_oldName, newName);
                QString newPath = newPathSplitted.join("/");

                return moveItem(oldPath, newPath);
            }
        }
    }
    else
        Quick::msgWarning(QObject::trUtf8("Error"),
                          QObject::trUtf8("You don't have permission to rename this item."));

    return false;
}

bool FileUtils::moveToTrash(const QString &path) {
    QFileInfo info(path);

    if(!info.exists())
        return false;

    if(!info.isWritable()) {
        Quick::msgWarning(QObject::trUtf8("Error"), QObject::trUtf8("You don't have permission to delete this item."));
        return false;
    }

    QString trashFiles = combine(QDir::homePath(), ".local/share/Trash/files");
    QString trashInfo = combine(QDir::homePath(), ".local/share/Trash/info");

    QString itemName = getName(path);
    QString trashPath = combine(trashFiles, itemName);
    QString _trashPath = trashPath;

    //getFileNameDoesntExist
    int i = 0;
    if(QFile(trashPath).exists()) {
        int i = 2;
        _trashPath = combine(trashFiles, itemName) + "." + QString::number(i);
        while(QFile(_trashPath).exists()) {
            _trashPath.remove(_trashPath.count() - 1, 1);
            _trashPath += QString::number(i);
            ++i;
        }
    }

    QString _trashInfo = combine(trashInfo, itemName) + (i == 0 ? "":"." + QString::number(i)) + ".trashinfo";
    moveItem(path, _trashPath);

    QDate date;

    QString dateTime = QString::number(date.currentDate().day()) + "-" +
                       QString::number(date.currentDate().month()) + "-" +
                       QString::number(date.day()) + "T" +
                       QString::number(QTime::currentTime().hour()) + ":" +
                       QString::number(QTime::currentTime().minute()) + ":" +
                       QString::number(QTime::currentTime().second());

    QSettings infoFile(_trashInfo);
    infoFile.beginGroup("Trash Info");
    infoFile.setValue("Path", path);
    infoFile.setValue("DeletionDate", dateTime);
    infoFile.endGroup();

    QFile file(_trashInfo);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "[Trash Info]\n";
    out << "Path=" + path + '\n';
    out << "DeletionDate=" + dateTime + '\n';
    file.close();

    return true;
}


//FIXME: if its too long, show progress bar etc.. use QThread
bool FileUtils::removeRecursively(const QString &itemPath) {
    bool result = true;

    QFileInfo info(itemPath);
    if(info.isFile())
        return QFile::remove(itemPath);

    //firstly, delete files
    QDirIterator iterator(itemPath, QDir::Hidden | QDir::AllDirs | QDir::AllEntries | QDir::System,
                                    QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
       iterator.next();
       if (iterator.fileInfo().isFile()) {
           result = QFile::remove(iterator.fileInfo().absoluteFilePath());
       }
       if(!result)
           return result;
    }

    //secondly, delete folders
    while(QFile::exists(itemPath)) {
        QDirIterator iteratorDirs(itemPath, QDir::Hidden | QDir::AllDirs, QDirIterator::Subdirectories);
        while (iteratorDirs.hasNext()) {
           iteratorDirs.next();
           QString dirName = iteratorDirs.fileInfo().fileName();
           if (iteratorDirs.fileInfo().isDir() /* && dirName != "." */ && dirName != "..") {
               qDebug() << iteratorDirs.fileInfo().absoluteFilePath();
               result = QDir("/").rmdir(iteratorDirs.fileInfo().absoluteFilePath());
           }
        }
    }

    return result;
}

bool FileUtils::copyRecursively(const QString &srcPath, const QString &tgtPath) {
    bool result = true;
    QFileInfo srcInfo(srcPath);

    if(srcInfo.isFile()) {
        if(!QFile(tgtPath).exists())
            result = QDir::root().mkpath(tgtPath);

        QString newPath = combine(tgtPath, srcInfo.fileName());
        if(!QFile(newPath).exists())
            result = QFile::copy(srcPath, newPath);

        if (!result)
            return result;
    }
    else if(srcInfo.isDir()) {
        QDir dirSrc(srcPath);
        QDir dirTgt(tgtPath);

        QDir(getUpperPath(tgtPath)).mkdir(getName(tgtPath));

        foreach(QFileInfo info, dirSrc.entryInfoList(QDir::NoDotAndDotDot | QDir::System |
                                                     QDir::Hidden  | QDir::AllDirs |
                                                     QDir::Files, QDir::DirsFirst)) {
            if(info.isDir()) {
                dirTgt.mkdir(getName(info.absoluteFilePath()));
                QString newDir = combine(tgtPath, getName(info.absoluteFilePath()));
                result = copyRecursively(info.absoluteFilePath(), newDir);
            }
            else if(info.isFile()) {
                QString newFile = combine(tgtPath, getName(info.absoluteFilePath()));
                result = QFile::copy(info.absoluteFilePath(), newFile);
            }
            if (!result)
                return result;
        }
    }
    return result;
}

qint64 FileUtils::getDirSize(const QString &path) {
    QFileInfo finfo(path);
    if(!finfo.isDir())
        return 0;

    qint64 totalSize = 0;

    QDirIterator iterator(path, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
       iterator.next();
       if (iterator.fileInfo().isFile()) {

           totalSize += iterator.fileInfo().size();
       }
    }
    return totalSize;
}

QString FileUtils::getMimeType(const QString &fileName) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(fileName);
    return type.name();
#else
//http://va-sorokin.blogspot.com.tr/2011/03/how-to-get-mime-type-on-nix-system.html
    QString result("application/octet-stream");
    magic_t magicMimePredictor;
    magicMimePredictor = magic_open(MAGIC_MIME_TYPE); // Open predictor
    if (!magicMimePredictor) {
        qDebug() << "libmagic: Unable to initialize magic library";
    }
    else {
        if (magic_load(magicMimePredictor, 0)) { // if not 0 - error
            qDebug() << "libmagic: Can't load magic database - " +
                        QString(magic_error(magicMimePredictor));
            magic_close(magicMimePredictor); // Close predictor
        }
        else {
            char *file = fileName.toAscii().data();
            const char *mime;
            mime = magic_file(magicMimePredictor, file); // getting mime-type
            result = QString(mime);
            magic_close(magicMimePredictor); // Close predictor
        }
    }
    return result;
#endif
}

bool FileUtils::openDetached(const QString &path) {
    QFileInfo info(path);

    if(info.exists()) {
        if(info.isExecutable()) {
            QProcess::startDetached(path);
        }
        else {
            bool opened = QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            if(!opened)
                return false; //FIXME: if there is not a registered app, handle it
        }

        return true;
    }
    else
        return false;
}

QString FileUtils::formatFileSize(const qint64 fileSize) {
    const int KB = 1024; // Kilobyte
    const int MB = 1024 * KB; // Megabyte
    const int GB = 1024 * MB; // Gigabyte

    QString result;
    if(fileSize < KB)
        result = QString::number(fileSize) + " B";
    else if(fileSize < MB)
        result = QString::number(fileSize / KB) + " KB";
    else if(fileSize < GB)
        result = QString::number(fileSize / MB) + " MB";
    else if(fileSize >= GB)
        result = QString::number(fileSize / GB) + " GB";

    return result;
}
