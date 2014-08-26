#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QtWidgets>

#include "utils/quick.h"
#include "utils/fileutils.h"

class Clipboard : public QObject
{
    Q_OBJECT
public:
    explicit Clipboard(QObject *parent = 0);
    static bool hasFiles();

    static bool pasteImage(const QString &pathToPaste);
    static bool pasteText(const QString &pathToPaste);
    static bool pasteFiles(const QString &pathToPaste);
    static bool paste(const QString &pathToPaste);


signals:

public slots:

};

#endif // CLIPBOARD_H
