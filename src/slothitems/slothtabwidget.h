#ifndef SLOTHTABWIDGET_H
#define SLOTHTABWIDGET_H

#include "qglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

class SlothTabWidget : public QTabWidget
{
    Q_OBJECT

private:

public:
    explicit SlothTabWidget(QWidget *parent = 0);

signals:

public slots:

protected:
   void tabInserted(int index);
   void tabRemoved(int index);

};

#endif // SLOTHTABWIDGET_H
