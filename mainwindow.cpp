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
    msgBox = new QMessageBox(this);
    msgBox->setModal(false);
    msgBox->setWindowTitle("Warning Direction Location");
    msgBox->setText("Bad configuration Paths.                                        ");
    msgBox->setIcon(QMessageBox::Warning);

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
    this->connect(ui->pushButtonNegativeDir,SIGNAL(clicked()),this,SLOT(FileSaveNegativeDirImages()));

    this->connect(ui->radioButtonNegative,SIGNAL(clicked()),this,SLOT(EnableNeg()));
    this->connect(ui->radioButtonPositive,SIGNAL(clicked()),this,SLOT(EnablePos()));

    this->connect(ui->pushButtonOK,SIGNAL(clicked()),this,SLOT(ClickOK()));

    this->connect(ui->pushButtonStopVideo,SIGNAL(clicked()),this,SLOT(StopVideo()));

    //this->connect(&VideoLoadOCV,SIGNAL(FrameCountVideo(int)), ui->labelSliderFrame, SLOT(setNum(int)));
    this->connect(ui->radioButtonBoxDynamic,SIGNAL(clicked(bool)), this,SLOT(ClickRadioButtonBox(bool)));
    this->connect(ui->radioButtonBoxStatic,SIGNAL(clicked(bool)), this,SLOT(ClickRadioButtonBox(bool)));
    this->connect(ui->spinBoxHeightBoxStatic,SIGNAL(valueChanged(int)), this,SLOT(ClickSpinBoxBoxStatic(int)));
    this->connect(ui->spinBoxWidthBoxStatic,SIGNAL(valueChanged(int)), this,SLOT(ClickSpinBoxBoxStatic(int)));

    this->connect(ui->horizontalSliderAngle,SIGNAL(valueChanged(int)),this,SLOT(ChangeValueScrollBarAngle(int)));
    this->connect(LabelImagen, SIGNAL(EmitRegionExtract()),this,SLOT(UpdateRegionExtract()));
    this->connect(&VideoLoadOCV, SIGNAL(ImageROIFromVideo(QImage)), this,SLOT(ReceiveImageROI(QImage)));

    this->connect(ui->checkBoxSVM,SIGNAL(stateChanged(int)),this,SLOT(ClickChecbox(int)));
    this->connect(ui->checkBoxResizeEXtract,SIGNAL(stateChanged(int)),this,SLOT(ClickChecbox(int)));
    this->connect(ui->spinBoxResHeight,SIGNAL(valueChanged(int)),this,SLOT(ClickSpinBoxResize(int)));
    this->connect(ui->spinBoxResWidth,SIGNAL(valueChanged(int)),this,SLOT(ClickSpinBoxResize(int)));
}

void MainWindow::ClickRadioButtonBox(bool State)
{
    QRadioButton *RadioButtonSelect = qobject_cast<QRadioButton *>(sender());
        if(!RadioButtonSelect->isChecked()){
            RadioButtonSelect->setChecked(true);
        }else{
            if(RadioButtonSelect->objectName() == "radioButtonBoxDynamic"){
                ui->radioButtonBoxDynamic->setChecked(true);
                ui->radioButtonBoxStatic->setChecked(false);
                ui->spinBoxHeightBoxStatic->setEnabled(false);
                ui->spinBoxWidthBoxStatic->setEnabled(false);
                ui->labelHeightBoxStatic->setEnabled(false);
                ui->labelWidthBoxStatic->setEnabled(false);
                LabelImagen->setTypeBox((unsigned char)0);
            }else if(RadioButtonSelect->objectName() == "radioButtonBoxStatic"){
                ui->radioButtonBoxDynamic->setChecked(false);
                ui->radioButtonBoxStatic->setChecked(true);
                ui->spinBoxHeightBoxStatic->setEnabled(true);
                ui->spinBoxWidthBoxStatic->setEnabled(true);
                ui->labelHeightBoxStatic->setEnabled(true);
                ui->labelWidthBoxStatic->setEnabled(true);
                LabelImagen->setTypeBox((unsigned char)1);
                LabelImagen->UpdateHeightandWidthBox(ui->spinBoxHeightBoxStatic->value(),
                                                     ui->spinBoxWidthBoxStatic->value());
            }
        }
}

