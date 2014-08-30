#ifndef SLOTHPLACESPANEL_H
#define SLOTHPLACESPANEL_H

#include "qglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "slothsettings.h"
#include "utils/utils.h"
#include "utils/fileutils.h"
#include "utils/quick.h"

class SlothPlaceItem : public QWidget
{
    Q_OBJECT
public:
    explicit SlothPlaceItem(QWidget *parent = 0, const QString &text = "",
                            const QString &path = "", const QIcon &icon = QIcon());
    QHBoxLayout *layout;
    QLabel *image;
    QLabel *text;
    QString path;

    void setIcon(const QIcon &icon);
    void setText(const QString &text);
    void setPath(const QString &path);
    void highlight();
    void clearHighlight();

signals:
    void clicked(const QString &path);

public slots:

protected:
    void mouseReleaseEvent(QMouseEvent *event);

};


class SlothPlacesPanel : public QScrollArea
{
    Q_OBJECT
public:
    explicit SlothPlacesPanel(QWidget *parent = 0);
    QVBoxLayout *layout;
    QSpacerItem *verticalSpacer;
    QWidget *mainWidget;

    void addItem(const QString &text, const QString &path, const QIcon &icon);
    void addLine();
    void setCurrentItem(const QString &path);
    void loadDrives();

signals:
    void itemClicked(const QString &path);

public slots:
    void emitClicked(const QString &path);

};

#endif // SLOTHPLACESPANEL_H
