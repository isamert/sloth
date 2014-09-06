#include "slothlistview.h"

SlothListView::SlothListView(QWidget *parent, const QString &path /* = QDir::homePath() */) :
    QListView(parent)
{
    this->loadSettings();
    this->loadActions();

    this->historyCurrent = -1;
    this->currentSelectedTotalDirs = 0;
    this->currentSelectedTotalFiles = 0;
    this->currentSelectedTotalSize = 0;
    this->currentSelectedTotalSubFiles = 0;


    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));
    //connect(this->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentIndexChange(QModelIndex,QModelIndex)));
    connect(this->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChange(QItemSelection,QItemSelection)));
    connect(this->sfsm, SIGNAL(rootPathChanged(QString)), this, SLOT(onRootPathChanged(QString)));

    this->openDir(path);

    this->clipboard = new Clipboard(this);
}

void SlothListView::loadSettings() {
    //this:
    SlothSettings::loadListViewValues(this);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    //this->setUniformItemSizes(true);
    this->setLayoutMode(QListView::Batched);
    this->setResizeMode(QListView::Adjust);
    //this->setSelectionRectVisible(true);
    //this->setFlow(QListView::LeftToRight);
    this->setWrapping(true);

    //sfsm:
    this->sfsm = new SlothFileSystemModel(this);
    this->sfsm->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    this->sfsm->setNameFilterDisables(false);
    this->sfsm->setIconSize(this->iconSize());
    this->setModel(this->sfsm);
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
    this->actExtract = new QAction(trUtf8("Extract here"), this);
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
    connect(this->actExtract, SIGNAL(triggered()), this, SLOT(extract()));
    connect(this->actSendTo, SIGNAL(triggered()), this, SLOT(sendTo()));
    connect(this->actProperties, SIGNAL(triggered()), this, SLOT(properties()));

    connect(this->actEmptyFile, SIGNAL(triggered()), this, SLOT(newEmptyFile()));
    connect(this->actNewFolder, SIGNAL(triggered()), this, SLOT(newFolder()));
    connect(this->actPaste, SIGNAL(triggered()), this, SLOT(paste()));
}

