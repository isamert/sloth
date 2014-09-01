#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "qglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "utils/quick.h"
#include "utils/fileutils.h"
#include "qfilecopier/slothcopier.h"

class Clipboard : public QObject
{
    Q_OBJECT
public:
    explicit Clipboard(QObject *parent = 0);
    SlothCopier *sc;

    static bool hasFiles();
    static QStringList getFiles();

    static bool pasteImage(const QString &pathToPaste);
    static bool pasteText(const QString &pathToPaste);
    bool pasteFiles(const QString &pathToPaste);
    bool paste(const QString &pathToPaste);



signals:

public slots:

};

#endif // CLIPBOARD_H
