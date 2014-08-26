#ifndef SLOTHTABWIDGET_H
#define SLOTHTABWIDGET_H

#include <QtWidgets>

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
