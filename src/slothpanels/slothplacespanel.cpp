#include "slothpanels/slothplacespanel.h"

//TODO: add/edit places items

SlothPlaceItem::SlothPlaceItem(QWidget *parent, const QString &text /* = "" */,
                               const QString &path /* = "" */, const QIcon &icon /* = QIcon() */) :
    QWidget(parent)
{
    this->layout = new QHBoxLayout(this);
    this->layout->setSpacing(0);
    this->layout->setMargin(0);

    this->setLayout(this->layout);

    this->image = new QLabel(this);
    this->text = new QLabel(this);

    this->layout->addWidget(this->image);
    this->layout->addWidget(this->text);

    this->image->setFixedWidth(40); //8 for spacing, setSpacing(8) does not work
                                    //it brokes background color
    this->setFixedHeight(35);     //3 for spacing between SlothPlaceItems

    this->text->setText(text);
    this->path = path;
    this->setIcon(icon);
}

void SlothPlaceItem::setIcon(const QIcon &icon) {
    this->image->setPixmap(icon.pixmap(32, 32));
}

void SlothPlaceItem::setText(const QString &text) {
    this->text->setText(text);
}

void SlothPlaceItem::setPath(const QString &path) {
    this->path = path;
}

void SlothPlaceItem::highlight() {
    //FIXME: find a better color
    //this->setStyleSheet("* { background-color: rgb(50, 50, 50); }");

    QColor color = QWidget::palette().color(QPalette::Highlight);
    this->text->setBackgroundRole(QPalette::Dark);
    this->setStyleSheet(QString("* { background-color: %1; }").arg(color.name()));

}

void SlothPlaceItem::clearHighlight() {
    this->setStyleSheet("");
    this->text->setBackgroundRole(QPalette::NoRole);
}

void SlothPlaceItem::mouseReleaseEvent(QMouseEvent *event) {
    emit clicked(this->path);
    event->accept();
}

SlothPlacesPanel::SlothPlacesPanel(QWidget *parent) :
    QScrollArea(parent)
{
    this->setFrameShape(QFrame::NoFrame);

    this->mainWidget = new QWidget(this);
    this->layout = new QVBoxLayout(this->mainWidget);
    this->layout->setSpacing(0);
    this->layout->setMargin(0);
    this->mainWidget->setLayout(this->layout);

    this->setWidget(this->mainWidget);
    this->setWidgetResizable(true);

    verticalSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    this->layout->addItem(verticalSpacer);

    foreach (QString name, SlothSettings::getPlaces()) {
        if(name == "addLine=addLine") {
            this->addLine();
        }

        QStringList l = name.split("=");
        //name=path=iconName
        this->addItem(l[0], l[1], Quick::getIcon(l[2]));
    }

    this->loadDrives();
}

void SlothPlacesPanel::emitClicked(const QString &path) {
    emit this->itemClicked(path);
}

void SlothPlacesPanel::loadDrives() {
    QStringList list = Utils::getDrives();

    if(!list.isEmpty()) {
        this->addLine();
        foreach (QString drive, list) {
            this->addItem(FileUtils::getName(drive), drive, Quick::getIcon("drive-removeable-usb"));
        }
    }
}

void SlothPlacesPanel::addItem(const QString &text, const QString &path, const QIcon &icon) {
    if(QFile::exists(path)) {
        SlothPlaceItem *spi;
        spi = new SlothPlaceItem(this);

        spi->setText(text);
        spi->setPath(path);
        spi->setIcon(icon);
        spi->setToolTip(path);

        connect(spi, SIGNAL(clicked(QString)), this, SLOT(emitClicked(QString)));
        this->layout->insertWidget(this->layout->count() - 1, spi);
        //insert before the spacer, after the last item
    }
}

void SlothPlacesPanel::addLine() {
    QFrame *line;
    line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setObjectName("__LINE__");

    this->layout->insertWidget(this->layout->count() - 1, line);
}

void SlothPlacesPanel::setCurrentItem(const QString &path) {
    SlothPlaceItem *spi;
    QWidget *w;

    for (int i = 0; i < this->layout->count(); ++i) {
        w = this->layout->itemAt(i)->widget();
        if(w != NULL && w->objectName() != "__LINE__")
            spi = dynamic_cast<SlothPlaceItem*>(w);

            if(spi->path == path)
                spi->highlight();
            else
                spi->clearHighlight();
    }
}


