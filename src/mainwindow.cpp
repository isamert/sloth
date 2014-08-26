#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->loadWindow();
    this->loadTab();
    this->loadToolbar();

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

    this->navbar = new SlothNavigationBar(this->toolbar, QDir::homePath(), true);
    this->toolbar->addWidget(this->navbar);

    connect(this->actBack, SIGNAL(triggered()), this, SLOT(goBack()));
    connect(this->actForward, SIGNAL(triggered()), this, SLOT(goForward()));
    connect(this->navbar, SIGNAL(barItemClicked(QString)), this, SLOT(openDir(QString)));
    connect(this->toolbar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(handleCustomContextMenu(QPoint)));
}

void MainWindow::loadTab() {
    this->tabWidget = new SlothTabWidget(this->centralWidget());
    this->gridLayout->addWidget(this->tabWidget, 0, 0, 1, 1);
    connect(this->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleCurrentTabChange(int)));
}

void MainWindow::goBack() {
    dynamic_cast<SlothListView*>(this->tabWidget->currentWidget())->goBack();
}

void MainWindow::goForward() {
    dynamic_cast<SlothListView*>(this->tabWidget->currentWidget())->goForward();
}

void MainWindow::openDir(const QString &path) {
    dynamic_cast<SlothListView*>(this->tabWidget->currentWidget())->openDir(path);
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
    this->navbar->setPath(path);
    this->tabWidget->setTabText(this->tabWidget->currentIndex(), QDir(path).dirName());
}

void MainWindow::handleCurrentTabChange(int index) {
    QString currPath = dynamic_cast<SlothListView*>(this->tabWidget->widget(index))->getCurrentDir();
    this->navbar->setPath(currPath);
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
}

void MainWindow::closeEvent(QCloseEvent *event) {
    SlothSettings::saveWindowValues(this);
    event->accept();
}
