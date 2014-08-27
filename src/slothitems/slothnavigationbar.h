#ifndef SLOTHNAVIGATIONBAR_H
#define SLOTHNAVIGATIONBAR_H

#include "qglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "slothitems/slothplacesbox.h"
#include "utils/quick.h"
#include "utils/fileutils.h"

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = 0, const QString &text = "", bool effect = false);
    bool effect;
    void setEffect(bool effect);

signals:
    void clicked();

public slots:

protected:
    void mouseReleaseEvent(QMouseEvent *event) ;
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

};


class SlothNavigationBar : public QWidget
{
    Q_OBJECT

private:
    QString currentPath;
    QSignalMapper *signalMapper;
    QLineEdit *linePath;
    QCompleter *completer;

    bool useLineModel;

    void setPathToBarModel(const QString &path);
    void clearItems();
    void loadLineModel();

public:
    explicit SlothNavigationBar(QWidget *parent = 0, const QString &path = QDir::homePath(),
                                bool useLineModel = false);

    void setPath(const QString &path);
    void changeModel();

signals:
    void barItemClicked(const QString &path);

public slots:

private slots:
    void openClickedDir(const QString &path);
    void openFromLineEdit();



};

#endif // SLOTHNAVIGATIONBAR_H
