#ifndef SLOTHSETTINGS_H
#define SLOTHSETTINGS_H

#include <QtWidgets>

class SlothSettings
{
public:
    SlothSettings();

    static void firstTimeRunning();
    static void loadWindowValues(QMainWindow *mw);
    static void saveWindowValues(QMainWindow *mw);

    static void loadListViewValues(QListView *view);
    static void saveListViewValues(QListView *view);

    static QSize getDefaultSize();
    static QPoint getDefaultPosition();
};

#endif // SLOTHSETTINGS_H
