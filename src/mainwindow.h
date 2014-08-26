#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "slothitems/slothlistview.h"
#include "slothitems/slothnavigationbar.h"
#include "slothitems/slothtabwidget.h"
#include "slothsettings.h"

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

    //Toolbar items:
    QToolBar *toolbar;
    QStatusBar *statusbar;
    QAction *actBack;
    QAction *actForward;
    QAction *actChangeModel;


    //ui:
    void loadWindow();
    void loadToolbar();
    void loadTab();

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //toolbar:
    void goBack();
    void goForward();

    //update:
    void handleCurrentPathChange(const QString &path);
    void handleCurrentTabChange(int index);
    void handleNewTabRequest(const QString &path);
    void handleCustomContextMenu(const QPoint &pos);
    void closeCurrentTab();
    void handleStatusChange(const QString &status);

    //other:
    void openDir(const QString &path);
    void changeModel();

    void addTab(const QString &path = QDir::homePath());


protected:
    void closeEvent(QCloseEvent *event);

};

#endif // MAINWINDOW_H
