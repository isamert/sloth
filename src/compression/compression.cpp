#include "compression/compression.h"

Compression::Compression(QObject *parent) :
    QObject(parent)
{
    proc = new QProcess();
    connect(proc, SIGNAL(finished(int)), this, SLOT(handleFinish(int)));
    connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(handleError()));
    connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));

    listFtype7z << "-ttar" << "-tgzip" << "-tbzip2" << "-t7z" << "-tzip" << "-tiso" << "-tudf";
    listCtype7z << "-mx0" << "-mx1" << "-mx3" << "-mx5" << "-mx7" << "-mx9";
    listFtypeTar << "-cvf" << "-cvzf" << "-cvjf";
}


bool Compression::compressWith7z(const QStringList &items, const QString &fileName, int ctype, int ftype, const QString &password) {
    QStringList args;
    args << "a" << this->listCtype7z[ctype] << this->listFtype7z[ftype] <<
            fileName << (password.isEmpty() ? "":"-p" + password);

    foreach(QString item, items)
        args << item;

    this->proc->start("7z", args);
    return true;
}

bool Compression::compressWithTar(const QStringList &items, const QString &fileName, int ftype) {
    /*
     * FType.tar (.tar),
     * FType.gzip (.gz, .tgz, .tar.gz),
     * FType.bzip2(.bz2, .tar.bz2, .tbz2, .tb2)
    */
    QStringList args;
    args << this->listFtypeTar[ftype] << fileName;

    foreach(QString item, items)
        args << FileUtils::getName(item);

    this->proc->setWorkingDirectory(FileUtils::getUpperPath(items[0]));
    this->proc->start("tar", args);
    return true;
}

bool Compression::compressWithRar(const QStringList &items, const QString &fileName) {
    QStringList args;
    args << "a" << fileName << "-ep1";
    //-ep1 = Do not store the path entered at the command line in archive. Exclude base folder from names.

    foreach(QString item, items)
        args << item;

    this->proc->start("rar", args);

    return true;
}

bool Compression::extract(const QString &path, const QString &extractPath) {
    //FIXME: Utils::mimeTypesOfArchives() --see
    //TODO: handle those: (7zip handles all)
    /* ARJ, CAB, CHM, CPIO, CramFS, DMG, FAT, HFS, LZH,
     *MBR, MSI, NSIS, NTFS, SquashFS, UDF, VHD, WIM, XAR and Z */

    QString command;
    QStringList args;

    QFileInfo info(path);
    if(!info.exists())
        return false;

    QString mime = FileUtils::getMimeType(path);
    if(mime == "application/x-tar") {
        command = "tar";
        args << "-xvf";
    }
    else if(mime == "application/x-gzip") {
        command = "tar";
        args << "-xvzf";
    }
    else if(mime == "application/x-bzip" || mime == "application/x-bzip2") {
        command = "tar";
        args << "-xvjf";
    }
    else if(mime == "application/x-7z-compressed" || mime == "application/zip" ||
            mime == "application/x-iso9660-image" || mime == "application/x-rar-compressed" ||
            mime == "application/x-rpm" || mime == "application/vnd.debian.binary-package" ||
            mime == "application/x-lzma") {
        command = "7z";
        args << "e" << "-y";
    }
    else
        return false;

    args << path;
    this->proc->setWorkingDirectory(extractPath);
    this->proc->start(command, args);

    return true;
}

bool Compression::createIso(const QStringList &items, const QString &fileName) {
    QStringList args;
    args << "-o" << fileName;

    foreach(QString item, items)
        args << item;

    this->proc->start("genisoimage", args);
    return true;
}

void Compression::readOutput() {
    emit this->progressChanged(QString(this->proc->readAllStandardOutput()));

}

void Compression::handleError() {
    QString out = QString(this->proc->readAllStandardError());
    emit this->compressionError(out);
}

void Compression::handleFinish(int i) {
    emit this->compressFinished(i);
}

bool Compression::is7zipInstalled() {
    return Utils::checkIfCommandExists("7z");
}

bool Compression::isRarInstalled() {
    return Utils::checkIfCommandExists("rar");
}

bool Compression::isGenIsoImageInstalled() {
    return Utils::checkIfCommandExists("genisoimage");
}

