#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    void ResizeVerticalSlider(int CountFrame);
    void NextFrameClick(void);
    void BackFrameClick(void);

    void ReceivePosX(QString value);

    void FileSavePositivesDirImages(void);

    void EnablePos(void);
    void EnableNeg(void);

    void ClickOK(void);
    void StopVideo(void);

    void ChangeSpinBox(int dato);

protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    LabelEventMouse *LabelImagen;
    cv::Mat *imageD;
    QImage FrameD;
    VIDEOLOADOCVTHREAD VideoLoadOCV;
    bool stopthread;
    QFile *WriteReadFileTXT;
    QString DirImages;
    QString DirFileTXT;
    QString ImageLocationAnt;

    int MaxFramesVideo;
    int posSlider;

    int datoAnt;

    void conexiones(void);


};

#endif // MAINWINDOW_H
