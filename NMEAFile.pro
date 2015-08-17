#-------------------------------------------------
#
# Project created by QtCreator 2015-05-31T22:35:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = NMEAFile
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    func.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    func.h

FORMS    += mainwindow.ui

ICON = NMEAFile.icns