void MainWindow::ClickSpinBoxBoxStatic(int Value)
{
    int y = Value;
    y = 0;
    LabelImagen->UpdateHeightandWidthBox(ui->spinBoxHeightBoxStatic->value(),
                                         ui->spinBoxWidthBoxStatic->value());
}

void MainWindow::ClickSpinBoxResize(int dato)
{
    QSpinBox *spinBox = qobject_cast<QSpinBox *>(sender());
    if(ui->checkBoxResizeEXtract->isEnabled()){
        if(ui->checkBoxResizeEXtract->isChecked()){
            if(spinBox->objectName() == "spinBoxResHeight"){
                VideoLoadOCV.UpdateResizeROI(dato, ui->spinBoxResWidth->value());
            }else if(spinBox->objectName() == "spinBoxResWidth"){
                VideoLoadOCV.UpdateResizeROI(ui->spinBoxResHeight->value(), dato);
            }
        }
    }
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

void MainWindow::ReceiveImageROI(QImage ImaVideoROI)
{
    FrameDROI = ImaVideoROI;
    ui->labelImageExtract->setPixmap(QPixmap::fromImage(FrameDROI));
}

void MainWindow::ClickChecbox(int Estado)
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    if(checkBox->objectName() == "checkBoxResizeEXtract"){
        qDebug()<<"CheckBox EXtract";
        if(Estado!=0){
            VideoLoadOCV.UpdateResizeROI(ui->spinBoxResHeight->value(), ui->spinBoxResWidth->value());
        }else{
            VideoLoadOCV.UpdateResizeROI(ui->labelHeightBox->text().toInt(), ui->labelWidthBox->text().toInt());
        }
    }else if(checkBox->objectName() == "checkBoxSVM"){
        //qDebug()<<"ChecBox SVM: "<<Estado;
        if(Estado!=0){
            ui->checkBoxResizeEXtract->setEnabled(true);
            ui->labelResHeight->setEnabled(true);
            ui->labelResWidth->setEnabled(true);
            ui->spinBoxResHeight->setEnabled(true);
            ui->spinBoxResWidth->setEnabled(true);
        }else{
            ui->checkBoxResizeEXtract->setEnabled(false);
            ui->labelResHeight->setEnabled(false);
            ui->labelResWidth->setEnabled(false);
            ui->spinBoxResHeight->setEnabled(false);
            ui->spinBoxResWidth->setEnabled(false);
        }
    }

}

void MainWindow::ResizeVerticalSlider(int CountFrame)
{
    MaxFramesVideo=CountFrame-2;
    ui->labelSliderFrame->setNum(CountFrame-2);
    ui->verticalSliderVIDEO->setMaximum((CountFrame-2));
    int valor=ui->spinBoxINFOTXT->value();
    ui->verticalSliderVIDEO->setSliderPosition(valor);
}

void MainWindow::ChangeValueScrollBarAngle(int Value)
{
    //qDebug()<<"Angulo: "<<Value;
    ui->labelAngleValue->setText(QString::number(Value));
    VideoLoadOCV.UpdateAngleRotate((float)Value);
}

void MainWindow::NextFrameClick(void)
{
    int posAct=0;
    posAct=ui->verticalSliderVIDEO->sliderPosition();
    posAct++;    
    VideoLoadOCV.UpdateAngleRotate((float)ui->horizontalSliderAngle->value());
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
                //qDebug()<<"HEIGHT:  "<<list.at(0);
                ui->labelHeightBox->setText(list.at(0));
            }else{
                if(list.at(1)=="WidthBox"){
                    //qDebug()<<"WIDTH:  "<<list.at(0);
                    ui->labelWidthBox->setText(list.at(0));
                }
            }
        }
    }

}

