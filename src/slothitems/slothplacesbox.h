#ifndef SLOTHPLACESBOX_H
#define SLOTHPLACESBOX_H

#include "qglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "utils/quick.h"
#include "utils/fileutils.h"

class SlothPlacesBox : public QComboBox
{
    Q_OBJECT

private:
    int lastIndex;

public:
    explicit SlothPlacesBox(QWidget *parent = 0);

    void setCurrentPath(const QString &path);
    QString showFolderSelectionDialog();

    static QString getDesktopLocation();
    static QString getDocumentsLocationn();
    static QString getMoviesLocation();
    static QString getPicturesLocation();
    static QString getMusicLocation();
    static QString getDownloadLocation();
    static QStringList getApplicationsLocation();
    static QString getPublicShareLocation();
    static QString getTemplatesLocation();

    static QString getFromXDG(const QString &dirName);

signals:

private slots:
    void onCurrentIndexChange(int i);

};

#endif // SLOTHPLACESBOX_H
