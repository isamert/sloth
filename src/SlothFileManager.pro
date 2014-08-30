#-------------------------------------------------
#
# Project created by QtCreator 2014-08-25T13:35:33
#
#-------------------------------------------------

QT       += core gui concurrent
LIBS     += -lmagic

greaterThan(QT_MAJOR_VERSION, 4): LIBS -= -lmagic
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SlothFileManager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    utils/quick.cpp \
    utils/fileutils.cpp \
    utils/utils.cpp \
    compression/compression.cpp \
    compression/compressiondialog.cpp \
    slothitems/slothplacesbox.cpp \
    slothitems/slothfilesystemmodel.cpp \
    slothitems/slothlistview.cpp \
    slothsettings.cpp \
    slothitems/slothtabwidget.cpp \
    slothitems/slothnavigationbar.cpp \
    utils/clipboard.cpp \
    slothaboutdialog.cpp \
    slothpanels/slothinfopanel.cpp \
    slothpanels/slothplacespanel.cpp \
    qfilecopier/qfilecopier.cpp \
    qfilecopier/slothcopier.cpp \
    utils/desktopfile.cpp \
    slothitems/slothfileeditor.cpp \
    slothitems/slothimageviewer.cpp

HEADERS  += mainwindow.h \
    utils/quick.h \
    utils/fileutils.h \
    utils/utils.h \
    compression/compression.h \
    compression/compressiondialog.h \
    slothitems/slothplacesbox.h \
    slothitems/slothfilesystemmodel.h \
    slothitems/slothlistview.h \
    slothsettings.h \
    slothitems/slothtabwidget.h \
    slothitems/slothnavigationbar.h \
    utils/clipboard.h \
    slothaboutdialog.h \
    slothpanels/slothinfopanel.h \
    slothpanels/slothplacespanel.h \
    qfilecopier/qfilecopier.h \
    qfilecopier/qfilecopier_global.h \
    qfilecopier/qfilecopier_p.h \
    qfilecopier/slothcopier.h \
    utils/desktopfile.h \
    slothitems/slothfileeditor.h \
    slothitems/slothimageviewer.h

FORMS += \
    compression/compressiondialog.ui \
    slothaboutdialog.ui \
    slothpanels/slothinfopanel.ui