void SlothListView::loadNewFileMenu(const QString &tempDir, QMenu *menu) {
    QDir dir(tempDir);

    foreach(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries, QDir::DirsFirst)) {
        if(info.isDir()) {
            this->loadNewFileMenu(info.absoluteFilePath(), this->menuNewFile->addMenu(info.fileName()));
        }
        else if(info.isFile()) {
            QIcon icon = this->sfsm->iconProvider()->icon(info);
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

void SlothListView::setViewModeToIconMode() {
    this->setViewMode(QListView::IconMode);
}

void SlothListView::setViewModeToListMode() {
    //FIXME:
    this->setViewMode(QListView::ListMode);
}

void SlothListView::openDir(const QString &dir, bool addHistory /* = true */) {
    QModelIndex mi = this->sfsm->setRootPath(dir);
    this->setRootIndex(mi);

    if(addHistory) {
        this->historyCurrent += 1;
        this->history.insert(this->historyCurrent, dir);
    }
}

void SlothListView::filterItems(const QString &filter) {
    this->filterItems(QStringList(filter));
}

void SlothListView::filterItems(const QStringList &filter) {
    if(filter.isEmpty()) //FIXME: is there another way to clean filters?
        this->sfsm->setNameFilters(QStringList("*"));
    else {
        if(filter[0].isEmpty())
            this->sfsm->setNameFilters(QStringList("*"));
        else
            this->sfsm->setNameFilters(QStringList(filter));
    }
}

QString SlothListView::getCurrentDir() {
    return this->sfsm->rootPath();
}

QString SlothListView::getCurrentTabName() {
    return FileUtils::getName(this->sfsm->rootPath());
}

QString SlothListView::getCurrentSelectedPath() {
    QModelIndex index = this->selectionModel()->currentIndex();
    return this->sfsm->fileInfo(index).absoluteFilePath();
}

QStringList SlothListView::getCurrentSelectedPaths() {
    QStringList list;
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
    foreach(const QModelIndex index, indexes) {
        if(index.column() == 0) {
            list << this->sfsm->fileInfo(index).absoluteFilePath();
        }
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

void SlothListView::goUp() {
    this->openDir(FileUtils::getUpperPath(this->getCurrentDir()));
}

void SlothListView::setShowHidden(bool enabled) {
    if(enabled)
        this->sfsm->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    else
        this->sfsm->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
}

void SlothListView::onCurrentIndexChange(const QModelIndex &current, const QModelIndex &previous) {
    QString status = "";
    QFileInfo info = this->sfsm->fileInfo(current);

    if(info.isFile())
        status = QString("%1 (%2) - %3").arg(info.fileName()).
                 arg(FileUtils::formatFileSize(info.size())).arg(this->sfsm->type(current));
    else if(info.isDir())
        status = info.fileName() + " " + QString(trUtf8("(Contains %1 items)")).
                                         arg(QDir(info.absoluteFilePath()).entryList().count() - 2); //-2 because . and ..

    emit this->currentStatusChanged(status);
}

void SlothListView::onSelectionChange(const QItemSelection &selected, const QItemSelection &deselected) {
    foreach (QModelIndex index, deselected.indexes()) {
        if(index.column() == 0) {
            QFileInfo info = this->sfsm->fileInfo(index);

            if(info.isFile()) {
                this->currentSelectedTotalFiles -=1;
                this->currentSelectedTotalSize -= info.size();
            }
            else if(info.isDir()) {
                this->currentSelectedTotalDirs -= 1;
                this->currentSelectedTotalSubFiles -= QDir(info.absoluteFilePath()).count() - 2;
            }
        }

    }

    foreach (QModelIndex index, selected.indexes()) {
        if(index.column() == 0) {
            QFileInfo info = this->sfsm->fileInfo(index);

            if(info.isFile()) {
                this->currentSelectedTotalFiles +=1;
                this->currentSelectedTotalSize += info.size();
            }
            else if(info.isDir()) {
                this->currentSelectedTotalDirs += 1;
                this->currentSelectedTotalSubFiles += QDir(info.absoluteFilePath()).count() - 2;
            }
        }
    }

    if(this->currentSelectedTotalDirs == 0 && this->currentSelectedTotalFiles == 0) {
        emit this->currentStatusChanged("");
        return;
    }


    QString status;
    status = QString(trUtf8("%1 dirs (Contains %2 items), %3 files (%4)"))
            .arg(this->currentSelectedTotalDirs)
            .arg(this->currentSelectedTotalSubFiles)
            .arg(this->currentSelectedTotalFiles)
            .arg(FileUtils::formatFileSize(this->currentSelectedTotalSize));


    emit this->currentStatusChanged(status);


    /* this is a slower method, maybe not. i dont know. */
    /*
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
            totalCount += QDir(info.absoluteFilePath()).count();
        }
    }

    status = QString(trUtf8("%1 dirs (Contains %2 items), %3 files (%4)"))
             .arg(totalDir).arg(totalCount).arg(totalFile).arg(FileUtils::formatFileSize(totalSize));

    if(totalDir == 0 && totalFile == 0)
        status = "";

    emit this->currentStatusChanged(status);
    */
}


void SlothListView::onDoubleClicked(const QModelIndex &index) {
    QString path = this->sfsm->filePath(index);
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
    if(!this->selectionModel()->selectedIndexes().isEmpty()) {
        if(event->key() == Qt::Key_Delete && event->modifiers() == Qt::SHIFT) {
            this->deletePermanently();
            return;
        }
        else if(event->key() == Qt::Key_Delete) {
            this->moveToTrash();
            return;
        }
        else if(event->key() == Qt::Key_F2) {
            this->rename();
            return;
        }
        else if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
            this->onDoubleClicked(this->selectionModel()->currentIndex());
            return;
        }
        else if(event->matches(QKeySequence::Copy)) {
            this->copy();
            return;
        }
        else if(event->matches(QKeySequence::Cut)) {
            this->cut();
            return;
        }
    }

    if(event->matches(QKeySequence::Paste)) {
        this->paste();
        return;
    }
    else if(event->matches(QKeySequence::SelectAll)) {
        this->selectAll();
        return;
    }
    else if(event->matches(QKeySequence::Find)) {
        emit this->filterRequested();
    }
    else if(event->key() == Qt::Key_Backspace) {
        this->goBack();
        return;
    }
    else if(event->key() == Qt::Key_W && event->modifiers() == Qt::CTRL) {
        emit this->tabCloseRequested();
        return;
    }
    else if(event->key() == Qt::Key_T && event->modifiers() == Qt::CTRL) {
        emit this->newEmptyTabRequested();
        return;
    }
    else if(event->key() == Qt::Key_H && event->modifiers() == Qt::CTRL) {
        this->actShowHidden->trigger();
        return;
    }
    else if(!event->text().isEmpty()){
        this->keyboardSearch(event->text());
        return;
    }

    QListView::keyPressEvent(event);
}

void SlothListView::onCustomContextMenu(const QPoint &point) {
    QModelIndex index = this->indexAt(point);
    if (index.isValid()) { //when clicked on items
        //FIXME:
        QFileInfo info = this->sfsm->fileInfo(index);
        QString mime = FileUtils::getMimeType(info.absoluteFilePath());
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
            QMenu *openWith = menu.addMenu(trUtf8("Open with..."));

            if(mime.startsWith("text/"))
                openWith->addAction(Quick::getIcon("document-open"), trUtf8("Open here"), this, SLOT(emitTextEditRequested()));
            else if(SlothFileEditor::getSupportedMimeTypes().contains(mime))
                openWith->addAction(Quick::getIcon("document-open"), trUtf8("Open here"), this, SLOT(emitTextEditRequested()));
            else if(mime.startsWith("image/"))
                openWith->addAction(Quick::getIcon("document-open"), trUtf8("Open here"), this, SLOT(emitImageViewerRequested()));

            DesktopFile df;
            foreach (QString desktopFilePath, df.getDesktopFilesFromMimeInfo(mime)) {
                df.setPath(desktopFilePath);

                if(df.load()) {
                    QAction *act = openWith->addAction(df.getIcon(), df.getName(), this, SLOT(openWith()));
                    act->setToolTip(df.getExec());
                }
            }

            if(openWith->isEmpty())
                delete openWith;

            menu.addAction(this->actCut);
            menu.addAction(this->actCopy);
            menu.addAction(this->actRename);
            menu.addSeparator();
            menu.addAction(this->actMoveToTrash);
            menu.addAction(this->actDelete);
            menu.addSeparator();
            if(Utils::mimeTypesOfArchives().contains(mime))
                menu.addAction(this->actExtract);
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

void SlothListView::emitTextEditRequested() {
    emit this->textEditRequested(this->getCurrentSelectedPath());
}

void SlothListView::emitImageViewerRequested() {
    emit this->imageViewerRequested(this->getCurrentSelectedPath());
}

void SlothListView::openInNewTab() {
    foreach(QString path, this->getCurrentSelectedPaths())
        emit this->newTabRequested(path);
}

void SlothListView::openWith() {
    QAction *action = qobject_cast<QAction*>(sender());
    QString exec = action->toolTip();
    QStringList args;
    QString file = this->getCurrentSelectedPath();

    args << file;
    exec = exec.split(" ")[0];

    QProcess::startDetached(exec, args);
}

void SlothListView::cut() {
    this->copy();

    QFile sfile(Utils::getTempFile());
    if (!sfile.open(QFile::WriteOnly | QFile::Text))
        return;

    QTextStream out(&sfile);
    out << qApp->clipboard()->text();
    sfile.close();
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
            QProgressDialog dialog;
            dialog.setLabelText(trUtf8("Deleting files..."));

            QFutureWatcher<void> futureWatcher;
            connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
            connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
            connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));
            connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
            futureWatcher.setFuture(QtConcurrent::run(&FileUtils::removeRecursivelyWithList, this->getCurrentSelectedPaths()));
            dialog.exec();
        }
    }
}

void SlothListView::compress() {
    this->compDialog = new CompressionDialog(this);
    this->compDialog->setItems(this->getCurrentSelectedPaths());
    this->compDialog->show();
}

void SlothListView::extract() {
    //TODO: add progressbar
    Compression comp;
    comp.extract(this->getCurrentSelectedPath(), this->getCurrentDir());
}

void SlothListView::sendTo() {
    //TODO:
}

void SlothListView::properties() {
    this->infoDialog = new SlothInfoPanel();
    this->infoDialog->setInfo(this->getCurrentSelectedPaths(), true);
    this->infoDialog->move(SlothSettings::defaultPosition());
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
    if(this->clipboard->hasFiles()) {
        QFile file(Utils::getTempFile());

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QStringList templist = QString(file.readAll()).split('\n');

            if(templist == this->clipboard->getFiles()) {
                foreach (QString item, templist) {
                    QString newPath = FileUtils::combine(this->getCurrentDir(), FileUtils::getName(item));
                    FileUtils::moveItem(item, newPath);
                    //FIXME: cannot move other drives
                }

                QFile::remove(Utils::getTempFile());
                qApp->clipboard()->clear();
                return;
            }
        }
    }

    this->clipboard->paste(this->getCurrentDir());
    qApp->clipboard()->clear();
}
