#include "slothlistview.h"

SlothListView::SlothListView(QWidget *parent, const QString &path /* = QDir::homePath() */) :
    QListView(parent)
{
    this->loadSettings();
    this->loadActions();

    this->historyCurrent = -1;

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));
    //connect(this->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentIndexChange(QModelIndex,QModelIndex)));
    connect(this->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChange(QItemSelection,QItemSelection)));
    connect(this->fsm, SIGNAL(rootPathChanged(QString)), this, SLOT(onRootPathChanged(QString)));

    this->openDir(path);
}

void SlothListView::loadSettings() {
    //this:
    SlothSettings::loadListViewValues(this);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setUniformItemSizes(true);
    this->setResizeMode(QListView::Adjust);
    this->setSelectionRectVisible(true);
    this->setWrapping(true);

    //fsm:
    this->fsm = new QFileSystemModel(this);
    this->fsm->setFilter(QDir::AllDirs | QDir::AllEntries | QDir::NoDotAndDotDot);
    this->setModel(this->fsm);
}

void SlothListView::loadActions() {
    this->menuNewFile = new QMenu(this);
    this->menuNewFile->setTitle(trUtf8("New file"));
    this->newFileMapper = new QSignalMapper(this);
    connect(this->newFileMapper, SIGNAL(mapped(const QString &)), this,
            SLOT(newFileMenuItemClicked(const QString &)));

    this->actEmptyFile = new QAction(this);
    this->actEmptyFile->setText(trUtf8("Empty File"));
    this->menuNewFile->addAction(this->actEmptyFile);
    this->menuNewFile->addSeparator();
    this->loadNewFileMenu(SlothPlacesBox::getTemplatesLocation(), this->menuNewFile);

    this->actNewFolder = new QAction(trUtf8("New folder"), this);
    this->actCut = new QAction(trUtf8("Cut"), this);
    this->actOpenInNewTab = new QAction(trUtf8("Open in new tab"), this);
    this->actCut = new QAction(trUtf8("Cut"), this);
    this->actCopy= new QAction(trUtf8("Copy"), this);
    this->actPaste= new QAction(trUtf8("Paste"), this);
    this->actRename = new QAction(trUtf8("Rename"), this);
    this->actDelete = new QAction(trUtf8("Delete permanently"), this);

    this->actShowHidden = new QAction(trUtf8("Show Hidden Files"), this);
    this->actShowHidden->setCheckable(true);

    this->actMoveToTrash = new QAction(trUtf8("Move to trash"), this);
    this->actCompress = new QAction(trUtf8("Compress"), this);
    this->actSendTo = new QAction(trUtf8("Send to..."), this);
    this->actProperties = new QAction(trUtf8("Properties"), this);

    connect(this->actOpenInNewTab, SIGNAL(triggered()), this, SLOT(openInNewTab()));
    connect(this->actCut, SIGNAL(triggered()), this, SLOT(cut()));
    connect(this->actCopy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(this->actRename, SIGNAL(triggered()), this, SLOT(rename()));
    connect(this->actMoveToTrash, SIGNAL(triggered()), this, SLOT(moveToTrash()));
    connect(this->actDelete, SIGNAL(triggered()), this, SLOT(deletePermanently()));
    connect(this->actShowHidden, SIGNAL(triggered(bool)), this, SLOT(setShowHidden(bool)));
    connect(this->actCompress, SIGNAL(triggered()), this, SLOT(compress()));
    connect(this->actSendTo, SIGNAL(triggered()), this, SLOT(sendTo()));
    connect(this->actProperties, SIGNAL(triggered()), this, SLOT(properties()));

    connect(this->actEmptyFile, SIGNAL(triggered()), this, SLOT(newEmptyFile()));
    connect(this->actNewFolder, SIGNAL(triggered()), this, SLOT(newFolder()));
    connect(this->actPaste, SIGNAL(triggered()), this, SLOT(paste()));
}

void SlothListView::loadNewFileMenu(const QString &tempDir, QMenu *menu) {
    QDir dir(tempDir);

    foreach(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs |
                                              QDir::Files, QDir::DirsFirst)) {
        qDebug() << info.absoluteFilePath();
        if(info.isDir()) {
            this->loadNewFileMenu(info.absoluteFilePath(), this->menuNewFile->addMenu(info.fileName()));
        }
        else if(info.isFile()) {
            QIcon icon = this->fsm->iconProvider()->icon(info);
            QAction *act = menu->addAction(icon, info.baseName());
            connect(act, SIGNAL(triggered()), this->newFileMapper, SLOT(map()));
            act->setToolTip(info.absoluteFilePath());

            this->newFileMapper->setMapping(act, info.absoluteFilePath());
        }

    }
}

void SlothListView::newFileMenuItemClicked(const QString &path) {
    QString newPath = FileUtils::combine(this->getCurrentDir(), FileUtils::getName(path));
    if(!QFile::copy(path, newPath))
        Quick::msgWarning(trUtf8("Error"), trUtf8("Can not create new file from template."));
}

