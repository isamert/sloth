#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->loadWindow();
    this->loadTabWidget();
    this->loadFilterBar();
    this->loadMiniTermBar();
    this->loadToolbar();
    this->loadMenuBar();
    this->loadPanels();

    this->openNewListView();
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

void MainWindow::loadTabWidget() {
    this->tabWidget = new SlothTabWidget(this->centralWidget());
    this->gridLayout->addWidget(this->tabWidget, 1, 0, 1, 1);
    connect(this->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleCurrentTabChange(int)));
    connect(this->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(handleTabCloseRequest(int)));
}

void MainWindow::loadToolbar() {
    this->toolbar = new QToolBar(trUtf8("Toolbar"), this);
    this->toolbar->setMovable(false);
    this->toolbar->setContextMenuPolicy(Qt::CustomContextMenu);
    //this->gridLayout->addWidget(this->toolbar, 0, 0, 1, 1);
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

    QMenu *menuViewMode;
    menuViewMode = new QMenu();
    menuViewMode->addAction(Quick::getIcon("view-list-details"), trUtf8("List View"),
                            this, SLOT(setViewModeToListMode()));
    menuViewMode->addAction(Quick::getIcon("view-list-icons"), trUtf8("Icon View"),
                            this, SLOT(setViewModeToIconMode()));
    menuViewMode->addSeparator();
    menuViewMode->addAction(this->actChangeModel);

    QMenu *menuShortcuts;
    menuShortcuts = new QMenu();
    menuShortcuts->addAction(Quick::getIcon("terminal"), trUtf8("Mini terminal"),
                             this->minitermbar, SLOT(showOrHide()));
    menuShortcuts->addAction(Quick::getIcon("find"), trUtf8("Filter items"),
                             this, SLOT(showFilterBar()));

    QToolButton *tbViewMode;
    tbViewMode = new QToolButton();
    tbViewMode->setFocusPolicy(Qt::NoFocus);
    tbViewMode->setMenu(menuViewMode);
    tbViewMode->setPopupMode(QToolButton::InstantPopup);
    tbViewMode->setIcon(Quick::getIcon("document-properties"));

    QToolButton *tbShortcuts;
    tbShortcuts = new QToolButton();
    tbShortcuts->setFocusPolicy(Qt::NoFocus);
    tbShortcuts->setMenu(menuShortcuts);
    tbShortcuts->setPopupMode(QToolButton::InstantPopup);
    tbShortcuts->setIcon(Quick::getIcon("document-properties"));

    QWidgetAction *waViewMode = new QWidgetAction(this->toolbar);
    waViewMode->setDefaultWidget(tbViewMode);

    QWidgetAction *waShortcuts = new QWidgetAction(this->toolbar);
    waShortcuts->setDefaultWidget(tbShortcuts);

    this->toolbar->addAction(waViewMode);
    this->toolbar->addAction(waShortcuts);

    connect(this->actBack, SIGNAL(triggered()), this, SLOT(goBack()));
    connect(this->actForward, SIGNAL(triggered()), this, SLOT(goForward()));
    connect(this->actUp, SIGNAL(triggered()), this, SLOT(goUp()));
    connect(this->navbar, SIGNAL(openPathRequested(QString)), this, SLOT(openDir(QString)));
    connect(this->navbar, SIGNAL(openPathInNewTabRequested(QString)), this, SLOT(openNewListView(QString)));
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

void MainWindow::loadFilterBar() {
    this->filterbar = new QToolBar(trUtf8("Filter Files/Folders"), this);
    this->gridLayout->addWidget(this->filterbar, 2, 0, 1, 1);
    this->filterbar->hide();

    this->lineFilter = new QLineEdit(this->filterbar);
    this->lineFilter->setPlaceholderText(trUtf8("(Press Enter to filter)"));
    this->lineFilter->installEventFilter(this);
    this->filterbar->addWidget(this->lineFilter);

    this->filterbar->addAction(Quick::getIcon("exit"), trUtf8("Close"), this->filterbar, SLOT(hide()));

    connect(lineFilter, SIGNAL(textChanged(QString)), this, SLOT(onFilterChange()));
}

void MainWindow::loadMiniTermBar() {
    this->minitermbar = new SlothMinitermWidget(this);
    this->gridLayout->addWidget(this->minitermbar, 3, 0, 1, 1);
    this->minitermbar->hide();

    connect(this->minitermbar, SIGNAL(openDirRequested(QString)), this, SLOT(openDir(QString)));
}

bool MainWindow::eventFilter(QObject* obj, QEvent *event) {
    //TODO:(maybe) take every event from slothlistview to here and add treeview, detailview etc...
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        if (obj == this->lineFilter &&
                (keyEvent->key() == Qt::Key_Escape ||
                keyEvent->matches(QKeySequence::Find))) {

            this->showFilterBar();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
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

void MainWindow::setViewModeToListMode() {
    this->currentSlothListView()->setViewModeToListMode();
}

void MainWindow::setViewModeToIconMode() {
    this->currentSlothListView()->setViewModeToIconMode();
}

void MainWindow::changeModel() {
    this->navbar->changeModel();
}

void MainWindow::openNewListView(const QString &path /* = QDir::homePath() */) {
    //TODO: get focus policy from SlothSettings(focus directly to new tab or not)
    SlothListView *slv = new SlothListView(this->tabWidget, path);
    slv->setObjectName("__SLOTHLISTVIEW__");

    int index = this->tabWidget->currentIndex() + 1;
    this->tabWidget->insertTab(index, slv, QDir(path).dirName());
    this->tabWidget->setCurrentIndex(index);
    this->handleCurrentPathChange(path);

    connect(slv, SIGNAL(currentPathChanged(QString)), this, SLOT(handleCurrentPathChange(QString)));
    connect(slv, SIGNAL(newTabRequested(QString)), this, SLOT(handleNewTabRequest(QString)));
    connect(slv, SIGNAL(newEmptyTabRequested()), this, SLOT(openNewListView()));
    connect(slv, SIGNAL(tabCloseRequested()), this, SLOT(closeCurrentTab()));
    connect(slv, SIGNAL(currentStatusChanged(QString)), this, SLOT(handleStatusChange(QString)));

    connect(slv, SIGNAL(textEditRequested(QString)), this, SLOT(openNewTextEdit(QString)));
    connect(slv, SIGNAL(imageViewerRequested(QString)), this, SLOT(openNewImageViewer(QString)));
    connect(slv, SIGNAL(filterRequested()), this, SLOT(showFilterBar()));
}

void MainWindow::handleCurrentPathChange(const QString &path) {
    this->tabWidget->setTabText(this->tabWidget->currentIndex(), QDir(path).dirName());
    this->navbar->setPath(path);
    this->pnlInfo->setInfo(path);
    this->pnlPlaces->setCurrentItem(path);
    this->minitermbar->setWorkingDirectory(path);
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
    this->minitermbar->setWorkingDirectory(currPath);
}

void MainWindow::onFilterChange() {
    QStringList items = this->lineFilter->text().split("|");
    for(int i = 0; i < items.count(); ++i) {
        items[i] += "*";
        items[i].insert(0, "*");
    }

    this->currentSlothListView()->filterItems(items);
}

void MainWindow::setEnabledWidgets(bool enabled) {
    this->toolbar->setEnabled(enabled);
    this->pnlPlaces->setEnabled(enabled);
    this->filterbar->setEnabled(enabled);
    this->minitermbar->setEnabled(enabled);

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
    this->openNewListView(path);
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

void MainWindow::showFilterBar() {
    this->filterbar->setVisible(!this->filterbar->isVisible());

    if(this->filterbar->isVisible())
        this->lineFilter->setFocus();
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
