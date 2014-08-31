#include "slothtabwidget.h"

SlothTabWidget::SlothTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    this->setTabsClosable(true);
    this->setMovable(true);
    this->setUsesScrollButtons(true);

    //To prevent bad focus thing
    this->tabBar()->setFocusPolicy(Qt::NoFocus);

    this->setDocumentMode(true);
    this->tabBar()->setExpanding(true);

    //TODO: set user selected shape,
    /* QTabBar::RoundedNorth
     * or QTabBar::TriangularNorth
     * this->tabBar()->setShape(QTabBar::TriangularNorth); */
}

void SlothTabWidget::tabInserted(int index) {
    this->tabBar()->setVisible(this->count() > 1);
}

void SlothTabWidget::tabRemoved(int index) {
    this->tabBar()->setVisible(this->count() > 1);
}
