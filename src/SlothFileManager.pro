#-------------------------------------------------
#
# Project created by QtCreator 2014-08-25T13:35:33
#
#-------------------------------------------------

QT       += core gui

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
    utils/clipboard.cpp

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
    utils/clipboard.h

FORMS += \
    compression/compressiondialog.ui

