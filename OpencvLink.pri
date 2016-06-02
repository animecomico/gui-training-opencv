#Opencv Linkado QT Creator
DEFINES+=USE_OPENCV
win32{
   PC_HOST += WINDOWS
   PATH_OPENCV_SDK=D:/BIBLIOTECA/WINDOWS/OPENCV/3.0_vs2013

}

linux-g++ | linux-g++-64 {
    PC_HOST += LINUX
    #PATH_OPENCV_SDK=/home/carlosx64/Escritorio/BUILDS/OpenCV/OpenCV3.0-Final     #Se especifica donde esta la SDK de OpenCV a Usar
    #PATH_OPENCV_SDK=/home/carlos/Escritorio/BUILDS/OPENCV/OpenCV3.0.0V1         #Se especifica donde esta la SDK de OpenCV a Usar
    PATH_OPENCV_SDK=/usr/local         #Se especifica donde esta la SDK de OpenCV a Usar

    #PATH_OPENCV_SDK=/home/carlosx64/Escritorio/BUILDS/OpenCV/OpenCV2.4.11V2
    #PATH_OPENCV_SDK=/home/carlos/Escritorio/BUILDS/OPENCV/OpenCV2.4.11ModV1
}

OpenCV_Ver=3                                                                #Se especifica la version de OpenCV a usar
#OpenCV_Ver=2

greaterThan(QT_MAJOR_VERSION, 4) {

    message(Qt Version 5)
}

#PATH_OPENCV_SDK=/home/carlos/Escritorio/BUILDS/OPENCV/Open2.4.11V1

CONFIG(debug, debug|release){
    contains(PC_HOST, WINDOWS){
        message(S.O. WINDOWS Debug)
        win32-g++{ #Para MinGW
        }
        win32-msvc2010{ #Para Visual C++ 2010
        }
        win32-msvc2012{ #Para Visual C++ 2012/3
        }
        win32-msvc2013{ #Para Visual C++ 2012/3
            message(msvc 2012/3)
            INCLUDEPATH+=$$PATH_OPENCV_SDK/include
            LIBS+=-L$$PATH_OPENCV_SDK/x64/vc12/lib
            contains(OpenCV_Ver, 3){
                message(OpenCV 3)
                LIBS+=-lopencv_core300d -lopencv_highgui300d -lopencv_imgcodecs300d -lopencv_imgproc300d -lopencv_ml300d
                LIBS+=-lopencv_objdetect300d -lopencv_photo300d -lopencv_stitching300d -lopencv_video300d -lopencv_videoio300d
                LIBS+=-lopencv_videostab300d -lopencv_ts300d -lopencv_calib3d300d
                DEFINES+=OPENCV3
            }
            contains(OpenCV_Ver, 2){
                message(OpenCV 2)
                LIBS+=-lopencv_gpu -lopencv_legacy
                DEFINES+=OPENCV2
            }

        }
    }

    contains(PC_HOST, LINUX){
        message(S.O. LINUX Debug)
        INCLUDEPATH+=$$PATH_OPENCV_SDK/include
        LIBS+=-L$$PATH_OPENCV_SDK/lib
        contains(OpenCV_Ver, 3){
            message(OpenCV 3)
            LIBS+=-lopencv_videoio -lopencv_shape -lopencv_imgcodecs
            DEFINES+=OPENCV3
        }
        contains(OpenCV_Ver, 2){
            message(OpenCV 2)OpencvLink.pri
            LIBS+=-lopencv_gpu -lopencv_legacy -lopencv_contrib #-lopencv_nonfree
            DEFINES+=OPENCV2
        }
        LIBS+=-lopencv_core -lopencv_highgui -lopencv_imgproc #-lopencv_contrib
        LIBS+=-lopencv_calib3d -lopencv_features2d -lopencv_flann -lopencv_ml
        LIBS+=-lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres
        LIBS+=-lopencv_video -lopencv_videostab
    }
}

CONFIG(release, debug|release){
    contains(PC_HOST, WINDOWS){
        message(S.O. WINDOWS Release)
            win32-msvc2013{ #Para Visual C++ 2012/3
            message(msvc 2012/3)
            INCLUDEPATH+=$$PATH_OPENCV_SDK/include
            LIBS+=-L$$PATH_OPENCV_SDK/x64/vc12/lib
            contains(OpenCV_Ver, 3){
                message(OpenCV 3)
                LIBS+=-lopencv_core300 -lopencv_highgui300 -lopencv_imgcodecs300 -lopencv_imgproc300 -lopencv_ml300
                LIBS+=-lopencv_objdetect300 -lopencv_photo300 -lopencv_stitching300 -lopencv_video300 -lopencv_videoio300
                LIBS+=-lopencv_videostab300 -lopencv_ts300 -lopencv_calib3d300
                DEFINES+=OPENCV3
            }
            contains(OpenCV_Ver, 2){
                message(OpenCV 2)
                LIBS+=-lopencv_gpu -lopencv_legacy
                DEFINES+=OPENCV2
            }

        }
    }

    contains(PC_HOST, LINUX){
        message(S.O. LINUX Release)
        message(S.O. LINUX Debug)
        INCLUDEPATH+=$$PATH_OPENCV_SDK/include
        LIBS+=-L$$PATH_OPENCV_SDK/lib
        contains(OpenCV_Ver, 3){
            message(OpenCV 3)
            LIBS+=-lopencv_videoio -lopencv_shape -lopencv_imgcodecs
            DEFINES+=OPENCV3
        }
        contains(OpenCV_Ver, 2){
            message(OpenCV 2)
            LIBS+=-lopencv_gpu -lopencv_legacy -lopencv_contrib #-lopencv_nonfree
            DEFINES+=OPENCV2
        }
        LIBS+=-lopencv_core -lopencv_highgui -lopencv_imgproc #-lopencv_contrib
        LIBS+=-lopencv_calib3d -lopencv_features2d -lopencv_flann -lopencv_ml
        LIBS+=-lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres
        LIBS+=-lopencv_video -lopencv_videostab
        
        
    }
}
