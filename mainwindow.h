#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include "LabelEventMouse.h"
#include "UTILITYQT.h"
#include "VideoLoadOCV_Thread.h"
#include "processonthread.h"

#if defined(WIN32)
#define WINDOWSSO 1
#endif

#if defined(linux)
#define LINUXSO 1
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void EmitFinProccess(bool State);
public slots:
    void LoadVideoNow(void);
    void ReceiveImage(QImage ImaVideo);
    void ReceiveImageROI(QImage ImaVideoROI);
    void ReceiveImagePOly(QImage ImaVideoPoly);
    void ResizeVerticalSlider(int CountFrame);
    void NextFrameClick(void);
    void BackFrameClick(void);
    void ClickExtract1(void);

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

    void ReceiveErrorProcess(unsigned char code);
    void ErrorProcess(QProcess::ProcessError error);
    void ProcessEnd(int exitCode, QProcess::ExitStatus exitStatus);
    void InitProcessConv(bool state);

    void ChangePage(int index);
    void ClickExecuteHaarSamples(void);
    void ClickBrowseCreateSamples(void);
    void ClickBrowseCreateSamplesOut(void);
    void ClickBrowseCreateSamplesVecOute(void);
    void onStdOutput(void);
    void updateBarHaarSamples(QString State);
    void regionBlacks(QList<QRect> boxes, QString PathSave);
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_checkBoxHide_stateChanged(int arg1);

    void on_pushButtonLoadDirectoryFil_clicked();

    void on_pushButtonROIOK_clicked();

    void on_pushButton_clicked();

    void on_pushButtonROIOK2_clicked();

    void on_pushButtonClearRois_clicked();

    void on_radioButtonBoth_clicked();

    void on_checkBoxHaar_clicked();

    void on_checkBoxSVM_clicked();

private:
    Ui::MainWindow *ui;
    LabelEventMouse *LabelImagen;
    cv::Mat *imageD;
    QImage FrameD;
    QImage FrameDROI;
    QImage ImagePoly;
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
    QString FileNameLocation;
    QProcess *VideoCodec;
    QFile *FileConv;
    bool ExtratbyPoints;
    ProcessOnThread *Proceso;
    int minBarHaar, maxBarHaar, valueBarHaar;
    QList<QImage> LRois;
};

#endif // MAINWINDOW_H