void SlothListView::openDir(QString dir, bool addHistory /* =true */) {
    this->setRootIndex(this->fsm->setRootPath(dir));
    if(addHistory) {
        this->historyCurrent += 1;
        this->history.insert(this->historyCurrent, dir);
    }
}

QString SlothListView::getCurrentDir() {
    QString path = this->fsm->rootPath();
    QFileInfo info(path);

    if(info.isDir())
        return path;
    else if(info.isFile())
        return info.absoluteDir().absolutePath();

    return NULL;
}

QString SlothListView::getCurrentTabName() {
    return FileUtils::getName(this->fsm->rootPath());
}

QStringList SlothListView::getCurrentSelectedPaths() {
    QStringList list;
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
    foreach(QModelIndex index, indexes) {
        list << FileUtils::combine(this->fsm->rootPath(), index.data(Qt::DisplayRole).toString());
        //list << this->fsm->fileInfo(index).absoluteFilePath().toUtf8();
        //WTF: //FIXME: this commented code has problems with utf-8 chars, but only here
    }
    return list;
}

bool SlothListView::goBack() {
    int count = this->history.count() - 1;
    if(count >= 1 && this->historyCurrent >= 1) {
        int index = this->historyCurrent - 1;
        this->historyCurrent = index;
        this->openDir(this->history[index], false);

        return (count >= 1 && this->historyCurrent >= 1);
    }
    return false;
}

bool SlothListView::goForward() {
    int count = this->history.count() - 1;
    if(count >= 0 && this->historyCurrent <= count - 1) {
        int index = this->historyCurrent + 1;
        this->historyCurrent = index;
        this->openDir(this->history[index], false);

        return (count >= 0 && this->historyCurrent <= count - 1);
    }
    return false;
}

