#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    qRegisterMetaType<QImage>("QImage");
    stopthread=false;
    ui->setupUi(this);
    LabelImagen= new LabelEventMouse(this);
    imageD=new cv::Mat();
    //*imageD=cv::imread(":R1/images/FAIRY1.jpg",0);
    //FrameD=MatTOQImage(*imageD);
    ui->scrollAreaIMAGE->setWidget(LabelImagen);

    FrameD.load(":R1/images/FAIRY1.jpg");
    LabelImagen->setPixmap(QPixmap::fromImage(FrameD));

    MaxFramesVideo=0;
    posSlider=0;
    this->conexiones();

    ImageLocationAnt="////";
    datoAnt=0;

}

MainWindow::~MainWindow()
{
    delete ui;    
    //VideoLoadOCV.terminate();

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug()<<"CERRANDO...";
    VideoLoadOCV.StopThread(true);
    //VideoLoadOCV.quit();
    //VideoLoadOCV.wait(2);
}

void MainWindow::StopVideo(void)
{
    VideoLoadOCV.StopThread(true);
    FrameD.load(":R1/images/FAIRY1.jpg");
    LabelImagen->setPixmap(QPixmap::fromImage(FrameD));
}

void MainWindow::conexiones(void)
{
    this->connect(ui->pushButtonLoadVideo,SIGNAL(clicked()),this,SLOT(LoadVideoNow()));
    this->connect(ui->verticalSliderVIDEO,SIGNAL(valueChanged(int)),&VideoLoadOCV,SLOT(ReceiveValFrameActual(int)));
    ///this->connect(ui->verticalSliderVIDEO,SIGNAL(valueChanged(int)),ui->spinBoxINFOTXT,SLOT(setValue(int)));
    this->connect(ui->verticalSliderVIDEO,SIGNAL(valueChanged(int)),this,SLOT(ChangeSpinBox(int)));
    this->connect(&VideoLoadOCV,SIGNAL(ImageFromVideo(QImage)),this,SLOT(ReceiveImage(QImage)));
    this->connect(&VideoLoadOCV,SIGNAL(FrameCountVideo(int)),this,SLOT(ResizeVerticalSlider(int)));

    this->connect(ui->pushButtonNEXT,SIGNAL(clicked()),this,SLOT(NextFrameClick()));
    this->connect(ui->pushButtonBACK,SIGNAL(clicked()),this,SLOT(BackFrameClick()));

    this->connect(LabelImagen,SIGNAL(linkActivated(QString)),this,SLOT(ReceivePosX(QString)));

    this->connect(ui->pushButtonPositiveDir,SIGNAL(clicked()),this,SLOT(FileSavePositivesDirImages()));
    this->connect(ui->pushButtonNegativeDir,SIGNAL(clicked()),this,SLOT(FileSavePositivesDirImages()));

    this->connect(ui->radioButtonNegative,SIGNAL(clicked()),this,SLOT(EnableNeg()));
    this->connect(ui->radioButtonPositive,SIGNAL(clicked()),this,SLOT(EnablePos()));

    this->connect(ui->pushButtonOK,SIGNAL(clicked()),this,SLOT(ClickOK()));

    this->connect(ui->pushButtonStopVideo,SIGNAL(clicked()),this,SLOT(StopVideo()));

    //this->connect(&VideoLoadOCV,SIGNAL(FrameCountVideo(int)), ui->labelSliderFrame, SLOT(setNum(int)));
}

void MainWindow::LoadVideoNow(void)
{
    QString fileName;

    if(VideoLoadOCV.isRunning()){
        //qDebug()<<"HILO YA ESTA CORRIENDO...";
        VideoLoadOCV.StopThread(true);
    }

    fileName = QFileDialog::getOpenFileName(this, tr("OPEN VIDEOS"), QDir::homePath(),
                tr("files AVI (*.avi *AVI);;files MP4 (*.mp4 *.MP4);;files FLV (*.flv *.FLV)"));
    if(fileName.size()>0){
       //qDebug()<<"FILE ANTES::  "<<fileName;
        VideoLoadOCV.SetPathVideoFile(fileName);
        VideoLoadOCV.start();

    }else{
        qDebug()<<"FILENAME VACIO";
    }
}

