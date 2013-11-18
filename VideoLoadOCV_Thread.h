#ifndef VIDEOLOADOCV_THREAD_H
#define VIDEOLOADOCV_THREAD_H

#include <QThread>
#include <QImage>
#include <QObject>
#include <QMetaType>
#include <exception>

#include "UTILITYQT.h"

class VIDEOLOADOCVTHREAD:public QThread
{
    Q_OBJECT
public:
    VIDEOLOADOCVTHREAD(QThread *parent=0);
    ~VIDEOLOADOCVTHREAD();

    void run();
    void SetPathVideoFile(QString PathVideo);
signals:
    void ImageFromVideo(QImage);
    void FrameCountVideo(int);
    void StateLoadVideo(bool);
    void StatePathVerified(bool);
public slots:
    void ReceiveValFrameActual(int ValuePosAct);
    void StopThread(bool value);
private:
    int GetPosFrame;
    bool GetFrame;
    bool Stoped;
    int FrameCount;
    std::string PathVideoOCV;
    cv::VideoCapture *LoadVideo;
    QImage ImageVideo;
};



#endif // VIDEOLOADOCV_THREAD_H
