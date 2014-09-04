#-------------------------------------------------
#
# Project created by QtCreator 2014-08-25T13:35:33
#
#-------------------------------------------------


QT        += gui core concurrent
#QT       += core
#QT       += gui
#QT       += concurrent
#QT       += multimedia


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
    slothsettings.cpp \
    utils/clipboard.cpp \
    slothaboutdialog.cpp \
    slothpanels/slothinfopanel.cpp \
    slothpanels/slothplacespanel.cpp \
    qfilecopier/qfilecopier.cpp \
    qfilecopier/slothcopier.cpp \
    utils/desktopfile.cpp \
    slothapps/slothfileeditor.cpp \
    slothapps/slothimageviewer.cpp \
    slothapps/slothmusicplayer.cpp \
    slothapps/slothmovieplayer.cpp \
    slothobjects/slothfilesystemmodel.cpp \
    slothwidgets/slothlistview.cpp \
    slothwidgets/slothnavigationbar.cpp \
    slothwidgets/slothplacesbox.cpp \
    slothwidgets/slothtabwidget.cpp \
    slothobjects/sloththumbnailcache.cpp \
    slothwidgets/slothminitermwidget.cpp

HEADERS  += mainwindow.h \
    utils/quick.h \
    utils/fileutils.h \
    utils/utils.h \
    compression/compression.h \
    compression/compressiondialog.h \
    slothsettings.h \
    utils/clipboard.h \
    slothaboutdialog.h \
    slothpanels/slothinfopanel.h \
    slothpanels/slothplacespanel.h \
    qfilecopier/qfilecopier.h \
    qfilecopier/qfilecopier_global.h \
    qfilecopier/qfilecopier_p.h \
    qfilecopier/slothcopier.h \
    utils/desktopfile.h \
    slothapps/slothfileeditor.h \
    slothapps/slothimageviewer.h \
    slothapps/slothmusicplayer.h \
    slothapps/slothmovieplayer.h \
    slothobjects/slothfilesystemmodel.h \
    slothwidgets/slothlistview.h \
    slothwidgets/slothnavigationbar.h \
    slothwidgets/slothplacesbox.h \
    slothwidgets/slothtabwidget.h \
    slothobjects/sloththumbnailcache.h \
    slothwidgets/slothminitermwidget.h

FORMS += \
    compression/compressiondialog.ui \
    slothaboutdialog.ui \
    slothpanels/slothinfopanel.ui

