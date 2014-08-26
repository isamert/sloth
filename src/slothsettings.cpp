#include "slothsettings.h"

SlothSettings::SlothSettings()
{

}

void SlothSettings::firstTimeRunning() {
    QSettings settings;
    settings.beginGroup("SlothFileManager");
    if(settings.value("firstTime", true).toBool()) {
        settings.setValue("firstTime", false);
        settings.setValue("size", getDefaultSize());
        settings.setValue("pos", getDefaultPosition());
        settings.setValue("fullscreen", false);
    }
    settings.endGroup();
}

QSize SlothSettings::getDefaultSize() {
    return QSize(720, 450);
}

QPoint SlothSettings::getDefaultPosition() {
    int width = getDefaultSize().width();
    int height = getDefaultSize().height();

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
