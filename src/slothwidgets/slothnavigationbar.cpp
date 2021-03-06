#include "slothwidgets/slothnavigationbar.h"

ClickableLabel::ClickableLabel(QWidget *parent /* = 0 */,
                               const QString &text /* = "" */,
                               bool effect /* = false */ ) :
    QLabel(parent)
{
    this->setText(text);
    this->setTextFormat(Qt::RichText);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    this->effect = effect;
}


void ClickableLabel::setEffectEnabled(bool effect) {
    this->effect = effect;
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *event) {
    emit clicked();
    event->accept();
}

void ClickableLabel::enterEvent(QEvent *event) {
    if(effect)
        this->setText("<b>" + this->text() + "</b>");
    event->accept();
}

void ClickableLabel::leaveEvent(QEvent *event) {
    if(effect) {
        QTextDocument doc;          //for converting html to plain text
        doc.setHtml(this->text());
        this->setText(doc.toPlainText());
    }
    event->accept();
}



SlothNavigationBar::SlothNavigationBar(QWidget *parent, const QString &path /* = QDir::homePath() */,
                                       bool useLineModel /* =false */) :
    QWidget(parent)
{
    this->useLineModel = useLineModel;

    if(this->useLineModel)
        this->loadLineModel();

    this->completer = new QCompleter(this);
    this->completer->setModel(new QDirModel(this->completer));
    //FIXME: using QDirModel because QFileSystemModel has some issues with QCompleter

    this->signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(const QString &)), this, SLOT(openClickedDir(const QString &)));
    this->setPath(path);
}

void SlothNavigationBar::setPath(const QString &path) {
    this->currentPath = path;

    if(this->useLineModel)
        this->linePath->setText(path);
    else
        this->setPathToBarModel(path);
}

void SlothNavigationBar::setPathToBarModel(const QString &path) {
    this->clearItems();
    this->setLayout(new QHBoxLayout);
    this->layout()->setSpacing(5);
    this->layout()->setMargin(0);

    QString strHome = QDir::homePath();
    QString npath = QDir::cleanPath(path);

    if(npath.startsWith(strHome))
        npath = npath.replace(strHome, "__HOME__");

    QStringList paths = npath.split('/');
    QString currentPath = "";

    foreach (QString lpath, paths) {
        if(lpath == "")
            continue;

        ClickableLabel *lblPath;
        lblPath = new ClickableLabel(this);
        lblPath->setEffectEnabled(true);

        if(lpath == "__HOME__") {
            currentPath += strHome;

            lblPath->setText(trUtf8("HOME"));
            lblPath->setPixmap(Quick::getIcon("go-home").pixmap(32, 32));
            lblPath->setEffectEnabled(false);
        }
        else {
            currentPath += QDir::separator() + lpath;

            if(lpath.count() > 20)
                lblPath->setText(lpath.remove(20, lpath.count() - 20) + "...");
            else
                lblPath->setText(lpath);
        }

        lblPath->setToolTip(currentPath);
        this->layout()->addWidget(lblPath);

        if(QDir::cleanPath(currentPath) != QDir::cleanPath(path)) {
            ClickableLabel *lblSeparator;
            lblSeparator = new ClickableLabel(this);
            lblSeparator->setToolTip(currentPath);
            lblSeparator->setText("<b>></b>");

            this->layout()->addWidget(lblSeparator);
            connect(lblSeparator, SIGNAL(clicked()), this->signalMapper, SLOT(map()));
            this->signalMapper->setMapping(lblSeparator, "__COMPLETER__" + currentPath);
        }


        connect(lblPath, SIGNAL(clicked()), this->signalMapper, SLOT(map()));
        this->signalMapper->setMapping(lblPath, currentPath);


        connect(lblPath, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showMenu(QPoint)));
    }

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->layout()->addWidget(spacer);
}

void SlothNavigationBar::clearItems() {
    if (this->layout() != NULL) {
        QLayoutItem* item;
        while ((item = this->layout()->takeAt(0)) != NULL) {
            delete item->widget();
            delete item;
        }
        delete this->layout();
    }
}

void SlothNavigationBar::changeModel() {
    if(this->useLineModel) {
        this->clearItems();
        this->useLineModel = false;
        this->setPath(this->currentPath);
    }
    else {
        this->clearItems();
        this->useLineModel = true;
        this->loadLineModel();
        this->setPath(this->currentPath);
    }
}

void SlothNavigationBar::loadLineModel() {
    this->linePath = new QLineEdit(this);
    this->setLayout(new QHBoxLayout);

    this->linePath->setCompleter(this->completer);

    this->layout()->setSpacing(0);
    this->layout()->setMargin(0);
    this->layout()->addWidget(this->linePath);

    connect(this->linePath, SIGNAL(returnPressed()), this, SLOT(openFromLineEdit()));
}

void SlothNavigationBar::openClickedDir(const QString &path) {
    if(path.startsWith("__COMPLETER__")) {
        QString cleanPath = path;
        cleanPath = cleanPath.replace("__COMPLETER__", "");

        QDir dir(cleanPath);
        QDir::Filters filters(QDir::AllEntries | QDir::NoDotAndDotDot);

        QMenu* menu;
        menu = new QMenu(this);
        QFileIconProvider iconProv;

        foreach (QFileInfo info, dir.entryInfoList(filters)) {
            QAction *act = menu->addAction(iconProv.icon(info), info.fileName());
            act->setToolTip(info.absoluteFilePath());
            connect(act, SIGNAL(triggered()), this->signalMapper, SLOT(map()));
            this->signalMapper->setMapping(act, FileUtils::combine(cleanPath, info.fileName()));
        }

        menu->setStyleSheet("QMenu { menu-scrollable: 1; }");    //using with setmaxheight doesnt work
        menu->setMaximumHeight(qApp->activeWindow()->height());

        QPoint pos = QPoint(QCursor::pos().x(), this->mapToGlobal(this->pos()).y() + this->height());

        connect(menu, SIGNAL(aboutToHide()), menu, SLOT(deleteLater())); //to avoiding memory leak
        menu->exec(pos);
    }
    else {
        QFileInfo info(path);

        if(info.isFile())
            FileUtils::openDetached(path);
        else
            emit this->openPathRequested(path);
    }
}

void SlothNavigationBar::openFromLineEdit() {
    QFileInfo info(this->linePath->text());

    if(info.isFile())
        FileUtils::openDetached(this->linePath->text());
    else
        emit this->openPathRequested(this->linePath->text());
}

void SlothNavigationBar::showMenu(const QPoint &pos) {
    ClickableLabel* sender = static_cast<ClickableLabel*>(QObject::sender());
    this->currentRightClickItem = sender->toolTip();

    QMenu menu;
    menu.addAction(trUtf8("Copy"), this, SLOT(copyPathToClipboard()));
    menu.addAction(trUtf8("Open in new tab"), this, SLOT(openPathInNewTab()));
    menu.exec(sender->mapToGlobal(pos));
}

void SlothNavigationBar::copyPathToClipboard() {
    qApp->clipboard()->setText(this->currentRightClickItem);
}

void SlothNavigationBar::openPathInNewTab() {
    emit this->openPathInNewTabRequested(this->currentRightClickItem);
}
