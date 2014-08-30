#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->loadWindow();
    this->loadTabWidget();
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
    this->actUp = new QAction(this->toolbar);

    this->actBack->setIcon(Quick::getIcon("back"));
    this->actForward->setIcon(Quick::getIcon("forward"));
    this->actUp->setIcon(Quick::getIcon("up"));

    this->toolbar->addAction(this->actBack);
    this->toolbar->addAction(this->actForward);
    this->toolbar->addAction(this->actUp);

    this->toolbar->addSeparator();

    this->navbar = new SlothNavigationBar(this->toolbar, QDir::homePath(), false);
    this->toolbar->addWidget(this->navbar);

    connect(this->actBack, SIGNAL(triggered()), this, SLOT(goBack()));
    connect(this->actForward, SIGNAL(triggered()), this, SLOT(goForward()));
    connect(this->actUp, SIGNAL(triggered()), this, SLOT(goUp()));
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

    this->tabifyDockWidget(this->dockRight, this->dockLeft);

    connect(this->pnlPlaces, SIGNAL(itemClicked(QString)), this, SLOT(openDir(QString)));
}

void MainWindow::loadTabWidget() {
    this->tabWidget = new SlothTabWidget(this->centralWidget());
    this->gridLayout->addWidget(this->tabWidget, 0, 0, 1, 1);
    connect(this->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleCurrentTabChange(int)));
    connect(this->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(handleTabCloseRequest(int)));
}

SlothListView *MainWindow::currentSlothListView() {
    return dynamic_cast<SlothListView*>(this->tabWidget->currentWidget());
}

SlothFileEditor *MainWindow::currentSlothFileEditor() {
    return dynamic_cast<SlothFileEditor*>(this->tabWidget->currentWidget());
}

SlothImageViewer *MainWindow::currentSlothImageViewer() {
    return dynamic_cast<SlothImageViewer*>(this->tabWidget->currentWidget());
}


void MainWindow::goBack() {
    this->currentSlothListView()->goBack();
}

void MainWindow::goForward() {
    this->currentSlothListView()->goForward();
}

void MainWindow::goUp() {
    this->currentSlothListView()->goUp();
}

void MainWindow::openDir(const QString &path) {
    this->currentSlothListView()->openDir(path);
}

QString MainWindow::getCurrentDir() {
    return this->currentSlothListView()->getCurrentDir();
}

QString MainWindow::getCurrentIndexFile() {
    return this->currentSlothListView()->getCurrentSelectedPath();
}

void MainWindow::changeModel() {
    this->navbar->changeModel();
}

void MainWindow::addTab(const QString &path /* = QDir::homePath() */) {
    //TODO: get focus policy from SlothSettings(focus directly to new tab or not)
    SlothListView *slv = new SlothListView(this->tabWidget, path);
    slv->setObjectName("__SLOTHLISTVIEW__");

    int index = this->tabWidget->currentIndex() + 1;
    this->tabWidget->insertTab(index, slv, QDir(path).dirName());
    this->tabWidget->setCurrentIndex(index);
    this->handleCurrentPathChange(path);

    connect(slv, SIGNAL(currentPathChanged(QString)), this, SLOT(handleCurrentPathChange(QString)));
    connect(slv, SIGNAL(newTabRequested(QString)), this, SLOT(handleNewTabRequest(QString)));
    connect(slv, SIGNAL(newEmptyTabRequested()), this, SLOT(addTab()));
    connect(slv, SIGNAL(tabCloseRequested()), this, SLOT(closeCurrentTab()));
    connect(slv, SIGNAL(currentStatusChanged(QString)), this, SLOT(handleStatusChange(QString)));

    connect(slv, SIGNAL(textEditRequested(QString)), this, SLOT(openNewTextEdit(QString)));
    connect(slv, SIGNAL(imageViewerRequested(QString)), this, SLOT(openNewImageViewer(QString)));
}

void MainWindow::handleCurrentPathChange(const QString &path) {
    this->tabWidget->setTabText(this->tabWidget->currentIndex(), QDir(path).dirName());
    this->navbar->setPath(path);
    this->pnlInfo->setInfo(path);
    this->pnlPlaces->setCurrentItem(path);
}

void MainWindow::handleCurrentTabChange(int index) {
    QString objname = this->tabWidget->currentWidget()->objectName();
    QString currPath = "";

    if(objname == "__SLOTHLISTVIEW__") {
        currPath = this->currentSlothListView()->getCurrentDir();
        this->setEnabledWidgets(true);
    }
    else {
        if(objname == "__SLOTHFILEEDITOR__")
            currPath = this->currentSlothFileEditor()->getCurrentFile();
        else if(objname == "__SLOTHIMAGEVIEWER__")
            currPath = this->currentSlothImageViewer()->getCurrentFile();

        this->setEnabledWidgets(false);
    }

    this->navbar->setPath(currPath);
    this->pnlInfo->setInfo(currPath);
}

void MainWindow::setEnabledWidgets(bool enabled) {
    this->toolbar->setEnabled(enabled);
    this->pnlPlaces->setEnabled(enabled);

    /*
    this->navbar->setEnabled(enabled);
    this->actBack->setEnabled(enabled);
    this->actForward->setEnabled(enabled);
    this->actUp->setEnabled(enabled);
    */
}

void MainWindow::handleTabCloseRequest(int index) {
    if(this->tabWidget->count() > 1)
        delete this->tabWidget->widget(index);
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
    if(this->tabWidget->count() > 1)
        delete this->tabWidget->currentWidget();
}

void MainWindow::handleStatusChange(const QString &status) {
    this->statusBar()->showMessage(status);

    //PANEL:
    if(status == "") //nothing selected
        this->pnlInfo->setInfo(this->getCurrentDir());
    else
        this->pnlInfo->setInfo(this->getCurrentIndexFile());
}

void MainWindow::handleImageViewerPathChange(const QString &filePath) {
    this->tabWidget->setTabText(this->tabWidget->currentIndex(), FileUtils::getName(filePath));
    this->navbar->setPath(filePath);
    this->pnlInfo->setInfo(filePath);
}

void MainWindow::openNewTextEdit(const QString &filePath) {
    SlothFileEditor *sfe = new SlothFileEditor(this->tabWidget);
    sfe->setObjectName("__SLOTHFILEEDITOR__");
    sfe->setFile(filePath);

    int index = this->tabWidget->currentIndex() + 1;
    this->tabWidget->insertTab(index, sfe, FileUtils::getName(filePath));
    this->tabWidget->setCurrentIndex(index);
}

void MainWindow::openNewImageViewer(const QString &filePath) {
    SlothImageViewer *siv = new SlothImageViewer(this->tabWidget);
    siv->setObjectName("__SLOTHIMAGEVIEWER__");
    siv->setFile(filePath);

    int index = this->tabWidget->currentIndex() + 1;
    this->tabWidget->insertTab(index, siv, FileUtils::getName(filePath));
    this->tabWidget->setCurrentIndex(index);

    connect(siv, SIGNAL(currentFileChanged(QString)), this, SLOT(handleImageViewerPathChange(QString)));
}

void MainWindow::closeEvent(QCloseEvent *event) {
    SlothSettings::saveWindowValues(this);
    event->accept();
}