void MainWindow::ReceiveImage(QImage ImaVideo)
{
    FrameD=ImaVideo;
    LabelImagen->setPixmap(QPixmap::fromImage(FrameD));
}

void MainWindow::ResizeVerticalSlider(int CountFrame)
{
    MaxFramesVideo=CountFrame-2;
    ui->labelSliderFrame->setNum(CountFrame-2);
    ui->verticalSliderVIDEO->setMaximum((CountFrame-2));
    int valor=ui->spinBoxINFOTXT->value();
    ui->verticalSliderVIDEO->setSliderPosition(valor);
}

void MainWindow::NextFrameClick(void)
{
    int posAct=0;
    posAct=ui->verticalSliderVIDEO->sliderPosition();
    posAct++;
    if(posAct>MaxFramesVideo){
        posAct=MaxFramesVideo;
    }

    ui->verticalSliderVIDEO->setSliderPosition(posAct);
    ///this->ChangeSpinBox(posAct);
    ui->labelHeightBox->setText("0");
    ui->labelWidthBox->setText("0");
    ui->labelPosXi->setText("0");
    ui->labelPosYi->setText("0");
}

void MainWindow::BackFrameClick(void)
{
    int posAct=0;
    posAct=ui->verticalSliderVIDEO->sliderPosition();
    posAct--;
    if(posAct<0){
        posAct=0;
    }

    ui->verticalSliderVIDEO->setSliderPosition(posAct);    
    ///this->ChangeSpinBox(posAct);

    ui->labelHeightBox->setText("0");
    ui->labelWidthBox->setText("0");
    ui->labelPosXi->setText("0");
    ui->labelPosYi->setText("0");
}

void MainWindow::ReceivePosX(QString value)
{
    //int ValueN=value.toInt();
    //qDebug()<<"POSX INICIO:  "<<value;
    QStringList list=value.split(":");
    //qDebug()<<"POSX INICIO list:  "<<list;
    if(list.at(1)=="Xinit"){
        //qDebug()<<"POSX INIT:  "<<list.at(0);
        ui->labelPosXi->setText(list.at(0));
    }else{
        if(list.at(1)=="Yinit"){
            //qDebug()<<"POSY INIT:  "<<list.at(0);
            ui->labelPosYi->setText(list.at(0));
        }else{
            if(list.at(1)=="HeightBox"){
                //qDebug()<<"POSY INIT:  "<<list.at(0);
                ui->labelHeightBox->setText(list.at(0));
            }else{
                if(list.at(1)=="WidthBox"){
                    //qDebug()<<"POSY INIT:  "<<list.at(0);
                    ui->labelWidthBox->setText(list.at(0));
                }
            }
        }
    }

}

void MainWindow::FileSavePositivesDirImages(void)
{
    DirImages.clear();
    DirFileTXT.clear();

    ui->lineEditTXTDir->clear();
    ui->lineEditImageDirOutput->clear();

    QString fileNameTXT,dirNameImages;
    fileNameTXT = QFileDialog::getSaveFileName(this, tr("Save TXT"), QDir::homePath(),
                tr("files TXT (*.txt *.TXT)"));
    if(fileNameTXT.size()!=0){

       dirNameImages = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        QDir::homePath(),
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);

       DirImages=dirNameImages;
       DirFileTXT=fileNameTXT;

       ui->lineEditTXTDir->setText(fileNameTXT);
       ui->lineEditImageDirOutput->setText(dirNameImages);

       int contFilas=0;
       QFile file(fileNameTXT);

       //WriteReadFileTXT.open("fileNameTXT",QIODevice::ReadWrite | QIODevice::Text);
       file.open(QIODevice::ReadWrite | QIODevice::Text);
       QTextStream outStream(&file);



       QString line;
       while (!outStream.atEnd()){
           line = outStream.readLine();
           contFilas++;
       }

       ui->spinBoxINFOTXT->setValue(contFilas);

       //qDebug()<<"Lineas:  "<<cont;

       /* Write the line to the file */
       //outStream << "Victory!\n";
       //outStream << "Victory2!\n";

       /* Close the file */
       file.close();
  }
}

