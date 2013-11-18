#include "VideoLoadOCV_Thread.h"
#include <QDebug>

VIDEOLOADOCVTHREAD::VIDEOLOADOCVTHREAD(QThread *parent):QThread(parent)
{
    qRegisterMetaType<QImage>("QImage");
    GetPosFrame=0;
    GetFrame=true;
    Stoped=false;
    LoadVideo=new cv::VideoCapture();
}

VIDEOLOADOCVTHREAD::~VIDEOLOADOCVTHREAD()
{
    if(this->isRunning()){
        Stoped = true;
        this->wait(100);
    }
    qDebug()<<"LIBERANDOVIDEO...";
}

void VIDEOLOADOCVTHREAD::run()
{
    qDebug()<<"THREAD INICIADO...";

    if(PathVideoOCV.size()!=0){
        emit StatePathVerified(true);
        qDebug()<<QString::fromStdString(PathVideoOCV);
        try
        {
            throw LoadVideo->open(PathVideoOCV);

        } catch (cv::Exception &exc)
        {
            qDebug() << "cv exception: " << exc.what();
        } catch(...)
        {
            qDebug() << "unknown exception";
        }


        if(LoadVideo->isOpened()==true){
            emit StateLoadVideo(true);
            FrameCount=(int)LoadVideo->get(CV_CAP_PROP_FRAME_COUNT);
            emit FrameCountVideo(FrameCount);
            while(Stoped==false){
                LoadVideo->set(CV_CAP_PROP_POS_FRAMES,(double)GetPosFrame);
                if(GetFrame==true){
                    cv::Mat FrameAct;
                    LoadVideo->read(FrameAct);
                    ImageVideo=MatTOQImage(FrameAct);
                    emit ImageFromVideo(ImageVideo);
                    GetFrame=false;
                }
                this->msleep(200);
            }
        }else{
            emit StateLoadVideo(false);
            qDebug()<<"THREAD LOADVIDEO...";
            //std::cout<<PathVideoOCV;
        }
    }else{
        emit StatePathVerified(false);
    }

    Stoped=false;
    GetFrame=true;
    GetPosFrame=0;

    LoadVideo->release();


    qDebug()<<"THREAD VIDEO TERMINADO...";
}

void VIDEOLOADOCVTHREAD::SetPathVideoFile(QString PathVideo)
{
    PathVideoOCV=PathVideo.toStdString();
    //std::cout<<"FILEPATH(OCVTHREAD):  "<<PathVideoOCV<<std::endl;
}

void VIDEOLOADOCVTHREAD::ReceiveValFrameActual(int ValuePosAct)
{
    GetPosFrame=ValuePosAct;
    GetFrame=true;
    //qDebug()<<"Value Changed";
}

void VIDEOLOADOCVTHREAD::StopThread(bool value)
{
    Stoped=value;
}