void MainWindow::UpdateRegionExtract(void)
{
    if(ui->labelPosXi->text().toInt()!=0 && ui->labelPosYi->text().toInt()!=0){
        VideoLoadOCV.UpdateExtractROI(ui->labelPosXi->text().toInt(), ui->labelPosYi->text().toInt(),
                                      ui->labelHeightBox->text().toInt(),ui->labelWidthBox->text().toInt());
    }
    if(ui->checkBoxResizeEXtract->isEnabled()){
        if(ui->checkBoxResizeEXtract->isChecked()){
            VideoLoadOCV.UpdateResizeROI(ui->spinBoxResHeight->value(), ui->spinBoxResWidth->value());
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
    fileNameTXT = QFileDialog::getSaveFileName(this, tr("Save TXT Positive"), QDir::homePath(),
                tr("files TXT (*.txt *.TXT)"));
    if(fileNameTXT.size()!=0){

       dirNameImages = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        QDir::homePath(),
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);

       DirImages=dirNameImages;
       DirFileTXT=fileNameTXT;
       DirFileTXTPosHaar = DirFileTXT;
       DirFileTXTPosSVM = DirFileTXT;
       DirFileTXTPosHaar.remove(DirFileTXTPosHaar.size()-4,4);
       DirFileTXTPosHaar = DirFileTXTPosHaar + "_Haar.txt";
       DirFileTXTPosSVM.remove(DirFileTXTPosSVM.size()-4,4);
       DirFileTXTPosSVM = DirFileTXTPosSVM + "_SVM.txt";

       ui->lineEditTXTDir->setText(fileNameTXT);
       ui->lineEditImageDirOutput->setText(dirNameImages);

       if(ui->checkBoxHaar->isChecked()){
           int contFilas=0;
           QFile file(DirFileTXTPosHaar);
           file.open(QIODevice::ReadWrite | QIODevice::Text);
           QTextStream outStream(&file);
           QString line;
           while (!outStream.atEnd()){
               line = outStream.readLine();
               contFilas++;
           }
           ui->spinBoxINFOTXT->setValue(contFilas);
           file.close();
       }

       if(ui->checkBoxSVM->isChecked()){
           int contFilas=0;
           QFile file(DirFileTXTPosSVM);
           file.open(QIODevice::ReadWrite | QIODevice::Text);
           QTextStream outStream(&file);
           QString line;
           while (!outStream.atEnd()){
               line = outStream.readLine();
               contFilas++;
           }
           ui->spinBoxINFOTXT->setValue(contFilas);
           file.close();
       }

//       int contFilas=0;
//       QFile file(fileNameTXT);
//       file.open(QIODevice::ReadWrite | QIODevice::Text);
//       QTextStream outStream(&file);
//       QString line;
//       while (!outStream.atEnd()){
//           line = outStream.readLine();
//           contFilas++;
//       }
//       ui->spinBoxINFOTXT->setValue(contFilas);
//       file.close();
  }
}

void MainWindow::FileSaveNegativeDirImages(void)
{
    DirImagesNeg.clear();
    DirFileTXTNeg.clear();

    ui->lineEditTXTDirNegative->clear();
    ui->lineEditImageDirOutputNeg->clear();

    QString fileNameTXT,dirNameImages;
    fileNameTXT = QFileDialog::getSaveFileName(this, tr("Save TXT Negative"), QDir::homePath(),
                tr("files TXT (*.txt *.TXT)"));
    if(fileNameTXT.size()!=0){

       dirNameImages = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        QDir::homePath(),
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);

       DirImagesNeg=dirNameImages;
       DirFileTXTNeg=fileNameTXT;

       DirFileTXTNegHaar = DirFileTXTNeg;
       DirFileTXTNegSVM = DirFileTXTNeg;
       DirFileTXTNegHaar.remove(DirFileTXTNegHaar.size()-4,4);
       DirFileTXTNegHaar = DirFileTXTNegHaar + "_Haar.txt";
       DirFileTXTNegSVM.remove(DirFileTXTNegSVM.size()-4,4);
       DirFileTXTNegSVM = DirFileTXTNegSVM + "_SVM.txt";

       ui->lineEditTXTDirNegative->setText(fileNameTXT);
       ui->lineEditImageDirOutputNeg->setText(dirNameImages);
       if(ui->checkBoxHaar->isChecked()){
           int contFilas=0;
           QFile file(DirFileTXTNegHaar);
           file.open(QIODevice::ReadWrite | QIODevice::Text);
           QTextStream outStream(&file);
           QString line;
           while (!outStream.atEnd()){
               line = outStream.readLine();
               contFilas++;
           }
           ui->spinBoxINFOTXT->setValue(contFilas);
           file.close();
       }

       if(ui->checkBoxSVM->isChecked()){
           int contFilas=0;
           QFile file(DirFileTXTNegSVM);
           file.open(QIODevice::ReadWrite | QIODevice::Text);
           QTextStream outStream(&file);
           QString line;
           while (!outStream.atEnd()){
               line = outStream.readLine();
               contFilas++;
           }
           ui->spinBoxINFOTXT->setValue(contFilas);
           file.close();
       }
  }
}

