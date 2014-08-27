#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "qglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "utils/fileutils.h"
#include "utils/utils.h"

class Compression : public QObject
{
    Q_OBJECT

private:
    QStringList listFtype7z;
    QStringList listFtypeTar;
    QStringList listCtype7z;
    QProcess *proc;

public:
    explicit Compression(QObject *parent = 0);

    enum FType {
        tar=0,
        gzip=1,
        bzip2=2,
        t7z=3,
        zip=4,
        iso=5,
        udf=6
    };

    enum CType {
        CopyMode=0,
        Fastest=1,
        Fast=2,
        Normal=3,
        Maximum=4,
        Ultra=5
    };
    bool compressWith7z(const QStringList &items, const QString &fileName, int ctype, int ftype, const QString &password);
    bool compressWithTar(const QStringList &items, const QString &fileName, int ftype);
    bool compressWithRar(const QStringList &items, const QString &fileName);
    bool extract(const QString &path, const QString &extractPath);
    bool createIso(const QStringList &items, const QString &fileName);

    bool is7zipInstalled();
    bool isRarInstalled();
    bool isGenIsoImageInstalled();

signals:
    void progressChanged(const QString &out);
    void compressFinished(int i);
    void compressionError(const QString &err);

private slots:
    void handleFinish(int i);
    void handleError();
    void readOutput();

};

#endif // COMPRESSION_H
