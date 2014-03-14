#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "LabelEventMouse.h"
#include "UTILITYQT.h"
#include "VideoLoadOCV_Thread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void LoadVideoNow(void);
    void ReceiveImage(QImage ImaVideo);
    void ReceiveImageROI(QImage ImaVideoROI);
    void ResizeVerticalSlider(int CountFrame);
    void NextFrameClick(void);
    void BackFrameClick(void);

    void ReceivePosX(QString value);

    void FileSavePositivesDirImages(void);
    void FileSaveNegativeDirImages(void);

    void EnablePos(void);
    void EnableNeg(void);

    void ClickOK(void);
    void StopVideo(void);

    void ChangeSpinBox(int dato);
    void ClickRadioButtonBox(bool State);
    void ClickSpinBoxBoxStatic(int Value);
    void ChangeValueScrollBarAngle(int Value);
    void UpdateRegionExtract(void);

    void ClickChecbox(int Estado);
    void ClickSpinBoxResize(int dato);
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    LabelEventMouse *LabelImagen;
    cv::Mat *imageD;
    QImage FrameD;
    QImage FrameDROI;
    VIDEOLOADOCVTHREAD VideoLoadOCV;
    bool stopthread;
    QFile *WriteReadFileTXT;
    QString DirImages;
    QString DirFileTXT;
    QString DirFileTXTPosHaar;
    QString DirFileTXTPosSVM;
    QString ImageLocationAnt;

    QString DirImagesNeg;
    QString DirFileTXTNeg;
    QString DirFileTXTNegHaar;
    QString DirFileTXTNegSVM;
    int MaxFramesVideo;
    int posSlider;

    int datoAnt;

    void conexiones(void);
    QMessageBox *msgBox;


};

#endif // MAINWINDOW_H
