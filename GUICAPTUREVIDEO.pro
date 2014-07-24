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

win32{
   PC_HOST += WINDOWS
   contains(QTVERSION,4){
        win32-msvc2010{
            INCLUDEPATH+=F:/WINDOWS/BIBLIOTECA/X64/OPENCV/OPENCV_2.4.7.2X64_VS2010V4/include/
            LIBS+=-LF:/WINDOWS/BIBLIOTECA/X64/OPENCV/OPENCV_2.4.7.2X64_VS2010V4/x64/vc10/lib/
            message(OPENCV Ver QT4.8 VS2010)
        }

        win32-msvc2012{
            INCLUDEPATH+=F:/WINDOWS/BIBLIOTECA/X64/OPENCV/OPENCV_2.4.7.2X64_VS2013V1/include/
            LIBS+=-LF:/WINDOWS/BIBLIOTECA/X64/OPENCV/OPENCV_2.4.7.2X64_VS2013V1/x64/vc12/lib/
            message(OPENCV Ver QT4.8 VS2013)
        }
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
        win32-msvc2010{    #Visual Studio C++
            QMAKE_LFLAGS += /INCREMENTAL            
            LIBS+= -lopencv_calib3d247d -lopencv_contrib247d -lopencv_core247d -lopencv_features2d247d
            LIBS+= -lopencv_highgui247d -lopencv_imgproc247d -lopencv_flann247d -lopencv_legacy247d -lopencv_video247d
        }
        win32-msvc2012{    #Visual Studio C++
            QMAKE_LFLAGS += /INCREMENTAL
            QMAKE_CXXFLAGS += /FS
            LIBS+= -lopencv_calib3d247d -lopencv_contrib247d -lopencv_core247d -lopencv_features2d247d
            LIBS+= -lopencv_highgui247d -lopencv_imgproc247d -lopencv_flann247d -lopencv_legacy247d -lopencv_video247d
        }
    }
    contains(PC_HOST, LINUX){
        message(S.O. LINUX Debug)

        INCLUDEPATH+= /media/linuxcarlos/WORKING/UBUNTU/14.04X64/OPENCV/OpenCV2.4.7/V1/Debug/include
        LIBS+= -L/media/linuxcarlos/WORKING/UBUNTU/14.04X64/OPENCV/OpenCV2.4.7/V1/Debug/lib
        LIBS+= -lopencv_core -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann
        LIBS+= -lopencv_gpu -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo
        LIBS+= -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_videostab
    }
}
CONFIG(release, debug|release){
    contains(PC_HOST, WINDOWS){
        message(S.O. WINDOWS Release)
        win32-msvc2010{    #Visual Studio C++
            QMAKE_LFLAGS += /INCREMENTAL            
            LIBS+= -lopencv_calib3d247 -lopencv_contrib247 -lopencv_core247 -lopencv_features2d247
            LIBS+= -lopencv_highgui247 -lopencv_imgproc247 -lopencv_flann247 -lopencv_legacy247 -lopencv_video247
        }
        win32-msvc2012{
            QMAKE_LFLAGS += /INCREMENTAL
            QMAKE_CXXFLAGS += /FS
            LIBS+= -lopencv_calib3d247 -lopencv_contrib247 -lopencv_core247 -lopencv_features2d247
            LIBS+= -lopencv_highgui247 -lopencv_imgproc247 -lopencv_flann247 -lopencv_legacy247 -lopencv_video247
        }
    }
    contains(PC_HOST, LINUX){
        message(S.O. LINUX Release)
        INCLUDEPATH+= /media/linuxcarlos/WORKING/UBUNTU/14.04X64/OPENCV/OpenCV2.4.7/V1/Release/include
        LIBS+= -L/media/linuxcarlos/WORKING/UBUNTU/14.04X64/OPENCV/OpenCV2.4.7/V1/Release/lib
        LIBS+= -lopencv_core -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann
        LIBS+= -lopencv_gpu -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo
        LIBS+= -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_videostab
    }
}

RESOURCES += \
    Recursos.qrc

