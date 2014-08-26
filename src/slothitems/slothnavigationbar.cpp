#include "slothitems/slothnavigationbar.h"

ClickableLabel::ClickableLabel(QWidget *parent /* = 0 */,
                               const QString &text /* = "" */, bool effect /* = false */ ) :
    QLabel(parent)
{
    this->setText(text);
    this->setTextFormat(Qt::RichText);

    this->effect = effect;
}


void ClickableLabel::setEffect(bool effect) {
    this->effect = effect;
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *event) {
    emit clicked();
}

void ClickableLabel::enterEvent(QEvent *event) {
    if(effect)
        this->setText("<b>" + this->text() + "</b>");
}

void ClickableLabel::leaveEvent(QEvent *event) {
    if(effect) {
        QTextDocument doc;          //for converting html to plain text
        doc.setHtml(this->text());
        this->setText(doc.toPlainText());
    }
}



SlothNavigationBar::SlothNavigationBar(QWidget *parent, const QString &path /* = QDir::homePath() */,
                                       bool useLineModel /* =false */) :
    QWidget(parent)
{
    this->useLineModel = useLineModel;
    if(this->useLineModel) {
        this->loadLineModel();
    }

    this->signalMapper = new QSignalMapper(this);
    this->setPath(path);
}

void SlothNavigationBar::setPath(const QString &path) {
    this->currentPath = path;
    if(this->useLineModel) {
        this->linePath->setText(path);
    }
    else {
        this->setPathToBarModel(path);
    }
}

void SlothNavigationBar::setPathToBarModel(const QString &path) {
    //TODO: add menu to lblsperetor which list the folders contents
    //TODO: instead of deleting all items, add/remove new ones if neccesary
    this->clearItems();
    QString strHome = QDir::homePath();
    QString npath = QDir::toNativeSeparators(path);

    if(npath.startsWith(strHome))
        npath = npath.replace(strHome, "__HOME__");

    QStringList paths = npath.split('/');

    this->setLayout(new QHBoxLayout);
    this->layout()->setSpacing(0);
    this->layout()->setMargin(0);

    int i;
    QString currentPath = "";
    for(i = 0; i < paths.count(); ++i) {
        QString str = paths[i];
        if(str != "") {
            ClickableLabel *lblPath;
            lblPath = new ClickableLabel();
            lblPath->setEffect(true);

            ClickableLabel *lblSeparator;
            lblSeparator = new ClickableLabel();

            if(str == "__HOME__") {
                currentPath += strHome;

                lblPath->setText(trUtf8("HOME"));
                lblPath->setPixmap(Quick::getIcon("go-home").pixmap(32, 32));
                lblPath->setEffect(false);
            }
            else {
                currentPath += "/" + str;
                if(str.count() > 20) {
                    lblPath->setText(str.remove(20, str.count() - 20) + "...");
                }
                else
                    lblPath->setText(str);
            }

            lblPath->setToolTip(currentPath);
            lblSeparator->setToolTip(currentPath);
            lblSeparator->setText(" <b>></b> "); //FIXME: incLude trailing spaces
                                                 //Qt::TextIncludeTrailingSpaces?

            this->layout()->addWidget(lblPath);
            this->layout()->addWidget(lblSeparator);

            connect(lblPath, SIGNAL(clicked()), this->signalMapper, SLOT(map()));
            this->signalMapper->setMapping(lblPath, currentPath);
        }
    }

    connect(signalMapper, SIGNAL(mapped(const QString &)), this, SLOT(openClickedDir(const QString &)));
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
        this->setPathToBarModel(this->currentPath);
        this->useLineModel = false;
    }
    else {
        this->clearItems();
        this->useLineModel = true;
        this->loadLineModel();
        this->setPath(this->currentPath);
    }
}

void SlothNavigationBar::loadLineModel() {
    //TODO: auto completion for line edit
    this->linePath = new QLineEdit(this);
    this->setLayout(new QHBoxLayout);

    completer = new QCompleter(this);
    completer->setModel(new QDirModel(completer));
    this->linePath->setCompleter(completer);

    this->layout()->setSpacing(0);
    this->layout()->setMargin(0);
    this->layout()->addWidget(this->linePath);

    connect(this->linePath, SIGNAL(returnPressed()), this, SLOT(openFromLineEdit()));
}

void SlothNavigationBar::openClickedDir(const QString &path) {
    emit this->barItemClicked(path);
}

void SlothNavigationBar::openFromLineEdit() {
    QFileInfo info(this->linePath->text());
    if(info.isFile())
        FileUtils::openDetached(this->linePath->text());
    else
        emit this->barItemClicked(this->linePath->text());
}