void MainWindow::EnablePos(void)
{
    ui->pushButtonNegativeDir->setEnabled(false);
    ui->pushButtonPositiveDir->setEnabled(true);
    if(ui->checkBoxHaar->isChecked()){
        if(DirFileTXTPosHaar.size()!=0){
            int contFilas=0;
            QFile file(DirFileTXTPosHaar);
            file.open(QIODevice::ReadWrite | QIODevice::Text);
            QTextStream outStream(&file);
            QString line;
            while (!outStream.atEnd()){
                line = outStream.readLine();
                contFilas++;
            }
            ui->spinBoxINFOTXT->setValue(contFilas);
            file.close();
        }
    }

    if(ui->checkBoxSVM->isChecked()){
        if(DirFileTXTPosSVM.size()!=0){
            int contFilas=0;
            QFile file(DirFileTXTPosSVM);
            file.open(QIODevice::ReadWrite | QIODevice::Text);
            QTextStream outStream(&file);
            QString line;
            while (!outStream.atEnd()){
                line = outStream.readLine();
                contFilas++;
            }
            ui->spinBoxINFOTXT->setValue(contFilas);
            file.close();
        }
    }
//    if(DirFileTXT.size()!=0){
//        int contFilas=0;
//        QFile file(DirFileTXT);
//        file.open(QIODevice::ReadWrite | QIODevice::Text);
//        QTextStream outStream(&file);
//        QString line;
//        while (!outStream.atEnd()){
//            line = outStream.readLine();
//            contFilas++;
//        }
//        ui->spinBoxINFOTXT->setValue(contFilas);
//        file.close();
//    }
}

void MainWindow::EnableNeg(void)
{
    ui->pushButtonNegativeDir->setEnabled(true);
    ui->pushButtonPositiveDir->setEnabled(false);
    if(ui->checkBoxHaar->isChecked()){
        if(DirFileTXTNegHaar.size()!=0){
            int contFilas=0;
            QFile file(DirFileTXTNegHaar);
            file.open(QIODevice::ReadWrite | QIODevice::Text);
            QTextStream outStream(&file);
            QString line;
            while (!outStream.atEnd()){
                line = outStream.readLine();
                contFilas++;
            }
            ui->spinBoxINFOTXT->setValue(contFilas);
            file.close();
        }
    }

    if(ui->checkBoxSVM->isChecked()){
        if(DirFileTXTNegSVM.size()!=0){
            int contFilas=0;
            QFile file(DirFileTXTNegSVM);
            file.open(QIODevice::ReadWrite | QIODevice::Text);
            QTextStream outStream(&file);
            QString line;
            while (!outStream.atEnd()){
                line = outStream.readLine();
                contFilas++;
            }
            ui->spinBoxINFOTXT->setValue(contFilas);
            file.close();
        }
    }
}