void SlothListView::setShowHidden(bool enabled) {
    if(enabled)
        this->fsm->setFilter(QDir::AllDirs | QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    else
        this->fsm->setFilter(QDir::AllDirs | QDir::AllEntries | QDir::NoDotAndDotDot);
}

void SlothListView::onCurrentIndexChange(const QModelIndex &current, const QModelIndex &previous) {
    QString status = "";
    QFileInfo info = this->fsm->fileInfo(current);

    if(info.isFile())
        status = QString("%1 (%2) - %3").arg(info.fileName()).
                 arg(FileUtils::formatFileSize(info.size())).arg(this->fsm->type(current));
    else if(info.isDir())
        status = info.fileName() + " " + QString(trUtf8("(Contains %1 items)")).
                                         arg(QDir(info.absoluteFilePath()).entryList().count() - 2); //-2 because . and ..

    emit this->currentStatusChanged(status);
}

void SlothListView::onSelectionChange(const QItemSelection &selected, const QItemSelection &deselected) {
    QStringList mlist = this->getCurrentSelectedPaths();
    QString status = "";

    if(mlist.count() == 1) {
        this->onCurrentIndexChange(this->currentIndex(), this->currentIndex());
        return;
    }

    qint64 totalSize = 0;
    int totalCount = 0;
    int totalFile = 0;
    int totalDir = 0;

    foreach (QString index, mlist) {
        QFileInfo info(index);

        if(info.isFile()) {
            totalSize += info.size();
            totalFile += 1;
        }
        else if(info.isDir()) {
            totalDir += 1;
            totalCount += QDir(info.absoluteFilePath()).entryList().count() - 2;
        }
    }

    status = QString(trUtf8("%1 dirs (Contains %2 items), %3 files (%4)"))
             .arg(totalDir).arg(totalCount).arg(totalFile).arg(FileUtils::formatFileSize(totalSize));

    if(totalDir == 0 && totalFile == 0)
        status = "";

    emit this->currentStatusChanged(status);
}


void SlothListView::onDoubleClicked(const QModelIndex &index) {
    QString path = this->fsm->filePath(index);
    QFileInfo info(path);

    if(info.isDir()) {
        this->openDir(path);
    }
    else if(info.isFile()) {
        FileUtils::openDetached(path);
    }
}

void SlothListView::onRootPathChanged(const QString &path) {
    emit currentPathChanged(path);

}

void SlothListView::keyPressEvent(QKeyEvent *event) {
    if(event->matches(QKeySequence::Paste)) {
        this->paste();
    }
    else if(event->matches(QKeySequence::SelectAll)) {
        this->selectAll();
    }
    else if(event->key() == Qt::Key_Backspace) {
        this->goBack();
    }
    else if(event->key() == Qt::Key_W && event->modifiers() == Qt::CTRL) {
        emit this->tabCloseRequested();
    }
    else if(event->key() == Qt::Key_T && event->modifiers() == Qt::CTRL) {
        emit this->newEmptyTabRequested();
    }
    else if(event->key() == Qt::Key_Delete && event->modifiers() == Qt::SHIFT) {
        this->deletePermanently();
    }
    else if(event->key() == Qt::Key_H && event->modifiers() == Qt::CTRL) {
        this->actShowHidden->trigger();
    }
    else if(!event->text().isEmpty()){
        this->keyboardSearch(event->text());
    }

    if(!this->selectionModel()->selectedIndexes().isEmpty()) {
        if(event->key() == Qt::Key_Delete) {
            this->moveToTrash();
        }
        else if(event->key() == Qt::Key_F2) {
            this->rename();
        }
        else if(event->matches(QKeySequence::Copy)) {
            this->copy();
        }
        else if(event->matches(QKeySequence::Cut)) {
            this->cut();
        }
    }
}

void SlothListView::onCustomContextMenu(const QPoint &point) {
    QModelIndex index = this->indexAt(point);
    if (index.isValid()) { //when clicked on items
        //FIXME:
        QFileInfo info(this->fsm->fileInfo(index).absoluteFilePath());
        QMenu menu;

        if(info.isDir()) {
            menu.addAction(this->actOpenInNewTab);
            menu.addSeparator();
            menu.addAction(this->actCut);
            menu.addAction(this->actCopy);
            menu.addAction(this->actRename);
            menu.addSeparator();
            menu.addAction(this->actMoveToTrash);
            menu.addAction(this->actDelete);
            menu.addSeparator();
            menu.addAction(this->actCompress);
            menu.addAction(this->actSendTo);
            menu.addSeparator();
            menu.addAction(this->actProperties);
            menu.exec(this->viewport()->mapToGlobal(point));
        }
        else if(info.isFile()) {
            //TODO:menu.addAction(this->actOpenWith)
            menu.addAction(this->actCut);
            menu.addAction(this->actCopy);
            menu.addAction(this->actRename);
            menu.addSeparator();
            menu.addAction(this->actMoveToTrash);
            menu.addAction(this->actDelete);
            menu.addSeparator();
            menu.addAction(this->actCompress);
            menu.addAction(this->actSendTo);
            menu.addSeparator();
            menu.addAction(this->actProperties);
            menu.exec(this->viewport()->mapToGlobal(point));
        }
    }
    else { //when clicked on empty area
        QMenu menu;
        menu.addMenu(this->menuNewFile);
        menu.addAction(this->actNewFolder);
        menu.addSeparator();
        menu.addAction(this->actPaste);
        menu.addSeparator();
        menu.addAction(this->actShowHidden);
        menu.exec(this->viewport()->mapToGlobal(point));
    }
}

void SlothListView::openInNewTab() {
    foreach(QString path, this->getCurrentSelectedPaths())
        emit this->newTabRequested(path);
}

void SlothListView::cut() {
    qDebug() << "AAAA";
}

void SlothListView::copy() {
    QClipboard *clipboard = QApplication::clipboard();
    QString clip;
    foreach(QString path, this->getCurrentSelectedPaths()) {
        clip += path + '\n';
    }
    clip.remove(clip.count() - 1, 1); //remove last "\n"
    clipboard->setText(clip);
}

void SlothListView::rename() {
    foreach(QString path, this->getCurrentSelectedPaths()) {
        FileUtils::rename(path);
    }
}

void SlothListView::moveToTrash() {
    foreach(QString path, this->getCurrentSelectedPaths())
        FileUtils::moveToTrash(path);
}

void SlothListView::deletePermanently() {
    if(!this->getCurrentSelectedPaths().isEmpty()) {
        bool reply = Quick::msgYesNo(trUtf8("Delete file or folder"),
                                     trUtf8("Are you sure you want to delete this file?"
                                            " The item will be deleted permanently"));
        if(reply) {
            foreach(QString path, this->getCurrentSelectedPaths())
                FileUtils::removeRecursively(path);
        }
    }
}

void SlothListView::compress() {
    this->compDialog = new CompressionDialog(this);
    this->compDialog->setItems(this->getCurrentSelectedPaths());
    this->compDialog->show();
}

void SlothListView::sendTo() {
    qDebug() << "AAAA";
}

void SlothListView::properties() {
    this->infoDialog = new SlothInfoPanel();
    this->infoDialog->setInfo(this->getCurrentSelectedPaths());
    this->infoDialog->show();
}

void SlothListView::newEmptyFile() {
    bool ok;
    QString newFileName = FileUtils::getFileNameDoesNotExists(
                          FileUtils::combine(this->getCurrentDir(), trUtf8("New File")), " ", 1);

    QString fileName = Quick::getText(trUtf8("New File"),
                                     trUtf8("Enter new file's name:"),
                                     &ok, newFileName);
    if(ok) {
        QFile file(FileUtils::combine(this->getCurrentDir(), fileName));

        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            Quick::msgWarning(trUtf8("Error"), trUtf8("Cannot write to the file."));
            return;
        }

        QTextStream out(&file);
        out << "";
        file.close();
    }
}

void SlothListView::newFolder() {
    bool ok;
    QDir dir(this->getCurrentDir());
    QString newDirName = FileUtils::getFileNameDoesNotExists(
                        dir.absoluteFilePath(trUtf8("New Folder")), " ", 1);

    QString dirName = Quick::getText(trUtf8("New Folder"),
                                     trUtf8("Enter new folder's name:"),
                                     &ok, newDirName);
    if(ok) {
        if(!dir.mkdir(dirName))
            Quick::msgWarning(trUtf8("Error"), trUtf8("Cannot create new folder."));
    }
}

void SlothListView::paste() {
    Clipboard::paste(this->getCurrentDir());
}
