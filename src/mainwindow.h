#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "slothitems/slothlistview.h"
#include "slothitems/slothnavigationbar.h"
#include "slothitems/slothtabwidget.h"
#include "slothpanels/slothinfopanel.h"
#include "slothpanels/slothplacespanel.h"
#include "slothsettings.h"
#include "slothitems/slothfileeditor.h"
#include "slothitems/slothimageviewer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QWidget *mainWidget;
    QGridLayout *gridLayout;

    //Settings
    SlothSettings *settings;

    //TabWidget
    SlothTabWidget *tabWidget;

    //NavigationBar:
    SlothNavigationBar *navbar;

    //items:
    QToolBar *toolbar;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QAction *actBack;
    QAction *actForward;
    QAction *actUp;
    QAction *actChangeModel;

    //Panels:
    QDockWidget *dockRight;
    SlothInfoPanel *pnlInfo;

    QDockWidget *dockLeft;
    SlothPlacesPanel *pnlPlaces;


    //ui:
    void loadWindow();
    void loadMenuBar();
    void loadToolbar();
    void loadTabWidget();
    void loadPanels();

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    SlothListView *currentSlothListView();
    SlothFileEditor *currentSlothFileEditor();
    SlothImageViewer *currentSlothImageViewer();

private slots:
    //toolbar:
    void goBack();
    void goForward();
    void goUp();

    //update:
    void handleCurrentPathChange(const QString &path);
    void handleCurrentTabChange(int index);
    void handleTabCloseRequest(int index);
    void handleNewTabRequest(const QString &path);
    void handleCustomContextMenu(const QPoint &pos);
    void closeCurrentTab();
    void handleStatusChange(const QString &status);
    void handleImageViewerPathChange(const QString &filePath);
    void setEnabledWidgets(bool enabled);

    //other:
    void openDir(const QString &path);
    QString getCurrentDir();
    QString getCurrentIndexFile();
    void changeModel();

    //open:
    void openNewTextEdit(const QString &filePath);
    void openNewImageViewer(const QString &filePath);

    void addTab(const QString &path = QDir::homePath());


protected:
    void closeEvent(QCloseEvent *event);

};

#endif // MAINWINDOW_H
