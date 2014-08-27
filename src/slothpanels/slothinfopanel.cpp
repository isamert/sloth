#include "slothpanels/slothinfopanel.h"
#include "ui_slothinfopanel.h"

SlothInfoPanel::SlothInfoPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SlothInfoPanel)
{
    //TODO: change file permissions

    ui->setupUi(this);
    this->setWindowTitle(trUtf8("Information"));
    ui->lblImage->setFixedHeight(128);

    this->filePath = "";
}

SlothInfoPanel::~SlothInfoPanel()
{
    delete ui;
}

void SlothInfoPanel::setInfo(const QString &path, bool showDirSize /* = false */) {
    this->setInfo(QFileInfo(path), showDirSize);
}

void SlothInfoPanel::setInfo(const QStringList &paths, bool showDirSize /* = false */) {
    if(paths.count() == 1) {
        this->setInfo(paths[0], true);
        return;
    }

    qint64 totalSize = 0;
    int totalCount = 0;
    int totalFile = 0;
    int totalDir = 0;

    foreach (QString index, paths) {
        QFileInfo info(index);

        if(info.isFile()) {
            totalSize += info.size();
            totalFile += 1;
            totalCount += 1;
        }
        else if(info.isDir()) {
            if(showDirSize)
                totalSize += FileUtils::getDirSize(info.absoluteFilePath());
            totalDir += 1;
            totalCount += QDir(info.absoluteFilePath()).entryList().count() - 2;
        }
    }

    ui->lblImage->setPixmap(QIcon("directory").pixmap(128, 128));

    ui->lblFileName->setText(QString("%1 Dirs, %2 files (Total %3 files)")
                             .arg(totalDir).arg(totalFile).arg(totalCount));
    ui->lblModified->setText("-");
    ui->lblSize->setText(FileUtils::formatFileSize(totalSize));
    ui->lblType->setText("-");
}

void SlothInfoPanel::setInfo(const QFileInfo &info, bool showDirSize /* = false */) {
    this->filePath = info.absoluteFilePath();

    QFileIconProvider iconProv;

    QPixmap pixmap;
    QString mime = FileUtils::getMimeType(info.absoluteFilePath());

    if(mime.contains("image")) {
        try { // maybe qt cannot read this image file
            QPixmap pm(info.absoluteFilePath());
            pixmap = pm.scaled(QSize(128, 128));
        }
        catch (...) {
            pixmap = iconProv.icon(info).pixmap(QSize(128, 128));
        }
    }
    else
        pixmap = iconProv.icon(info).pixmap(QSize(128, 128));

    ui->lblImage->setPixmap(pixmap);

    ui->lblFileName->setText(info.fileName());
    ui->lblModified->setText(info.lastModified().toString(Qt::DefaultLocaleShortDate));
    if(showDirSize && info.isDir())
        ui->lblSize->setText(FileUtils::formatFileSize(
                             FileUtils::getDirSize(info.absoluteFilePath())));
    else
        ui->lblSize->setText(FileUtils::formatFileSize(info.size()));

    ui->lblType->setText(mime);

    this->loadPermissions();
}

void SlothInfoPanel::loadPermissions() {
    QFileInfo info(this->filePath);

    ui->cGroupR->setChecked(info.permission(QFile::ReadGroup));
    ui->cGroupW->setChecked(info.permission(QFile::WriteGroup));
    ui->cGroupX->setChecked(info.permission(QFile::ExeGroup));

    ui->cOthersR->setChecked(info.permission(QFile::ReadOther));
    ui->cOthersW->setChecked(info.permission(QFile::WriteOther));
    ui->cOthersX->setChecked(info.permission(QFile::ExeOther));

    ui->cOwnerR->setChecked(info.permission(QFile::ReadOwner));
    ui->cOwnerW->setChecked(info.permission(QFile::WriteOwner));
    ui->cOwnerX->setChecked(info.permission(QFile::ExeOwner));

}
