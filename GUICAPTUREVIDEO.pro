#-------------------------------------------------
#
# Project created by QtCreator 2013-04-16T18:24:27
#
#-------------------------------------------------

QT       += core gui opengl

QTVERSION = 4

greaterThan(QT_MAJOR_VERSION, 4){
    QT += widgets
    QTVERSION = 5
}

TARGET = GUICAPTUREVIDEO
TEMPLATE = app

win32{
   PC_HOST += WINDOWS
   contains(QTVERSION,4){
        INCLUDEPATH+=F:/WINDOWS/BIBLIOTECA/X64/OPENCV/OPENCV_2.4.6X64_VS2010V1/include/
        LIBS+=-LF:/WINDOWS/BIBLIOTECA/X64/OPENCV/OPENCV_2.4.6X64_VS2010V1/lib/
        message(OPENCV Ver QT4.8)
   }
   contains(QTVERSION,5){
        INCLUDEPATH+=F:/WINDOWS/BIBLIOTECA/X64/OPENCV/OPENCV_2.4.6X64_VS2010V2/include/
        LIBS+=-LF:/WINDOWS/BIBLIOTECA/X64/OPENCV/OPENCV_2.4.6X64_VS2010V2/lib/
        message(OPENCV Ver QT5.0)
   }
}

linux-g++ | linux-g++-64 {
    PC_HOST += LINUX
}


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

CONFIG(debug, debug|release){
    contains(PC_HOST, WINDOWS){
        message(S.O. WINDOWS Debug)
        win32-msvc*{    #Visual Studio C++
            QMAKE_LFLAGS += /INCREMENTAL
            LIBS+= -lopencv_calib3d246d -lopencv_contrib246d -lopencv_core246d -lopencv_features2d246d
            LIBS+= -lopencv_highgui246d -lopencv_imgproc246d -lopencv_flann246d -lopencv_legacy246d -lopencv_video246d
        }
    }
    contains(PC_HOST, LINUX){
        message(S.O. LINUX Debug)

        INCLUDEPATH+= /media/WORKING/UBUNTU/12.04X64/OPENCV/OPENCV2.4.9_X64_GCC4.6.3/Debug/include
        LIBS+= -L/media/WORKING/UBUNTU/12.04X64/OPENCV/OPENCV2.4.9_X64_GCC4.6.3/Debug/lib
        LIBS+= -lopencv_core -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann
        LIBS+= -lopencv_gpu -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo
        LIBS+= -lopencv_softcascade -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_videostab
    }
}
CONFIG(release, debug|release){
    contains(PC_HOST, WINDOWS){
        message(S.O. WINDOWS Release)
        win32-msvc*{    #Visual Studio C++
            QMAKE_LFLAGS += /INCREMENTAL
            LIBS+= -lopencv_calib3d246 -lopencv_contrib246 -lopencv_core246 -lopencv_features2d246
            LIBS+= -lopencv_highgui246 -lopencv_imgproc246 -lopencv_flann246 -lopencv_legacy246 -lopencv_video246
        }
    }
    contains(PC_HOST, LINUX){
        message(S.O. LINUX Release)
        INCLUDEPATH+= /media/WORKING/UBUNTU/12.04X64/OPENCV/OPENCV2.4.9_X64_GCC4.6.3/Release/include
        LIBS+= -L/media/WORKING/UBUNTU/12.04X64/OPENCV/OPENCV2.4.9_X64_GCC4.6.3/Release/lib
        LIBS+= -lopencv_core -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann
        LIBS+= -lopencv_gpu -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo
        LIBS+= -lopencv_softcascade -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_videostab
    }
}

RESOURCES += \
    Recursos.qrc

