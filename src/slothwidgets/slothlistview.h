#ifndef SLOTHLISTVIEW_H
#define SLOTHLISTVIEW_H

#include "qglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QtConcurrentRun>

#include "compression/compressiondialog.h"
#include "utils/fileutils.h"
#include "utils/clipboard.h"
#include "slothsettings.h"
#include "slothpanels/slothinfopanel.h"
#include "utils/desktopfile.h"
#include "slothapps//slothfileeditor.h"
#include "slothobjects/slothfilesystemmodel.h"

class SlothListView : public QListView
{
    Q_OBJECT
private:
    qint64 currentSelectedTotalSize;
    int currentSelectedTotalDirs;
    int currentSelectedTotalFiles;
    int currentSelectedTotalSubFiles;

public:
    explicit SlothListView(QWidget *parent = 0, const QString &path = QDir::homePath());

    SlothFileSystemModel *sfsm;
    QMenu *contextMenuValid;
    QMenu *contextMenuEmpty;
    QStringList history;
    int historyCurrent;
    Clipboard *clipboard;
    QFileCopier *copier;



    QString getCurrentDir();
    QString getCurrentTabName();
    QString getCurrentSelectedPath();
    QStringList getCurrentSelectedPaths();
    void openDir(const QString &dir, bool addHistory = true);
    void filterItems(const QString &filter);
    void filterItems(const QStringList &filter);
    bool goBack();
    bool goForward();
    void goUp();

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
    void openWith();
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
    void emitTextEditRequested();
    void emitImageViewerRequested();

public slots:
    void setViewModeToIconMode();
    void setViewModeToListMode();

signals:
    void currentPathChanged(const QString &currentPath);
    void newTabRequested(const QString &currentPath);
    void newEmptyTabRequested();
    void tabCloseRequested();
    void filterRequested();
    void currentStatusChanged(const QString &currentPath);
    void textEditRequested(const QString &filePath);
    void imageViewerRequested(const QString &filePath);

protected:
    void keyPressEvent(QKeyEvent *event);

};

#endif // SLOTHLISTVIEW_H
