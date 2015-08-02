#include "VideoLoadOCV_Thread.h"
#include <QDebug>

VIDEOLOADOCVTHREAD::VIDEOLOADOCVTHREAD(QThread *parent):QThread(parent)
{
    qRegisterMetaType<QImage>("QImage");
    GetPosFrame=0;
    GetFrame=true;
    Stoped=false;
    LoadVideo=new cv::VideoCapture();
    //LoadVideo->set(CV_CAP_PROP_FOURCC,CV_FOURCC('A', 'V', 'C', '1'));

    AngleRotate = 0;
    RotateNew = false;

    ExtractNew = false;
    HeightRROI = 100;
    WidthRROI = 100;
    ResizeROINew = false;
    ConvEnd = false;
    ExtractPolyog = false;
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
    QString Codec = "";
    if(PathVideoOCV.size()!=0){
        emit StatePathVerified(true);
        qDebug()<<QString::fromStdString(PathVideoOCV);
        try{
            LoadVideo->open(PathVideoOCV);
            int ex = static_cast<int>(LoadVideo->get(CV_CAP_PROP_FOURCC));
            char EXT[] = {ex & 0XFF , (ex & 0XFF00) >> 8,(ex & 0XFF0000) >> 16,(ex & 0XFF000000) >> 24, 0};
            qDebug()<<"CODEC: "<<EXT;
            Codec.push_back(QChar(EXT[0]));
            Codec.push_back(QChar(EXT[1]));
            Codec.push_back(QChar(EXT[2]));
            Codec.push_back(QChar(EXT[3]));
        } catch (cv::Exception &exc)
        {
            qDebug() << "cv exception: " << exc.what();
        }

        if(Codec == "H264"){
            LoadVideo->release();
            emit EmitProccessConvVideo(true);
            while(!ConvEnd){
                this->msleep(200);
                if(Stoped){
                    break;
                }
            }
            if(!Stoped){
                qDebug()<<"Abriendo arvhio convertido....";
                try{
                    LoadVideo->open("myfile.mp4");
                }catch (cv::Exception &exc){
                    qDebug() << "cv exception: " << exc.what();
                }
            }
            ConvEnd = false;
        }
        if(LoadVideo->isOpened()==true){
            emit StateLoadVideo(true);
            FrameCount=(int)LoadVideo->get(CV_CAP_PROP_FRAME_COUNT);
            emit FrameCountVideo(FrameCount);
            //cv::Mat Copya;
            cv::Mat FrameAct, FrameActRotate, FrameROI, FrameROIRes, FrameMask;
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
                if(ExtractPolyog == true){
                    cv::Mat Mask = cv::Mat::zeros(FrameActRotate.size(), CV_8UC3);
                    cv::Mat Mask2;
                    std::vector<cv::Point> PointsOP;
                    cv::Point** Puntos= new cv::Point*[1];
                    for(int i =0;i<1;i++){
                        int numCols = PointsR.size();
                        Puntos[i]=new cv::Point[numCols];
                        for(int j =0;j<numCols;j++){
                            cv::Point Punto = cv::Point(PointsR[j].x(), PointsR[j].y());
                            PointsOP.push_back(Punto);
                            Puntos[i][j]=cv::Point(Punto);
                        }
                    }
                    const cv::Point* ppt[1] = { Puntos[0] };
                    int npt[] = { PointsR.size() };
                    cv::fillPoly( Mask, ppt, npt, 1, cv::Scalar( 255, 255, 255 ), 8 );
                    cv::bitwise_not(Mask, Mask2);
                    cv::bitwise_and(FrameActRotate, Mask, FrameMask);
                    cv::bitwise_or(FrameMask, Mask2, FrameMask);
                    cv::Rect BOUND = cv::boundingRect(PointsOP);
                    cv::Mat ROI(FrameMask, BOUND);
                    //cv::imwrite("salida.png",ROI);
                    ImageVideoROI=MatTOQImage(ROI);
                    ImagePoly = MatTOQImage(FrameMask);
                    emit ImageROIFromVideo(ImageVideoROI);
                    emit ImagePolyExtract(ImagePoly);
                    ExtractPolyog = false;
                }
                this->msleep(300);
            }
        }else{
            emit StateLoadVideo(false);
            qDebug()<<"THREAD LOADVIDEO CERRANDO...";
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

void VIDEOLOADOCVTHREAD::FinishConv(bool state)
{
    ConvEnd = state;
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

void VIDEOLOADOCVTHREAD::ExtractRegionPloygonal(std::vector<QPoint> Points)
{
    PointsR = Points;
    ExtractPolyog = true;
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
