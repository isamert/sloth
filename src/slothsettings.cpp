#include "slothsettings.h"

SlothSettings::SlothSettings()
{

}

void SlothSettings::firstTimeRunning() {
    QSettings settings;
    settings.beginGroup("SlothFileManager");
    if(settings.value("firstTime", true).toBool()) {
        settings.setValue("firstTime", false);
        settings.setValue("size", defaultSize());
        settings.setValue("pos", defaultPosition());
        settings.setValue("fullscreen", false);
    }
    settings.endGroup();
}

QSize SlothSettings::defaultSize() {
    return QSize(720, 450);
}

QPoint SlothSettings::defaultPosition() {
    int width = defaultSize().width();
    int height = defaultSize().height();

    QDesktopWidget wid;
    int screenWidth = wid.screen()->width();
    int screenHeight = wid.screen()->height();

    return QPoint((screenWidth - width)/2, (screenHeight - height)/2);
}

void SlothSettings::loadWindowValues(QMainWindow *mw) {
    QSettings settings;
    settings.beginGroup("SlothFileManager");
    mw->resize(settings.value("size").toSize());
    mw->move(settings.value("pos").toPoint());
    if(settings.value("fullscreen").toBool())
        mw->showMaximized();
    settings.endGroup();
}

void SlothSettings::saveWindowValues(QMainWindow *mw) {
    QSettings settings;
    settings.beginGroup("SlothFileManager");
    settings.setValue("size", mw->size());
    settings.setValue("pos", mw->pos());
    settings.setValue("fullscreen", mw->isFullScreen());
    settings.endGroup();
}

void SlothSettings::loadListViewValues(QListView *view) {
    QSettings settings;
    settings.beginGroup("SlothListView");
    view->setViewMode(static_cast<QListView::ViewMode>(settings.value("viewMode", QListView::IconMode).toUInt()));

    QSize iconSize = settings.value("iconSize", QSize(48, 48)).toSize();
    view->setIconSize(iconSize);
    view->setGridSize(QSize(iconSize.width() + 48, iconSize.height() + 48));
    settings.endGroup();
}

void SlothSettings::saveListViewValues(QListView *view) {
    //TODO:
    QSettings settings;
    settings.beginGroup("ListView");
    //settings.setValue();
    settings.endGroup();
}

QStringList SlothSettings::defaultPlaces() {
    QStringList list;
    list << QObject::trUtf8("Home") + "=" + QDir::homePath();
    list << QObject::trUtf8("Desktop") + "=" + SlothPlacesBox::getDesktopLocation();
    list << QObject::trUtf8("Documents") + "=" + SlothPlacesBox::getDocumentsLocationn();
    list << QObject::trUtf8("Downloads") + "=" + SlothPlacesBox::getDownloadLocation();
    list << QObject::trUtf8("Music") + "=" + SlothPlacesBox::getMusicLocation();
    list << QObject::trUtf8("Pictures") + "=" + SlothPlacesBox::getPicturesLocation();
    list << QObject::trUtf8("Movies") + "=" + SlothPlacesBox::getMoviesLocation();
    return list;
}

QStringList SlothSettings::getPlaces() {
    QSettings settings;
    QStringList list;
    settings.beginGroup("Places");

    QStringList names = settings.allKeys();

    if(names.isEmpty())
        return defaultPlaces();

    foreach(QString name, names) {
        list << name + "=" + settings.value(name).toString();
    }
    settings.endGroup();

    return list;
}

void SlothSettings::setPlaces(const QStringList &list) {
    QSettings settings;
    settings.beginGroup("Places");
    foreach (QString name, list) {
        QStringList l = name.split("=");
        settings.setValue(l[0], l[1]);
    }
    settings.endGroup();
}
