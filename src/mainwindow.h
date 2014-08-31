#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "slothwidgets/slothlistview.h"
#include "slothwidgets/slothnavigationbar.h"
#include "slothwidgets/slothtabwidget.h"
#include "slothpanels/slothinfopanel.h"
#include "slothpanels/slothplacespanel.h"
#include "slothsettings.h"
#include "slothapps//slothfileeditor.h"
#include "slothapps/slothimageviewer.h"

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
    QToolBar *filterbar;
    QLineEdit *lineFilter;
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
    void loadFilterBar();

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
    void onFilterChange();
    void setEnabledWidgets(bool enabled);
    void showFilterBar();

    //other:
    void openDir(const QString &path);
    QString getCurrentDir();
    QString getCurrentIndexFile();
    void changeModel();

    //open:
    void openNewTextEdit(const QString &filePath);
    void openNewImageViewer(const QString &filePath);

    void openNewListView(const QString &path = QDir::homePath());


protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *event);

};

#endif // MAINWINDOW_H
