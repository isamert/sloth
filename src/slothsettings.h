#ifndef SLOTHSETTINGS_H
#define SLOTHSETTINGS_H

#include "qglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "slothitems/slothplacesbox.h"

class SlothSettings
{

public:
    SlothSettings();

    static void firstTimeRunning();
    static void loadWindowValues(QMainWindow *mw);
    static void saveWindowValues(QMainWindow *mw);

    static void loadListViewValues(QListView *view);
    static void saveListViewValues(QListView *view);

    static QSize defaultSize();
    static QPoint defaultPosition();

    static QStringList defaultPlaces();
    static QStringList getPlaces();
    static void setPlaces(const QStringList &list);
};

#endif // SLOTHSETTINGS_H
