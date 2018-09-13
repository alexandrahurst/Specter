#-------------------------------------------------
#
# Project created by QtCreator 2015-04-01T13:25:04
#
#-------------------------------------------------

QT       += core gui multimedia

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Specter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    spectrogramwidget.cpp \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    spectrogramwidget.h \
    imageutil.h \
    settingsdialog.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

DISTFILES +=

LIBS += -lfftw3
