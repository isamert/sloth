#include "utils/clipboard.h"

Clipboard::Clipboard(QObject *parent) :
    QObject(parent)
{
    sc = new SlothCopier();
}

bool Clipboard::hasFiles() {
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    bool hasFiles = true;
    if(mimeData->hasText()) {
        QStringList listFiles = mimeData->text().split("\n");

        foreach(QString file, listFiles) {
            if(!file.isEmpty()) {
                QFileInfo info(file);
                if(!info.exists())
                    hasFiles = false;
            }
        }
    }
    else
        return false;

    return hasFiles;
}

QStringList Clipboard::getFiles() {
    //must call hasFiles before

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    QStringList listFiles = mimeData->text().split("\n");

    return listFiles;
}

bool Clipboard::pasteImage(const QString &pathToPaste) {
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if (mimeData->hasImage()) {
        bool ok;
        QString fileName = Quick::getText(trUtf8("File name"), trUtf8("New file name:"),
                                          &ok, trUtf8("clipboard") + ".png");

        if(ok) {
            QString filePath = FileUtils::combine(pathToPaste, fileName);
            QFile file(filePath);

            if(file.exists()) {
                Quick::msgWarning(trUtf8("Error"), trUtf8("The file already exists."));
                return false;
            }
            if(!file.open(QIODevice::WriteOnly)) {
                Quick::msgWarning(trUtf8("Error"), trUtf8("Cannot write to the file."));
                return false;
            }

            qvariant_cast<QImage>(mimeData->imageData()).save(&file, "PNG");
            file.close();

            return true;
        }
    }
    return false;
}

bool Clipboard::pasteText(const QString &pathToPaste) {
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if(mimeData->hasText()) {
        bool ok;
        QString fileName = Quick::getText(trUtf8("File name"), trUtf8("New file name:"), &ok);

        if(ok) {
            QString filePath = FileUtils::combine(pathToPaste, fileName);
            QFile file(filePath);

            if(file.exists()) {
                Quick::msgWarning(trUtf8("Error"), trUtf8("The file already exists."));
                return false;
            }
            if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                Quick::msgWarning(trUtf8("Error"), trUtf8("Cannot write to the file."));
                return false;
            }

            QTextStream out(&file);
            out << mimeData->text().toUtf8();
            file.close();

            return true;
        }
    }

    return false;
}

bool Clipboard::pasteFiles(const QString &pathToPaste) {
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool result = false;

    if(hasFiles()) {
        QStringList listFiles = mimeData->text().split("\n");

        foreach (QString file, listFiles) {
            QFileInfo info(file);
            QString newPath;
            if(info.isFile())
                newPath = pathToPaste;
            else if(info.isDir())
                newPath = FileUtils::combine(pathToPaste, FileUtils::getName(file));

            sc->copier->copy(file, newPath);
            result = true;
            //result = FileUtils::copyRecursively(file, newPath);

            if(!result)
                return result;
        }
    }
    return result;
}

bool Clipboard::paste(const QString &pathToPaste) {
    if(pasteFiles(pathToPaste) ||  pasteText(pathToPaste) || pasteImage(pathToPaste))
        return true;
    return false;
}

