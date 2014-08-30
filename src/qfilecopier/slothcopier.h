#ifndef SLOTHCOPIER_H
#define SLOTHCOPIER_H

#include <QObject>
#include <QDebug>
#include <QApplication>
#include <QThread>

#include "qfilecopier/qfilecopier.h"

class SlothCopier : public QObject
{
    Q_OBJECT
public:
    explicit SlothCopier(QObject *parent = 0);
    QFileCopier *copier;

signals:

public slots:
    void onStateChanged(QFileCopier::State);
    void onStarted(int);
    void onFinished(int);
    void onProgress(qint64, qint64);
    void onError(int id, QFileCopier::Error error, bool stopped);

};

#endif // SLOTHCOPIER_H
