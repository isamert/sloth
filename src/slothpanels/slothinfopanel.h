#ifndef SLOTHINFOPANEL_H
#define SLOTHINFOPANEL_H

#include <QtWidgets>
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

    void setInfo(const QString &path);
    void setInfo(const QStringList &paths);
    void setInfo(const QFileInfo &info);

    void loadPermissions();

protected:
    //void resizeEvent(QResizeEvent *event);

private slots:
};

#endif // SLOTHINFOPANEL_H
