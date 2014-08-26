#include "slothitems/slothplacesbox.h"

SlothPlacesBox::SlothPlacesBox(QWidget *parent) :
    QComboBox(parent)
{
    //FIXME: load items from left places bar settings
    this->addItem(Quick::getIcon("user-home"), FileUtils::getName(QDir::homePath()),
                  QVariant(QDir::homePath()));
    this->addItem(Quick::getIcon("user-desktop"), trUtf8("Desktop"),
                  QVariant(this->getDesktopLocation()));
    this->addItem(Quick::getIcon("emblem-system"), "/" + trUtf8("(File System)"),
                  QVariant("/"));

    this->insertSeparator(this->count());

    this->addItem(Quick::getIcon("user-documents"), trUtf8("Documents"),
                  QVariant(this->getDocumentsLocationn()));
    this->addItem(Quick::getIcon("user-downloads"), trUtf8("Downloads"),
                  QVariant(this->getDownloadLocation()));
    this->addItem(Quick::getIcon("user-photos"), trUtf8("Photos"),
                  QVariant(this->getPicturesLocation()));
    this->addItem(Quick::getIcon("user-movies"), trUtf8("Movies"),
                  QVariant(this->getMoviesLocation()));

    this->insertSeparator(this->count());

    this->addItem(trUtf8("Other..."), QVariant("__SELECTFOLDER__"));

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChange(int)));

    this->lastIndex = 0;
}


void SlothPlacesBox::onCurrentIndexChange(int i) {
    if(this->itemData(i).toString() == "__SELECTFOLDER__") {
        QString dir = this->showFolderSelectionDialog();

        if(!dir.isEmpty()) {
            this->setCurrentPath(dir);
        }
        else
            this->setCurrentIndex(this->lastIndex);
    }
    this->lastIndex = i;
}

void SlothPlacesBox::setCurrentPath(const QString &path) {
    for(int i = 0; i < this->count(); ++i) {
        if(this->itemData(i).toString() == path) {
            this->setCurrentIndex(i);
            this->lastIndex = i;
            return;
        }
    }

    this->insertSeparator(this->count());
    this->addItem(FileUtils::getName(path), QVariant(path));
    this->setCurrentIndex(this->count() - 1);
}

QString SlothPlacesBox::showFolderSelectionDialog() {
    return QFileDialog::getExistingDirectory(this, trUtf8("Open Directory"), QDir::homePath(),
                                             QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
}


QString SlothPlacesBox::getDesktopLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#endif

#if QT_VERSION <= QT_VERSION_CHECK(5, 0, 0)
    return QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif
}

QString SlothPlacesBox::getDocumentsLocationn()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#endif

#if QT_VERSION <= QT_VERSION_CHECK(5, 0, 0)
    return QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#endif
}

QString SlothPlacesBox::getDownloadLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
#endif

#if QT_VERSION <= QT_VERSION_CHECK(5, 0, 0)
    return this->getFromXDG("DOWNLOADS");
#endif
}

QString SlothPlacesBox::getMoviesLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
#endif

#if QT_VERSION <= QT_VERSION_CHECK(5, 0, 0)
    return QDesktopServices::storageLocation(QDesktopServices::MoviesLocation);
#endif
}

QString SlothPlacesBox::getPicturesLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
#endif

#if QT_VERSION <= QT_VERSION_CHECK(5, 0, 0)
    return QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
#endif
}

QStringList SlothPlacesBox::getApplicationsLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
#endif

#if QT_VERSION <= QT_VERSION_CHECK(5, 0, 0)
    return QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif
}

QString SlothPlacesBox::getTemplatesLocation() {
    return getFromXDG("TEMPLATES");
}

QString SlothPlacesBox::getFromXDG(const QString &dirName) {
    QString cfile = FileUtils::combine(QDir::homePath(), ".config/user-dirs.dirs");
    QString home = QDir::homePath();

    QFile file(cfile);
    QString xdgLine = "XDG_" + dirName.toUpper() + "_DIR";
    QString path = "";


    if (file.open(QIODevice::ReadOnly)) {
       QTextStream in(&file);
       while (!in.atEnd()) {
          QString line = in.readLine();
          if(line.contains(xdgLine)) {
              path = line.replace(xdgLine, "").replace("=", "").replace("\"", "")
                                              .replace("~", home).replace("$HOME", home);
          }
       }
       file.close();
    }
    return path;
}
