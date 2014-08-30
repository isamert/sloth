#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("isamert");
    a.setOrganizationDomain("github.com/isamert");
    a.setApplicationName("SlothFileManager");

    MainWindow w;
    w.show();

    return a.exec();
}
