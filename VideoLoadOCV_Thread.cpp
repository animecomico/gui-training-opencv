#include "VideoLoadOCV_Thread.h"
#include <QDebug>

VIDEOLOADOCVTHREAD::VIDEOLOADOCVTHREAD(QThread *parent):QThread(parent)
{
    qRegisterMetaType<QImage>("QImage");
    GetPosFrame=0;
    GetFrame=true;
    Stoped=false;
    LoadVideo=new cv::VideoCapture();

    AngleRotate = 0;
    RotateNew = false;

    ExtractNew = false;
    HeightRROI = 100;
    WidthRROI = 100;
    ResizeROINew = false;
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
            //cv::Mat Copya;
            cv::Mat FrameAct, FrameActRotate, FrameROI, FrameROIRes;
            FrameROI.create(200,200,CV_8UC1);
            while(Stoped==false){
                LoadVideo->set(CV_CAP_PROP_POS_FRAMES,(double)GetPosFrame);
                if(GetFrame==true){
                    LoadVideo->read(FrameAct);                    
                    FrameAct.copyTo(FrameActRotate);
                    ImageVideo=MatTOQImage(FrameAct);
                    emit ImageFromVideo(ImageVideo);
                    GetFrame=false;                    
                }
                if(RotateNew == true){                    
                    ApplyRotate(FrameAct, FrameActRotate, AngleRotate);
                    ImageVideo=MatTOQImage(FrameActRotate);
                    emit ImageFromVideo(ImageVideo);
                    RotateNew = false;
                }

                if(ExtractNew == true){
                    ExtractRegionROI(FrameActRotate, FrameROI, ROIRec);
                    ImageVideoROI=MatTOQImage(FrameROI);
                    emit ImageROIFromVideo(ImageVideoROI);
                    ExtractNew = false;
                }

                if(ResizeROINew == true){
                    ResizeImage(FrameROI, FrameROIRes, HeightRROI, WidthRROI);
                    ImageVideoROI=MatTOQImage(FrameROIRes);
                    emit ImageROIFromVideo(ImageVideoROI);
                    ResizeROINew = false;
                }
                this->msleep(300);
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

void VIDEOLOADOCVTHREAD::ApplyRotate(cv::Mat &Src, cv::Mat &Dst, float angle)
{
    cv::Point2f src_center(Src.cols/2.0F, Src.rows/2.0F);
    cv::Mat rot_mat = cv::getRotationMatrix2D(src_center, (double)angle, 1.0);
    cv::warpAffine(Src, Dst, rot_mat, Src.size(),cv::INTER_LINEAR,cv::BORDER_CONSTANT,cv::Scalar(255,255,255));
}

void VIDEOLOADOCVTHREAD::UpdateAngleRotate(float angle)
{
    AngleRotate = angle;
    RotateNew = true;
}

void VIDEOLOADOCVTHREAD::ExtractRegionROI(cv::Mat &Src, cv::Mat &Dst, cv::Rect &ROIR)
{
    cv::Mat Temp = Src(ROIR);
    Dst.create(Temp.size(),Temp.type());
    Temp.copyTo(Dst);
}

void VIDEOLOADOCVTHREAD::UpdateExtractROI(int X, int Y, int height, int width)
{
    ROIRec.x = X;
    ROIRec.y = Y;
    ROIRec.height = height;
    ROIRec.width = width;
    ExtractNew = true;
    //qDebug()<<"Recta:  "<<ROIRec.x<<" "<<ROIRec.y<<" "<<ROIRec.height<<" "<<ROIRec.width;
}

void VIDEOLOADOCVTHREAD::ResizeImage(cv::Mat &Src, cv::Mat &Dst, int SH, int SW)
{
    cv::Mat Temp(SH, SW, Src.type());
    Dst.create(Temp.size(),Temp.type());
    cv::resize(Src, Temp, Temp.size(),0,0,cv::INTER_CUBIC);
    Temp.copyTo(Dst);
}

void VIDEOLOADOCVTHREAD::UpdateResizeROI(int Height, int Width)
{
    HeightRROI = Height;
    WidthRROI = Width;
    ResizeROINew = true;
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
