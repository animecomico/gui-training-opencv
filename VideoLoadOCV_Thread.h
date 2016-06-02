#ifndef VIDEOLOADOCV_THREAD_H
#define VIDEOLOADOCV_THREAD_H

#include <QThread>
#include <QImage>
#include <QObject>
#include <QMetaType>
#include <exception>
#include <QMutex>
#include <QPoint>

#include "UTILITYQT.h"

#if defined(WIN32)
#define WINDOWSSO 1
#endif

#if defined(linux)
#define LINUXSO 1
#endif

class VIDEOLOADOCVTHREAD:public QThread
{
    Q_OBJECT
public:
    VIDEOLOADOCVTHREAD(QThread *parent=0);
    ~VIDEOLOADOCVTHREAD();

    void run();
    void SetPathVideoFile(QString PathVideo);
    void SetParhImageDirectoty(QString PathDir);
signals:
    void ImageFromVideo(QImage);
    void FrameCountVideo(int);
    void StateLoadVideo(bool);
    void StatePathVerified(bool);
    void ImageROIFromVideo(QImage);
    void EmitErrorProccess(unsigned char code);
    void EmitProccessConvVideo(bool state);
    void ImagePolyExtract(QImage);
public slots:
    void ReceiveValFrameActual(int ValuePosAct);
    void UpdateAngleRotate(float angle);
    void UpdateExtractROI(int X, int Y, int height, int width);
    void UpdateResizeROI(int Height, int Width);
    void ExtractRegionPloygonal(std::vector<QPoint> Points);
    void StopThread(bool value);
    void FinishConv(bool state);
private:
    void ApplyRotate(cv::Mat &Src, cv::Mat &Dst, float angle);
    void ExtractRegionROI(cv::Mat &Src, cv::Mat &Dst, cv::Rect &ROIR);
    void ResizeImage(cv::Mat &Src, cv::Mat &Dst, int SH, int SW);
    void listFilesImagesOnDir(void);
    bool checkPreffix(const std::string& FI, const std::string& preffix);
    bool checkSuffix(const std::string& FI, const std::string& suffix);
    int GetPosFrame;
    bool GetFrame;
    bool Stoped;
    int FrameCount;
    std::string PathVideoOCV;
    cv::VideoCapture *LoadVideo;
    QImage ImageVideo;
    QImage ImageVideoROI;
    QImage ImagePoly;
    QMutex lock;
    float AngleRotate;
    bool RotateNew;
    cv::Rect ROIRec;
    bool ExtractNew;
    int HeightRROI;
    int WidthRROI;
    bool ResizeROINew;
    bool ConvEnd;
    bool ExtractPolyog;
    std::vector<QPoint> PointsR;
    bool isVideo;
    std::vector<std::string> ImageListFiles;
};



#endif // VIDEOLOADOCV_THREAD_H
