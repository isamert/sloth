#ifndef COMPRESSIONDIALOG_H
#define COMPRESSIONDIALOG_H

#include "qglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "compression/compression.h"
#include "utils/fileutils.h"
#include "utils/quick.h"
#include "slothwidgets/slothplacesbox.h"

namespace Ui {
class CompressionDialog;
}

class CompressionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CompressionDialog(QWidget *parent = 0);
    ~CompressionDialog();

    void setItems(const QStringList &list);

private slots:
    void on_btnCreate_clicked();
    void on_btnExit_clicked();
    void on_comboFileType_currentIndexChanged(int index);
    void on_lineFileName_textChanged(const QString &arg1);

    void handleCompressionFinish(int i);
    void handleCompressionError(const QString &err);
    void handleProgressChange(const QString &out);

private:
    Ui::CompressionDialog *ui;
    Compression *zip;
    QStringList items;

    SlothPlacesBox *comboPath;

    QString errString;
    QString outString;

};

#endif // COMPRESSIONDIALOG_H
