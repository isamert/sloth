#ifndef SLOTHIMAGEVIEWER_H
#define SLOTHIMAGEVIEWER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QToolBar>
#include <QScrollArea>
#include <QLabel>
#include <QAction>

#include "utils/quick.h"
#include "utils/fileutils.h"

class SlothImageViewer : public QWidget
{
    Q_OBJECT

private:
    QVBoxLayout *vlayout;
    QToolBar *toolbar;
    QScrollArea *scroll;
    QLabel *image;

    QAction *actNext;
    QAction *actPrevious;
    QAction *actNormal;
    QAction *actFit;
    QAction *actZoomIn;
    QAction *actZoomOut;

    QString file;
    double scale;

    void changeScale(double x);

public:
    explicit SlothImageViewer(QWidget *parent = 0, const QString &filePath = "");

    void loadUi();
    bool setFile(const QString &filePath);
    QString getCurrentFile();

    static QStringList getSupportedMimeTypes();


signals:
    void currentFileChanged(const QString &filePath);

public slots:
    void goPrevious();
    void goNext();
    void zoomIn();
    void zoomOut();
    void fitToWindow();
    void normalSize();

};

#endif // SLOTHIMAGEVIEWER_H
