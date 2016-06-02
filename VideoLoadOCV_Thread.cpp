#include "VideoLoadOCV_Thread.h"
#include <sys/stat.h>
#include <exception>
#ifdef __linux__
#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#ifdef __linux__
#include <unistd.h>
#endif
#ifdef _WIN32
#include "unistd_win.h"
#endif
#include <errno.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#endif
#include<dirent.h>


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
    isVideo=true;
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
        if(isVideo){
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
        }else{
            listFilesImagesOnDir();
        }
        if(LoadVideo->isOpened()==true || ImageListFiles.size()>0){
            emit StateLoadVideo(true);
            if(isVideo){
                FrameCount=(int)LoadVideo->get(CV_CAP_PROP_FRAME_COUNT);
            }else{
                FrameCount = ImageListFiles.size();
            }
            emit FrameCountVideo(FrameCount);
            //cv::Mat Copya;
            cv::Mat FrameAct, FrameActRotate, FrameROI, FrameROIRes, FrameMask;
            FrameROI.create(200,200,CV_8UC1);
            while(Stoped==false){
                if(isVideo){
                    LoadVideo->set(CV_CAP_PROP_POS_FRAMES,(double)GetPosFrame);
                }
                if(GetFrame==true){
                    if(isVideo){
                        LoadVideo->read(FrameAct);
                    }else{
                         FrameAct = cv::imread(ImageListFiles.at(GetPosFrame));
                    }
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
                    if(ROIRec.width>0 && ROIRec.height>0 && ROIRec.x>0 && ROIRec.y>0){
                        ExtractRegionROI(FrameActRotate, FrameROI, ROIRec);
                        ImageVideoROI=MatTOQImage(FrameROI);
                        emit ImageROIFromVideo(ImageVideoROI);
                    }
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
    if(isVideo){
        LoadVideo->release();
    }


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
    isVideo=true;
}

void VIDEOLOADOCVTHREAD::SetParhImageDirectoty(QString PathDir)
{
    PathVideoOCV=PathDir.toStdString();
    isVideo=false;
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

void VIDEOLOADOCVTHREAD::listFilesImagesOnDir(void)
{
    char modeRead = 0; //0 order file, 1 no order file
    short Readonly = 1;
    std::string prefix="";
#ifdef _WIN32
        WIN32_FIND_DATA search_data;
        std::vector<std::string> Lnames;
        std::string ImageLocate;
        bool validExt = false;
        memset(&search_data, 0, sizeof(WIN32_FIND_DATA));
        std::replace(DirectoryConv.begin(), DirectoryConv.end(), '\\','/');
        ImageLocate = DirectoryConv+"/*";
        HANDLE handle = FindFirstFile(ImageLocate.c_str(), &search_data);

        while(handle != INVALID_HANDLE_VALUE)
        {
           //cout<<"\n"<<search_data.cFileName;
           std::string FileName(search_data.cFileName);
           Lnames.push_back(FileName);
           if(FindNextFile(handle, &search_data) == FALSE)
             break;
        }
        //Close the handle after use or memory/resource leak
        FindClose(handle);
        std::sort(Lnames.begin(), Lnames.end(), compareNat);
        if(Lnames.size()>=0){
            for(int i=0;i<Lnames.size();i++){
                ImageLocate=Lnames[i];
                if(ImageLocate.size()>4){
                    validExt = false;
                    if(checkSuffix(ImageLocate, ".jpg")){
                        validExt = true;
                    }else if(checkSuffix(ImageLocate, ".png")){
                        validExt = true;
                    }else if(checkSuffix(ImageLocate, ".bmp")){
                        validExt = true;
                    }else if(checkSuffix(ImageLocate,"ppm")){
                        validExt = true;
                    }else if(checkSuffix(ImageLocate, ".JPG")){
                        validExt = true;
                    }else if(checkSuffix(ImageLocate, ".PNG")){
                        validExt = true;
                    }else if(checkSuffix(ImageLocate, ".BMP")){
                        validExt = true;
                    }else if(checkSuffix(ImageLocate, ".PPM")){
                        validExt = true;
                    }else if(checkSuffix(ImageLocate, ".jpeg")){
                        validExt = true;
                    }

                    if(validExt){
                        ImageLocate = DirectoryConv + "/" + ImageLocate;
                        ImageListFiles.push_back(ImageLocate);
                    }
                }
            }
        }else{
            return (false);
        }
#endif
#ifdef __linux__
        struct dirent **namelist;
        int n,i;
        std::string ImageLocate;
        bool validExt = false;
        if(modeRead == (char)0){
            n = scandir(PathVideoOCV.c_str(), &namelist, 0, versionsort);
            if (n < 0){
                perror("scandir");
            }else{
                for(i =0 ; i < n; ++i){
                    ImageLocate = std::string(namelist[i]->d_name);
                    if(ImageLocate.size()>4){
                        validExt = false;
                        if(Readonly>=(short)0 && Readonly<=(short)2){
                            if(checkSuffix(ImageLocate, ".jpg")){
                                validExt = true;
                            }else if(checkSuffix(ImageLocate, ".png")){
                                validExt = true;
                            }else if(checkSuffix(ImageLocate, ".bmp")){
                                validExt = true;
                            }else if(checkSuffix(ImageLocate,"ppm")){
                                validExt = true;
                            }else if(checkSuffix(ImageLocate, ".JPG")){
                                validExt = true;
                            }else if(checkSuffix(ImageLocate, ".PNG")){
                                validExt = true;
                            }else if(checkSuffix(ImageLocate, ".BMP")){
                                validExt = true;
                            }else if(checkSuffix(ImageLocate, ".PPM")){
                                validExt = true;
                            }else if(checkSuffix(ImageLocate, ".jpeg")){
                                validExt = true;
                            }else if(checkSuffix(ImageLocate, ".tif")){
                                validExt = true;
                            }else if(checkSuffix(ImageLocate, ".TIF")){
                                validExt = true;
                            }else if(checkSuffix(ImageLocate, ".tiff")){
                                validExt = true;
                            }else if(checkSuffix(ImageLocate, ".TIFF")){
                                validExt = true;
                            }

                            if(Readonly==(short)2){
                                if(checkPreffix(ImageLocate, prefix)){
                                    validExt = true;
                                }else{
                                    validExt = false;
                                }
                            }

                            if(Readonly==0){
                                if(checkSuffix(ImageLocate, ".bin")){
                                    validExt = true;
                                }
                            }
                        }else if(Readonly>=3 && Readonly<=4){
                            if(checkSuffix(ImageLocate, ".bin")){
                                if(Readonly==3){
                                    validExt=true;
                                }else if(Readonly==4){
                                    if(checkPreffix(ImageLocate, prefix)){
                                        validExt = true;
                                    }
                                }
                            }
                        }

                        if(validExt){
                            ImageLocate = PathVideoOCV + "/" + ImageLocate;
                            ImageListFiles.push_back(ImageLocate);
                        }
                    }
                    free(namelist[i]);
                }
                free(namelist);
            }
        }
#endif
}

bool VIDEOLOADOCVTHREAD::checkSuffix(const std::string& FI, const std::string& suffix)
{
    return (FI.size() >= suffix.size()) && equal(suffix.rbegin(), suffix.rend(), FI.rbegin());
}

bool VIDEOLOADOCVTHREAD::checkPreffix(const std::string& FI, const std::string& preffix)
{
    bool ban = false;
    //auto mypair = std::mismatch(preffix.begin(), preffix.end(), FI.begin());
    //if(mypair.first==preffix.end()){
    //    ban=true;
    //}
    return(ban);
}

