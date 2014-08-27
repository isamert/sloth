#ifndef SLOTHLISTVIEW_H
#define SLOTHLISTVIEW_H

#include "qglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "compression/compressiondialog.h"
#include "utils/fileutils.h"
#include "utils/clipboard.h"
#include "slothsettings.h"
#include "slothpanels/slothinfopanel.h"

class SlothListView : public QListView
{
    Q_OBJECT
private:

public:
    explicit SlothListView(QWidget *parent = 0, const QString &path = QDir::homePath());

    QFileSystemModel *fsm;
    QMenu *contextMenuValid;
    QMenu *contextMenuEmpty;
    QStringList history;
    int historyCurrent;

    QString getCurrentDir();
    QString getCurrentTabName();
    QString getCurrentSelectedPath();
    QStringList getCurrentSelectedPaths();
    void openDir(QString dir, bool addHistory = true);
    bool goBack();
    bool goForward();

    void loadSettings();
    void loadActions();
    void loadNewFileMenu(const QString &tempDir, QMenu *menu);

    SlothInfoPanel *infoDialog;

    QMenu *menuNewFile;
    QSignalMapper *newFileMapper;

    QAction *actEmptyFile;
    QAction *actNewFolder;
    QAction *actOpenInNewTab;
    QAction *actSendTo;
    QAction *actShowHidden;
    QAction *actCut;
    QAction *actCopy;
    QAction *actPaste;
    QAction *actDelete;
    QAction *actMoveToTrash;
    QAction *actRename;
    QAction *actCompress;
    QAction *actExtract;
    QAction *actOpenTerminalHere;
    QAction *actProperties;

    CompressionDialog *compDialog;

private slots:
    void onCurrentIndexChange(const QModelIndex &current, const QModelIndex &previous);
    void onSelectionChange(const QItemSelection &selected, const QItemSelection &deselected);
    void onDoubleClicked(const QModelIndex &index);
    void onCustomContextMenu(const QPoint &point);
    void onRootPathChanged(const QString &path);

    void setShowHidden(bool enabled);

    void openInNewTab();
    void cut();
    void copy();
    void rename();
    void moveToTrash();
    void deletePermanently();
    void compress();
    void extract();
    void sendTo();
    void properties();

    void newEmptyFile();
    void newFolder();
    void paste();

    void newFileMenuItemClicked(const QString &path);

signals:
    void currentPathChanged(const QString &currentPath);
    void newTabRequested(const QString &currentPath);
    void newEmptyTabRequested();
    void tabCloseRequested();
    void currentStatusChanged(const QString &currentPath);

protected:
    void keyPressEvent(QKeyEvent *event);

};

#endif // SLOTHLISTVIEW_H
