#ifndef SLOTHINFOPANEL_H
#define SLOTHINFOPANEL_H

#include "qglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "utils/fileutils.h"

namespace Ui {
class SlothInfoPanel;
}

class SlothInfoPanel : public QWidget
{
    Q_OBJECT

private:
    Ui::SlothInfoPanel *ui;
    QString filePath;

public:
    explicit SlothInfoPanel(QWidget *parent = 0);
    ~SlothInfoPanel();

    void setInfo(const QString &path, bool showDirSize = false);
    void setInfo(const QStringList &paths, bool showDirSize = false);
    void setInfo(const QFileInfo &info, bool showDirSize = false);

    void loadPermissions();

protected:
    //void resizeEvent(QResizeEvent *event);

private slots:
};

#endif // SLOTHINFOPANEL_H
