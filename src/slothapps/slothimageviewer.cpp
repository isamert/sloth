#include "slothapps/slothimageviewer.h"

SlothImageViewer::SlothImageViewer(QWidget *parent, const QString &filePath /* = "" */) :
    QWidget(parent)
{
    this->loadUi();

    if(!filePath.isEmpty())
        this->setFile(filePath);
}

void SlothImageViewer::loadUi() {
    this->vlayout = new QVBoxLayout(this);
    this->toolbar = new QToolBar(this);
    this->scroll = new QScrollArea(this);
    this->image = new QLabel(this);

    this->image->setBackgroundRole(QPalette::Base);
    this->image->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    this->image->setScaledContents(true);

    this->vlayout->setSpacing(0);
    this->vlayout->setMargin(0);

    this->vlayout->addWidget(this->toolbar);
    this->vlayout->addWidget(this->scroll);

    this->scroll->setBackgroundRole(QPalette::Dark);
    //this->scroll->setWidgetResizable(true);
    this->scroll->setWidget(this->image);
    this->scroll->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);

    this->actPrevious = new QAction(Quick::getIcon("back"), trUtf8("Previous"), this->toolbar);
    this->actNext = new QAction(Quick::getIcon("forward"), trUtf8("Next"), this->toolbar);
    this->actZoomIn = new QAction(Quick::getIcon("zoom-in"), trUtf8("Zoom In"), this->toolbar);
    this->actZoomOut = new QAction(Quick::getIcon("zoom-out"), trUtf8("Zoom Out"), this->toolbar);
    this->actNormal = new QAction(Quick::getIcon("zoom-original"), trUtf8("Normal Size"), this->toolbar);
    this->actFit = new QAction(Quick::getIcon("zoom-fit-best"), trUtf8("Fit to Window"), this->toolbar);

    this->toolbar->addAction(this->actPrevious);
    this->toolbar->addAction(this->actNext);
    this->toolbar->addSeparator();
    this->toolbar->addAction(this->actZoomIn);
    this->toolbar->addAction(this->actZoomOut);
    this->toolbar->addSeparator();
    this->toolbar->addAction(this->actNormal);
    this->toolbar->addAction(this->actFit);

    connect(this->actPrevious, SIGNAL(triggered()), this, SLOT(goPrevious()));
    connect(this->actNext, SIGNAL(triggered()), this, SLOT(goNext()));
    connect(this->actZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(this->actZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(this->actNormal, SIGNAL(triggered()), this, SLOT(normalSize()));
    connect(this->actFit, SIGNAL(triggered()), this, SLOT(fitToWindow()));
}

bool SlothImageViewer::setFile(const QString &filePath) {
    this->file = filePath;

    if (!filePath.isEmpty()) {
        QImage imageFile(filePath);
        this->image->setPixmap(QPixmap::fromImage(imageFile));
        this->scale = 1.0;
        this->normalSize();

        return true;
    }

    return false;
}

void SlothImageViewer::goPrevious() {
    QString lastFile = "";

    QDir dir(FileUtils::getUpperPath(this->file));
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    dir.setSorting(QDir::Name);

    foreach(QFileInfo info, dir.entryInfoList()) {
        QString path = info.absoluteFilePath();

        if(FileUtils::getMimeType(path).startsWith("image/")) {
            if(QDir::cleanPath(this->file) == QDir::cleanPath(path)) {
                if(!lastFile.isEmpty()) {
                    this->setFile(lastFile);
                    this->actNext->setEnabled(true);
                    emit this->currentFileChanged(lastFile);
                    return;
                }
            }

            lastFile = path;
        }
    }
    this->actPrevious->setEnabled(false);
}

void SlothImageViewer::goNext() {
    bool set = false;

    QDir dir(FileUtils::getUpperPath(this->file));
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    dir.setSorting(QDir::Name);

    foreach(QFileInfo info, dir.entryInfoList()) {
        QString path = info.absoluteFilePath();

        if(FileUtils::getMimeType(path).startsWith("image/")) {
            if(set) {
                this->setFile(path);
                this->actPrevious->setEnabled(true);
                emit this->currentFileChanged(path);
                return;
            }
            if(QDir::cleanPath(this->file) == QDir::cleanPath(path))
                set = true;
        }
    }
    this->actNext->setEnabled(false);
}

void SlothImageViewer::zoomIn() {
   this->changeScale(1.25);
}

void SlothImageViewer::zoomOut() {
    this->changeScale(0.8);
}

void SlothImageViewer::fitToWindow() {
    this->scroll->setWidgetResizable(true);
}

void SlothImageViewer::normalSize() {
    if(this->scroll->widgetResizable())
        this->scroll->setWidgetResizable(false);

    this->scale = 1.0;
    this->image->adjustSize();
}

void SlothImageViewer::changeScale(double x) {
    if(this->scroll->widgetResizable())
        this->scroll->setWidgetResizable(false);

    this->scale *= x;
    this->image->resize(this->scale * this->image->pixmap()->size());

    this->actZoomIn->setEnabled(this->scale < 3.0);
    this->actZoomOut->setEnabled(this->scale > 0.3);

    this->image->pixmap();
}

QString SlothImageViewer::getCurrentFile() {
    return this->file;
}

QStringList SlothImageViewer::getSupportedMimeTypes() {
    QStringList list;
    list << "application/x-php" << "application/x-shellscript" << "application/x-zerosize";
    return list;
}





