#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->loadWindow();
    this->loadTab();
    this->loadToolbar();
    this->loadMenuBar();
    this->loadPanels();

    this->addTab();
}

MainWindow::~MainWindow() {}

void MainWindow::loadWindow() {
    this->mainWidget = new QWidget(this);
    this->gridLayout = new QGridLayout(this->mainWidget);
    this->mainWidget->setLayout(this->gridLayout);
    this->gridLayout->setSpacing(0);
    this->gridLayout->setMargin(0);
    this->setCentralWidget(mainWidget);

    this->actChangeModel = new QAction(this);
    this->actChangeModel->setText(trUtf8("Change Navigation Model"));
    connect(this->actChangeModel, SIGNAL(triggered()), this, SLOT(changeModel()));

    this->statusbar = new QStatusBar();
    this->setStatusBar(this->statusbar);

    SlothSettings::firstTimeRunning();
    SlothSettings::loadWindowValues(this);
}

void MainWindow::loadToolbar() {
    this->toolbar = new QToolBar();
    this->toolbar->setMovable(false);
    this->toolbar->setContextMenuPolicy(Qt::CustomContextMenu);
    this->addToolBar(Qt::TopToolBarArea, toolbar);
    this->setUnifiedTitleAndToolBarOnMac(true);

    this->actBack = new QAction(this->toolbar);
    this->actForward = new QAction(this->toolbar);
    this->actBack->setIcon(Quick::getIcon("back"));
    this->actForward->setIcon(Quick::getIcon("forward"));

    this->toolbar->addAction(this->actBack);
    this->toolbar->addAction(this->actForward);

    this->toolbar->addSeparator();

    this->navbar = new SlothNavigationBar(this->toolbar, QDir::homePath(), false);
    this->toolbar->addWidget(this->navbar);

    connect(this->actBack, SIGNAL(triggered()), this, SLOT(goBack()));
    connect(this->actForward, SIGNAL(triggered()), this, SLOT(goForward()));
    connect(this->navbar, SIGNAL(barItemClicked(QString)), this, SLOT(openDir(QString)));
    connect(this->toolbar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(handleCustomContextMenu(QPoint)));
}

void MainWindow::loadMenuBar() {
    this->menubar = new QMenuBar(this);
    this->setMenuBar(this->menubar);
}

void MainWindow::loadPanels() {
    this->dockLeft = new QDockWidget(trUtf8("Places"), this);

    this->addDockWidget(Qt::RightDockWidgetArea, this->dockLeft);

    this->pnlPlaces = new SlothPlacesPanel(this->dockLeft);
    this->dockLeft->setWidget(this->pnlPlaces);

    this->dockRight = new QDockWidget(trUtf8("Info"), this);
    this->addDockWidget(Qt::RightDockWidgetArea, this->dockRight);

    this->pnlInfo = new SlothInfoPanel(this->dockRight);
    this->dockRight->setWidget(this->pnlInfo);
}

void MainWindow::loadTab() {
    this->tabWidget = new SlothTabWidget(this->centralWidget());
    this->gridLayout->addWidget(this->tabWidget, 0, 0, 1, 1);
    connect(this->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleCurrentTabChange(int)));
}

SlothListView* MainWindow::currentListView() {
    return dynamic_cast<SlothListView*>(this->tabWidget->currentWidget());
}

void MainWindow::goBack() {
    this->currentListView()->goBack();
}

void MainWindow::goForward() {
    this->currentListView()->goForward();
}

void MainWindow::openDir(const QString &path) {
    this->currentListView()->openDir(path);
}

QString MainWindow::getCurrentDir() {
    return this->currentListView()->getCurrentDir();
}

QString MainWindow::getCurrentIndexFile() {
    QString fileName = this->currentListView()->selectionModel()->currentIndex().data().toString();
    return FileUtils::combine(this->getCurrentDir(), fileName);
}

void MainWindow::changeModel() {
    this->navbar->changeModel();
}

void MainWindow::addTab(const QString &path /* = QDir::homePath() */) {
    //TODO: get focus policy from SlothSettings(focus directly to new tab or not)
    SlothListView *tabitem = new SlothListView(this, path);

    this->tabWidget->addTab(tabitem, QDir(path).dirName());
    this->tabWidget->setCurrentIndex(this->tabWidget->count() - 1);
    this->handleCurrentPathChange(path);

    connect(tabitem, SIGNAL(currentPathChanged(QString)), this, SLOT(handleCurrentPathChange(QString)));
    connect(tabitem, SIGNAL(newTabRequested(QString)), this, SLOT(handleNewTabRequest(QString)));
    connect(tabitem, SIGNAL(newEmptyTabRequested()), this, SLOT(addTab()));
    connect(tabitem, SIGNAL(tabCloseRequested()), this, SLOT(closeCurrentTab()));
    connect(tabitem, SIGNAL(currentStatusChanged(QString)), this, SLOT(handleStatusChange(QString)));
}

void MainWindow::handleCurrentPathChange(const QString &path) {
    this->tabWidget->setTabText(this->tabWidget->currentIndex(), QDir(path).dirName());
    this->navbar->setPath(path);
    this->pnlInfo->setInfo(path);
}

void MainWindow::handleCurrentTabChange(int index) {
    QString currPath = dynamic_cast<SlothListView*>(this->tabWidget->widget(index))->getCurrentDir();
    this->navbar->setPath(currPath);
    this->pnlInfo->setInfo(currPath);
}

void MainWindow::handleNewTabRequest(const QString &path) {
    this->addTab(path);
}

void MainWindow::handleCustomContextMenu(const QPoint &pos) {
    QMenu menu;
    menu.addAction(this->actChangeModel);
    menu.exec(this->mapToGlobal(pos));
}

void MainWindow::closeCurrentTab() {
    //handles tabCloseRequested signal
    //this signal can only comes from current tab
    //TODO: close
}

void MainWindow::handleStatusChange(const QString &status) {
    this->statusBar()->showMessage(status);

    //PANEL:
    if(status == "") //nothing selected
        this->pnlInfo->setInfo(this->getCurrentDir());
    else
        this->pnlInfo->setInfo(this->getCurrentIndexFile());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    SlothSettings::saveWindowValues(this);
    event->accept();
}