void MainWindow::ClickOK(void)
{

    QString NamePos="HaarPos";
    QString NameNeg="HaarNeg";
    QString NameSVM="SVMPos";
    QString NameSVMNeg="SVMNeg";
    QString ImageLocalization;
    QString ImageLocalizationSVM;
    QString FramePos=QString::number(ui->spinBoxINFOTXT->value());
    QString PosXini=ui->labelPosXi->text();
    QString PosYini=ui->labelPosYi->text();
    QString AngleRotation = QString::number(ui->horizontalSliderAngle->value());
    QString ResizeHeight = ui->spinBoxResHeight->text();
    QString ResizeWidth = ui->spinBoxResWidth->text();
    QString HeightBox=ui->labelHeightBox->text();
    QString WidthBox=ui->labelWidthBox->text();

    if(ui->radioButtonPositive->isChecked()){
        QString ImageLocationFromUser = ui->lineEditImageDirOutput->text();
        QString ImageLocationFromUser2 = ui->lineEditImageDirOutput->text();
        if(DirImages.size()!=0){
            if(DirFileTXT.size()!=0){
                   NamePos="/"+NamePos+FramePos+".png";
                   NameSVM="/"+NameSVM+FramePos+".png";
                   ImageLocalization=DirImages+NamePos;
                   ImageLocalizationSVM=DirImages+NameSVM;                   
                   if(ImageLocationAnt!=ImageLocalization){
//                       QFile file(DirFileTXT);
//                       file.open(QIODevice::ReadWrite |QIODevice::Append | QIODevice::Text);
//                       QTextStream outStream(&file);
//                       outStream <<ImageLocationFromUser<<" "<<"1"<<" "<<PosXini<<" "<<PosYini<<" "<<WidthBox<<" "<<HeightBox<<"\n";
//                       file.close();
                       if(ui->checkBoxHaar->isChecked()){
                           ImageLocationFromUser = ImageLocationFromUser + NamePos;
                           FrameD.save(ImageLocalization,"png");
                           QFile file(DirFileTXTPosHaar);
                           file.open(QIODevice::ReadWrite |QIODevice::Append | QIODevice::Text);
                           QTextStream outStream(&file);
                           outStream <<ImageLocationFromUser<<" "<<"1"<<" "<<PosXini<<" "<<PosYini<<" "<<WidthBox<<" "<<HeightBox<<"\n";
                           file.close();
                       }
                       if(ui->checkBoxSVM->isChecked()){
                           ImageLocationFromUser2 = ImageLocationFromUser2 + NameSVM;
                           FrameDROI.save(ImageLocalizationSVM, "png");
                           QFile file(DirFileTXTPosSVM);
                           file.open(QIODevice::ReadWrite |QIODevice::Append | QIODevice::Text);
                           QTextStream outStream(&file);
                           outStream <<ImageLocationFromUser2<<" "<<"1"<<" "<<PosXini<<" "<<PosYini<<" "<<WidthBox<<" "<<HeightBox;
                           outStream <<AngleRotation<<" "<<ResizeWidth<<" "<<ResizeHeight<<"\n";
                           file.close();
                       }
                       ImageLocationAnt=ImageLocalization;
                   }
            }else{
                msgBox->setInformativeText("Postive file TXT location Bad");
                msgBox->show();
                //qDebug()<<"DEBE INDICAR UN ARCHIVO .txt PARA ALMACENAR LA INFO...";
            }
        }else{
            msgBox->setInformativeText("Postive Dir Image location Bad");
            msgBox->show();
            //qDebug()<<"DEBE INDICAR UN DIRECTORIO DONDE SE VA ALMACENAR LAS IMAGENES (POSITIVAS)...";
        }
    }

    if(ui->radioButtonNegative->isChecked()){
        QString ImageLocationFromUser = ui->lineEditImageDirOutputNeg->text();
        if(DirImagesNeg.size()!=0){
            if(DirFileTXTNeg.size()!=0){

                    NameNeg="/"+NameNeg+FramePos+".png";
                    NameSVMNeg="/"+NameSVMNeg+FramePos+".png";
                    ImageLocalization=DirImagesNeg+NameNeg;
                    ImageLocalizationSVM=DirImagesNeg+NameSVMNeg;
                    ImageLocationFromUser = ImageLocationFromUser + NameSVMNeg;
                    if(ImageLocationAnt!=ImageLocalization){
                        if(ui->checkBoxHaar->isChecked()){
                            FrameD.save(ImageLocalization,"png");
                            QFile file(DirFileTXTNegHaar);
                            file.open(QIODevice::ReadWrite |QIODevice::Append | QIODevice::Text);
                            QTextStream outStream(&file);
                            outStream <<ImageLocalization<<"\n";
                            file.close();
                        }

                        if(ui->checkBoxSVM->isChecked()){
                            FrameDROI.save(ImageLocalizationSVM, "png");
                            QFile file(DirFileTXTNegSVM);
                            file.open(QIODevice::ReadWrite |QIODevice::Append | QIODevice::Text);
                            QTextStream outStream(&file);
                            outStream <<ImageLocationFromUser<<" "<<"1"<<" "<<PosXini<<" "<<PosYini<<" "<<WidthBox<<" "<<HeightBox;
                            outStream <<AngleRotation<<" "<<ResizeWidth<<" "<<ResizeHeight<<"\n";
                            file.close();
                        }
                        ImageLocationAnt=ImageLocalization;
                    }
            }else{
                msgBox->setInformativeText("Negtive file TXT location Bad");
                msgBox->show();
                //qDebug()<<"DEBE INDICAR UN ARCHIVO .txt PARA ALMACENAR LA INFO...";
            }
        }else{
            msgBox->setInformativeText("Negative Dir Image location Bad");
            msgBox->show();
            //qDebug()<<"DEBE INDICAR UN DIRECTORIO DONDE SE VA ALMACENAR LAS IMAGENES (NEGATIVAS)...";
        }
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
