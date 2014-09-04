#ifndef SLOTHMINITERMWIDGET_H
#define SLOTHMINITERMWIDGET_H

#include <QWidget>
#include <QToolBar>
#include <QLineEdit>

#include "utils/quick.h"
#include "utils/fileutils.h"

class SlothMinitermWidget : public QToolBar
{
    Q_OBJECT

private:
    QProcess *process;
    QLineEdit *lineMiniterm;
    QString workingDirectory;
    QStringList commandHistory;
    QPlainTextEdit *txtOutput;
    int currentHistoryIndex;

public:
    explicit SlothMinitermWidget(QWidget *parent = 0);
    ~SlothMinitermWidget();
    void setWorkingDirectory(const QString &dir);

signals:
    void openDirRequested(const QString &dir);

public slots:
    void showOrHide();
    void executeCommand();
    void printOutput();
    void showOutput();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

};

#endif // SLOTHMINITERMWIDGET_H
