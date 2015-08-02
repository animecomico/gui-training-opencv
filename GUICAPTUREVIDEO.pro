#-------------------------------------------------
#
# Project created by QtCreator 2013-04-16T18:24:27
#
#-------------------------------------------------

QT       += core gui opengl widgets

QTVERSION = 4

greaterThan(QT_MAJOR_VERSION, 4){
    QT += widgets
    QTVERSION = 5
}

TARGET = GUICAPTUREVIDEO
TEMPLATE = app

linux-g++ | linux-g++-64 {
    PC_HOST += LINUX
}

include(../configures/OpencvLink.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    UTILITYQT.cpp \
    LabelEventMouse.cpp \
    VideoLoadOCV_Thread.cpp

HEADERS  += mainwindow.h \
    UTILITYQT.h \
    LabelEventMouse.h \
    VideoLoadOCV_Thread.h

FORMS    += mainwindow.ui

contains(QTVERSION,5){
    DEFINES+=USE_QT5
}

RESOURCES += \
    Recursos.qrc

