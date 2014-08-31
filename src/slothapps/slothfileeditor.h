#ifndef SLOTHFILEEDITOR_H
#define SLOTHFILEEDITOR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QTextCodec>
#include <QToolBar>
#include <QFile>
#include <QAction>
#include <QByteArray>

#include "utils/quick.h"

class SlothFileEditor : public QWidget
{
    Q_OBJECT

private:
    QVBoxLayout *vlayout;
    QToolBar *toolbar;
    QPlainTextEdit *textEdit;

    QAction *actSave;
    QAction *actUndo;
    QAction *actRedo;
    QAction *actCut;
    QAction *actCopy;
    QAction *actPaste;

    QString file;

public:
    explicit SlothFileEditor(QWidget *parent = 0, const QString &filePath = "");

    void loadUi();
    bool setFile(const QString &filePath);
    QString getCurrentFile();

    static QStringList getSupportedMimeTypes();

signals:

public slots:
    bool saveFile();

};

#endif // SLOTHFILEEDITOR_H