void MainWindow::EnablePos(void)
{
    ui->pushButtonNegativeDir->setEnabled(false);
    ui->pushButtonPositiveDir->setEnabled(true);
}

void MainWindow::EnableNeg(void)
{
    ui->pushButtonNegativeDir->setEnabled(true);
    ui->pushButtonPositiveDir->setEnabled(false);
}

void MainWindow::ClickOK(void)
{

    QString NamePos="Positiva";
    QString NameNeg="Negativa";
    QString ImageLocalization;
    QString FramePos=QString::number(ui->spinBoxINFOTXT->value());
    QString PosXini=ui->labelPosXi->text();
    QString PosYini=ui->labelPosYi->text();
    QString HeightBox=ui->labelHeightBox->text();
    QString WidthBox=ui->labelWidthBox->text();
    QString ImageLocationFromUser = ui->lineEditImageDirOutput->text();

    if(DirImages.size()!=0){
        if(DirFileTXT.size()!=0){
            if(ui->radioButtonPositive->isChecked()){
               NamePos="/"+NamePos+FramePos+".png";
               ImageLocalization=DirImages+NamePos;
               ImageLocationFromUser = ImageLocationFromUser + NamePos;
               if(ImageLocationAnt!=ImageLocalization){
                   QFile file(DirFileTXT);
                   file.open(QIODevice::ReadWrite |QIODevice::Append | QIODevice::Text);
                   QTextStream outStream(&file);

                   //outStream <<ImageLocalization<<" "<<"1"<<" "<<PosXini<<" "<<PosYini<<" "<<WidthBox<<" "<<HeightBox<<"\n";
                   outStream <<ImageLocationFromUser<<" "<<"1"<<" "<<PosXini<<" "<<PosYini<<" "<<WidthBox<<" "<<HeightBox<<"\n";

                   file.close();

                   FrameD.save(ImageLocalization,"png");
                   ImageLocationAnt=ImageLocalization;
               }

            }else{
                if(ui->radioButtonNegative->isChecked()){
                    NameNeg="/"+NameNeg+FramePos+".png";
                    ImageLocalization=DirImages+NameNeg;
                    if(ImageLocationAnt!=ImageLocalization){
                        QFile file(DirFileTXT);
                        file.open(QIODevice::ReadWrite |QIODevice::Append | QIODevice::Text);
                        QTextStream outStream(&file);

                        outStream <<ImageLocalization<<"\n";
                        file.close();

                        FrameD.save(ImageLocalization,"png");
                        ImageLocationAnt=ImageLocalization;
                    }
                }
            }
        }else{
            qDebug()<<"DEBE INDICAR UN ARCHIVO .txt PARA LAMECNAR LA INFO...";
        }
    }else{
        qDebug()<<"DEBE INDICAR UN DIRECTORIO DONDE SE VA ALMACENAR LAS IMAGENES (POSITIVAS O NEGATIVAS)...";
    }


}

void MainWindow::ChangeSpinBox(int dato)
{
    int ValorSpin = ui->spinBoxINFOTXT->value();
    if(dato>datoAnt){
        if(ValorSpin>=0 && ValorSpin<ui->spinBoxINFOTXT->maximum()){
            ValorSpin++;
        }
    }else{
        if(ValorSpin>0){
            ValorSpin--;
        }else{
            ValorSpin=0;
        }
    }

    ui->spinBoxINFOTXT->setValue(ValorSpin);

    datoAnt=dato;

    //qDebug()<<ValorSpin;
}
