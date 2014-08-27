#include "compression/compressiondialog.h"
#include "ui_compressiondialog.h"

CompressionDialog::CompressionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompressionDialog)
{
    ui->setupUi(this);

    this->comboPath = new SlothPlacesBox(this);
    ui->gridLayout->addWidget(comboPath, 1, 2, 1, 1);
    ui->lblIcon->setPixmap(Quick::getIcon("tar").pixmap(62, 62));
    ui->progressBar->hide();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    this->resize(this->width(), this->height() - ui->progressBar->height());


    zip = new Compression(this);

    if(zip->is7zipInstalled()) {
        ui->comboFileType->addItem(".7z");
        ui->comboFileType->addItem(".zip");
    }
    if(zip->isRarInstalled()) {
        ui->comboFileType->addItem(".rar");
    }
    if(zip->isGenIsoImageInstalled()) {
        ui->comboFileType->addItem(".iso");
    }
}

CompressionDialog::~CompressionDialog()
{
    delete ui;
}

void CompressionDialog::setItems(const QStringList &list) {
    this->items = list;
    //get working dir's name

    QString archiveName;
    archiveName = list.count() == 1 ? FileUtils::getName(list[0]) : FileUtils::getName(
                                                                      FileUtils::getUpperPath(list[0]));
    ui->lineFileName->setText(archiveName);
    this->comboPath->setCurrentPath(FileUtils::getUpperPath(list[0]));
}

void CompressionDialog::on_btnCreate_clicked() {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QString cFile = FileUtils::combine(this->comboPath->currentData().toString(), ui->lineFileName->text());
#else
    QString cFile = FileUtils::combine(this->comboPath->itemData(this->comboPath->currentIndex()).toString(), ui->lineFileName->text());
#endif

    QString suffix = ui->comboFileType->currentText();
    cFile.append(suffix);

    if(QFile(cFile).exists()) {
        Quick::msgWarning(trUtf8("Warning"), trUtf8("This file exists. Choose another name."));
        return;
    }

    int compressionType = ui->comboCompressType->currentIndex(); //Compression::Ultra (6), Maximum(5) ...
                                                                 //Same order as combobox

    connect(zip, SIGNAL(compressionError(QString)), this, SLOT(handleCompressionError(QString)));
    connect(zip, SIGNAL(progressChanged(QString)), this, SLOT(handleProgressChange(QString)));
    connect(zip, SIGNAL(compressFinished(int)), this, SLOT(handleCompressionFinish(int)));

    if(suffix == ".tar") {
        zip->compressWithTar(this->items, cFile, zip->tar);
    }
    else if(suffix == ".gz" || suffix == ".tar.gz") {
        zip->compressWithTar(this->items, cFile, zip->gzip);
    }
    else if(suffix == ".tar.bz2" || suffix == ".tbz2" || suffix == ".tb2") {
        zip->compressWithTar(this->items, cFile, zip->bzip2);
    }
    else if(suffix == ".7z") {
        zip->compressWith7z(this->items, cFile, compressionType, zip->t7z, ui->linePassword->text());
    }
    else if(suffix == ".bz2") {
        zip->compressWith7z(this->items, cFile, compressionType, zip->bzip2, ui->linePassword->text());
    }
    else if(suffix == ".zip") {
        zip->compressWith7z(this->items, cFile, compressionType, zip->zip, ui->linePassword->text());
    }
    else if(suffix == ".rar") {
        zip->compressWithRar(this->items, cFile);
    }
    else if(suffix == ".iso") {
        zip->createIso(this->items, cFile);
    }

    ui->btnCreate->setEnabled(false);
    ui->btnCreate->setText(trUtf8("Please Wait..."));

    ui->progressBar->show();
    this->resize(this->width(), this->height() + ui->progressBar->height());
}

void CompressionDialog::on_btnExit_clicked() {
    this->close();
}

void CompressionDialog::on_lineFileName_textChanged(const QString &arg1) {
    ui->btnCreate->setEnabled(!ui->lineFileName->text().isEmpty());
}


void CompressionDialog::on_comboFileType_currentIndexChanged(int index) {
    QString suffix = ui->comboFileType->itemText(index);
    if(suffix == ".7z" || suffix == ".bz2" || suffix == ".zip") {
        ui->linePassword->setEnabled(true);
        ui->linePassword->clear();
        ui->comboCompressType->setEnabled(true);
    } else {
        ui->linePassword->setEnabled(false);
        ui->comboCompressType->setEnabled(false);
    }
    //FIXME: add icons
    ui->lblIcon->setPixmap(QIcon::fromTheme(suffix.remove(0 , 1), QIcon::fromTheme("tar")).pixmap(62, 62));
}

void CompressionDialog::handleProgressChange(const QString &out) {
    //FIXME: maybe there is a better way than this fake progress
    this->outString += out + '\n';
    int newValue = ui->progressBar->value() + 10;
    if(newValue <= 100)
        ui->progressBar->setValue(newValue);
}


void CompressionDialog::handleCompressionError(const QString &err) {
    this->errString += err + '\n';
}

void CompressionDialog::handleCompressionFinish(int i) {
    ui->progressBar->setValue(100);

    if(i == -1 || i == 1 || i == 2) {
        this->errString.remove(this->errString.count() - 1, 1); //remove last "new line"
        Quick::msgWarning(trUtf8("Error"), trUtf8("Compression failed!") + '\n' + this->errString);
    }
    else
        Quick::msgInfo(trUtf8("Finished"), trUtf8("Compression finished!"));

    this->close();
}
