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

    VideoCodec = new QProcess(this);
    Proceso = new ProcessOnThread(this);
    FileConv = new QFile(this);
    FileConv->setFileName("myfile.mp4");
    if(FileConv->exists()){
        FileConv->remove();
    }

    this->conexiones();

    ImageLocationAnt="////";
    datoAnt=0;
    ExtratbyPoints = false;
    minBarHaar = 0; maxBarHaar=100;
    valueBarHaar = 0;
    ui->pushButtonOK2->setVisible(false);
    ui->pushButtonROIOK2->setVisible(false);
    ui->pushButtonClearRois->setVisible(false);
    ui->labelRegSave2->setVisible(false);
    ui->spinBoxINFOTXT2->setVisible(false);
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
    this->connect(ui->pushButtonOK2,SIGNAL(clicked()),this,SLOT(ClickOK()));

    this->connect(ui->pushButtonStopVideo,SIGNAL(clicked()),this,SLOT(StopVideo()));

    //this->connect(&VideoLoadOCV,SIGNAL(FrameCountVideo(int)), ui->labelSliderFrame, SLOT(setNum(int)));
    this->connect(ui->radioButtonBoxDynamic,SIGNAL(clicked(bool)), this,SLOT(ClickRadioButtonBox(bool)));
    this->connect(ui->radioButtonBoxStatic,SIGNAL(clicked(bool)), this,SLOT(ClickRadioButtonBox(bool)));
    this->connect(ui->radioButtonBoxPoints,SIGNAL(clicked(bool)), this,SLOT(ClickRadioButtonBox(bool)));
    this->connect(ui->spinBoxHeightBoxStatic,SIGNAL(valueChanged(int)), this,SLOT(ClickSpinBoxBoxStatic(int)));
    this->connect(ui->spinBoxWidthBoxStatic,SIGNAL(valueChanged(int)), this,SLOT(ClickSpinBoxBoxStatic(int)));

    this->connect(ui->horizontalSliderAngle,SIGNAL(valueChanged(int)),this,SLOT(ChangeValueScrollBarAngle(int)));
    this->connect(LabelImagen, SIGNAL(EmitRegionExtract()),this,SLOT(UpdateRegionExtract()));
    this->connect(&VideoLoadOCV, SIGNAL(ImageROIFromVideo(QImage)), this,SLOT(ReceiveImageROI(QImage)));

    this->connect(ui->checkBoxSVM,SIGNAL(stateChanged(int)),this,SLOT(ClickChecbox(int)));
    this->connect(ui->checkBoxResizeEXtract,SIGNAL(stateChanged(int)),this,SLOT(ClickChecbox(int)));
    this->connect(ui->spinBoxResHeight,SIGNAL(valueChanged(int)),this,SLOT(ClickSpinBoxResize(int)));
    this->connect(ui->spinBoxResWidth,SIGNAL(valueChanged(int)),this,SLOT(ClickSpinBoxResize(int)));

    this->connect(VideoCodec,SIGNAL(error(QProcess::ProcessError)),this,SLOT(ErrorProcess(QProcess::ProcessError)));
    this->connect(VideoCodec,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(ProcessEnd(int,QProcess::ExitStatus)));
    this->connect(&VideoLoadOCV,SIGNAL(EmitProccessConvVideo(bool)),this,SLOT(InitProcessConv(bool)));
    this->connect(this,SIGNAL(EmitFinProccess(bool)),&VideoLoadOCV,SLOT(FinishConv(bool)));

    this->connect(ui->pushButtonExtract1,SIGNAL(clicked()),this,SLOT(ClickExtract1()));
    this->connect(&VideoLoadOCV,SIGNAL(ImagePolyExtract(QImage)),this,SLOT(ReceiveImagePOly(QImage)));

    this->connect(ui->tabWidgetImage,SIGNAL(currentChanged(int)),this,SLOT(ChangePage(int)));
    this->connect(ui->pushButtonExecuteHaarSamples,SIGNAL(clicked()),this,SLOT(ClickExecuteHaarSamples()));
    this->connect(ui->pushButtonCreateSamples,SIGNAL(clicked()),this,SLOT(ClickBrowseCreateSamples()));
    this->connect(ui->pushButtonSamplesOut,SIGNAL(clicked()),this,SLOT(ClickBrowseCreateSamplesOut()));
    this->connect(ui->pushButtonVecOut,SIGNAL(clicked()),this,SLOT(ClickBrowseCreateSamplesVecOute()));

    this->connect(Proceso,SIGNAL(emitEstateFinish(QString)),this,SLOT(updateBarHaarSamples(QString)));
}

void MainWindow::ChangePage(int index)
{
    if(index == 0){
        if(ui->radioButtonPositive->isChecked()){
            ui->pushButtonNegativeDir->setEnabled(false);
            ui->pushButtonPositiveDir->setEnabled(true);
        }else{
            ui->pushButtonNegativeDir->setEnabled(true);
            ui->pushButtonPositiveDir->setEnabled(false);
        }
    }else if(index == 1){
        ui->pushButtonNegativeDir->setEnabled(true);
        ui->pushButtonPositiveDir->setEnabled(true);
    }
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
                ui->pushButtonExtract1->setEnabled(false);
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
                ui->pushButtonExtract1->setEnabled(false);
            }else if(RadioButtonSelect->objectName() == "radioButtonBoxPoints"){
                ui->radioButtonBoxDynamic->setChecked(false);
                ui->radioButtonBoxStatic->setChecked(false);
                ui->spinBoxHeightBoxStatic->setEnabled(false);
                ui->spinBoxWidthBoxStatic->setEnabled(false);
                ui->labelHeightBoxStatic->setEnabled(false);
                ui->labelWidthBoxStatic->setEnabled(false);
                ui->pushButtonExtract1->setEnabled(true);
                LabelImagen->setTypeBox((unsigned char)2);
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

void MainWindow::ReceiveErrorProcess(unsigned char code)
{
    msgBox->setWindowTitle("PROCESS ERROR");
    msgBox->setText("Error ejecution FFMPEG conv.                                        ");
    msgBox->setIcon(QMessageBox::Critical);
    if(code == 0){
        msgBox->setInformativeText("Start FFMPEG failed....");
        msgBox->show();
    }else if(code == 1){
        msgBox->setInformativeText("Crasehd FFMPEG....");
        msgBox->show();
    }else if(code == 2){
        msgBox->setInformativeText("TimeOut FFMPEG....");
        msgBox->show();
    }else if(code == 3){
        msgBox->setInformativeText("Write FFMPEG error....");
        msgBox->show();
    }else if(code == 4){
        msgBox->setInformativeText("Read FFMPEG error....");
        msgBox->show();
    }else if(code == 5){
        msgBox->setInformativeText("Unknow error FFMPEG....");
        msgBox->show();
    }else if(code == 6){
        msgBox->setInformativeText("Deafult FFMPEG....");
        msgBox->show();
    }
}

void MainWindow::ErrorProcess(QProcess::ProcessError error)
{
    switch(error)
    {
    case QProcess::FailedToStart:
        ReceiveErrorProcess(0);
        break;
    case QProcess::Crashed:
        ReceiveErrorProcess(1);
        break;
    case QProcess::Timedout:
        ReceiveErrorProcess(2);
        break;
    case QProcess::WriteError:
        ReceiveErrorProcess(3);
        break;
    case QProcess::ReadError:
        ReceiveErrorProcess(4);
        break;
    case QProcess::UnknownError:
        ReceiveErrorProcess(5);
        break;
    default:
        ReceiveErrorProcess(6);
        break;
    }
}

void MainWindow::ProcessEnd(int exitCode, QProcess::ExitStatus exitStatus)
{
    switch (exitStatus){
    case QProcess::NormalExit:
        qDebug()<<"Salida normal... "<<exitCode;
        emit EmitFinProccess(true);
        break;
    case QProcess::CrashExit:
        qDebug()<<"Salida crasehd... "<<exitCode;
        emit EmitFinProccess(true);
        break;
    default:
        break;
    }
    msgBox->hide();
}

void MainWindow::InitProcessConv(bool state)
{
    if(state){
#ifdef Q_OS_LINUX
        QString ProgramPath = "ffmpeg";
#else
        QString ProgramPath = "ffmpeg-20140403/bin/ffmpeg.exe";
#endif
        QStringList argumentos;
        argumentos<<"-loglevel"<<"quiet"<<"-i"<<FileNameLocation<<"-vcodec"<<"libx264"<<"-profile:v"<<"baseline"<<"-preset"<<"ultrafast"<<"myfile.mp4";
        VideoCodec->start(ProgramPath,argumentos);
        if(!VideoCodec->waitForStarted()){
             ReceiveErrorProcess(0);
        }else{
            msgBox->setWindowTitle("Processing Video File");
            msgBox->setText("Processing Video File.                                        ");
            msgBox->setInformativeText("Please wait a moment, if you want stop push Stop Video");
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->show();
        }
    }else{
    }
}

void MainWindow::ClickExecuteHaarSamples(void)
{
    QString ProgramPath = ui->lineEditCreateSamples->text();
    minBarHaar=0; maxBarHaar=100;valueBarHaar=0;
    ui->progressBarHaarSamples->setValue(valueBarHaar);
    this->connect(VideoCodec,SIGNAL(readyReadStandardOutput()),this,SLOT(onStdOutput()));
    if(!ProgramPath.isEmpty()){
        ProgramPath = ProgramPath+"/opencv_createsamples";
        QFile File(ProgramPath);
        if(File.exists()){
            QString SampleDirOutput = ui->lineEditSamplesOut->text();
            if(!SampleDirOutput.isEmpty()){
                QString VecDirect=ui->lineEditVecOut->text();
                if(!VecDirect.isEmpty()){
                    QString DirPosL = ui->lineEditImageDirOutput->text();
                    QString DirNegL= ui->lineEditImageDirOutputNeg->text();
                    QDir DirPos(DirPosL);
                    QDir DirNeg(DirNegL);
                    if(DirPos.exists() && DirNeg.exists()){
                        DirPos.setNameFilters(QStringList()<<"*.bmp"<<"*.png"<<"*.jpg"<<"*.BMP"<<"*.PNG"<<"*.JPG");
                        DirNeg.setNameFilters(QStringList()<<"*.bmp"<<"*.png"<<"*.jpg"<<"*.BMP"<<"*.PNG"<<"*.JPG");
                        QStringList ListFileNeg = DirNeg.entryList();
                        QStringList ListFilePos = DirPos.entryList();
                        QFile file(DirFileTXTNegHaar);
                        file.open(QIODevice::ReadWrite | QIODevice::Text);
                        QTextStream outStream(&file);
                        foreach (QString FileLN, ListFileNeg) {
                            //FileLN = "."+DirNegL + "/" +FileLN;
                            FileLN = "./" +FileLN;
                            outStream<<FileLN<<"\n";
                        }
                        file.close();
                        QList<QStringList> Parametros;
                        foreach (QString FileL, ListFilePos) {
                            QStringList argumentos;
                            QString FileSampleOutN;
                            if(FileL.contains(".png") || FileL.contains(".PNG")){
                                QString FileTempo = FileL;
                                FileSampleOutN = SampleDirOutput+"/"+FileTempo.remove(".png") + ".txt";
                                FileL = DirPosL+"/"+FileL;
                            }
                            argumentos<<"-img"<<FileL<<"-bg"<<DirFileTXTNegHaar<<"-info"<<FileSampleOutN;
                            argumentos<<"-num"<<ui->spinBoxSamplesOut->text()<<"-maxxangle"<<ui->doubleSpinBoxMaxAX->text();
                            argumentos<<"-maxyangle"<<ui->doubleSpinBoxMaxAY->text();
                            argumentos<<"-maxzangle"<<ui->doubleSpinBoxMaxAZ->text();
                            argumentos<<"-bgcolor"<<ui->spinBoxBackGColor->text();
                            argumentos<<"-bgthresh"<<ui->spinBoxBackGThresh->text();
                            argumentos<<"-w"<<ui->spinBoxWidthSample->text();argumentos<<"-h"<<ui->spinBoxHeightSample->text();
                            argumentos<<"-maxidev"<<ui->spinBoxMaxDevInten->text();
                            //qDebug()<<argumentos;
//                            VideoCodec->start(ProgramPath,argumentos);
//                            if(!VideoCodec->waitForStarted()){
//                                 ReceiveErrorProcess(0);
//                            }
//                            VideoCodec->waitForFinished();
                            Parametros.push_back(argumentos);
                        }
                        maxBarHaar = Parametros.size();
                        ui->progressBarHaarSamples->setMaximum(maxBarHaar);
                        ui->progressBarHaarSamples->setMinimum(minBarHaar);
                        Proceso->passProgramAndParams(ProgramPath, Parametros);
                        Proceso->Start();
                    }else{
                        msgBox->setWindowTitle("Paths opencv_createsamples");
                        msgBox->setText("Invalid Path                                  ");
                        msgBox->setInformativeText("Please indicate location directory of Positive Images");
                        msgBox->setIcon(QMessageBox::Warning);
                        msgBox->show();
                    }
                }else{
                    msgBox->setWindowTitle("Paths opencv_createsamples");
                    msgBox->setText("Invalid Path                                  ");
                    msgBox->setInformativeText("Please indicate location dircetory of .vec out");
                    msgBox->setIcon(QMessageBox::Warning);
                    msgBox->show();
                }
            }else{
                msgBox->setWindowTitle("Paths opencv_createsamples");
                msgBox->setText("Invalid Path                                  ");
                msgBox->setInformativeText("Please indicate location dircetory of samples out");
                msgBox->setIcon(QMessageBox::Warning);
                msgBox->show();
            }
        }else{
            msgBox->setWindowTitle("Paths opencv_createsamples");
            msgBox->setText("Invalid Path, not found opencv_createsamples on directory              ");
            msgBox->setInformativeText("Please indicate location dircetory of opencv_createsamples");
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->show();
        }
    }else{
        msgBox->setWindowTitle("Paths opencv_createsamples");
        msgBox->setText("Invalid Path.                                        ");
        msgBox->setInformativeText("Please indicate location dircetory of opencv_createsamples");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->show();
    }
    this->disconnect(VideoCodec,SIGNAL(readyReadStandardOutput()),this,SLOT(onStdOutput()));
}

void MainWindow::updateBarHaarSamples(QString State)
{
    if(State == "Done"){
        valueBarHaar++;
        ui->progressBarHaarSamples->setValue(valueBarHaar);
    }else{

    }
}

void MainWindow::onStdOutput(void)
{
    qDebug()<<VideoCodec->readAllStandardOutput();
}

void MainWindow::ClickBrowseCreateSamples(void)
{
    QString FileName;
    FileName = QFileDialog::getExistingDirectory(this,tr("Path of opencv_createsamples"), QDir::homePath());
    if(!FileName.isEmpty()){
        ui->lineEditCreateSamples->setText(FileName);
    }
}

void MainWindow::ClickBrowseCreateSamplesOut(void)
{
    QString FileName;
    FileName = QFileDialog::getExistingDirectory(this,tr("Path of samples out"), QDir::homePath());
    if(!FileName.isEmpty()){
        ui->lineEditSamplesOut->setText(FileName);
    }
}

void MainWindow::ClickBrowseCreateSamplesVecOute(void)
{
    QString FileName;
    FileName = QFileDialog::getSaveFileName(this,tr("Path of vec out"), QDir::homePath(),tr("files TXT (*.vec)"));
    if(!FileName.isEmpty()){
        ui->lineEditVecOut->setText(FileName);
    }
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
                tr("files AVI (*.avi *AVI);;files MP4 (*.mp4 *.MP4);;files FLV (*.flv *.FLV);;File 3gp(*.3gp *.3GP)"));
    FileNameLocation = fileName;
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

void MainWindow::ReceiveImagePOly(QImage ImaVideoPoly)
{
    ImagePoly = ImaVideoPoly;
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
            ui->checkBoxImageEXtractOnly->setEnabled(true);
        }else{
            ui->checkBoxResizeEXtract->setEnabled(false);
            ui->labelResHeight->setEnabled(false);
            ui->labelResWidth->setEnabled(false);
            ui->spinBoxResHeight->setEnabled(false);
            ui->spinBoxResWidth->setEnabled(false);
            ui->checkBoxImageEXtractOnly->setEnabled(false);
        }
    }

}

void MainWindow::ResizeVerticalSlider(int CountFrame)
{
    MaxFramesVideo=CountFrame-2;
    ui->labelSliderFrame->setNum(CountFrame-2);
    ui->verticalSliderVIDEO->setMaximum((CountFrame-2));
    ui->spinBoxINFOTXT->setMaximum(MaxFramesVideo-2);
    ui->spinBoxINFOTXT2->setMaximum(MaxFramesVideo-2);
    //int valor=ui->spinBoxINFOTXT->value();
    //ui->verticalSliderVIDEO->setSliderPosition(valor);
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
    if(!ui->checkBoxRetainLast->isChecked()){
        ui->labelHeightBox->setText("0");
        ui->labelWidthBox->setText("0");
        ui->labelPosXi->setText("0");
        ui->labelPosYi->setText("0");
    }
    ExtratbyPoints = false;
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

    if(list.at(1)=="RGBA"){
        QStringList color = list.at(0).split(";");
        ui->label_R->setText(color.at(0));
        ui->label_G->setText(color.at(1));
        ui->label_B->setText(color.at(2));
        qDebug()<<color;
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

    dirNameImages = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     QDir::homePath(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);

    if(dirNameImages.size()!=0){
//        fileNameTXT = QFileDialog::getSaveFileName(this, tr("Save TXT Positive"), dirNameImages,
//                    tr("files TXT (*.txt *.TXT)"));

        fileNameTXT = dirNameImages+"/POS";


       DirImages=dirNameImages;
       DirFileTXT=fileNameTXT;
       DirFileTXTPosHaar = DirFileTXT;
       DirFileTXTPosSVM = DirFileTXT;
#ifdef Q_OS_WIN32
       DirFileTXTPosHaar.remove(DirFileTXTPosHaar.size()-4,4);
#endif
       DirFileTXTPosHaar = DirFileTXTPosHaar + "_Haar.txt";
#ifdef Q_OS_WIN32
       DirFileTXTPosSVM.remove(DirFileTXTPosSVM.size()-4,4);
#endif
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
    dirNameImages = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     QDir::homePath(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);

    if(dirNameImages.size()!=0){
//        fileNameTXT = QFileDialog::getSaveFileName(this, tr("Save TXT Negative"), dirNameImages,
//                    tr("files TXT (*.txt *.TXT)"));
       fileNameTXT = dirNameImages+"/NEG";

       DirImagesNeg=dirNameImages;
       DirFileTXTNeg=fileNameTXT;

       DirFileTXTNegHaar = DirFileTXTNeg;
       DirFileTXTNegSVM = DirFileTXTNeg;
#ifdef Q_OS_WIN32
       DirFileTXTNegHaar.remove(DirFileTXTNegHaar.size()-4,4);
#endif
       DirFileTXTNegHaar = DirFileTXTNegHaar + "_Haar.txt";
#ifdef Q_OS_WIN32
       DirFileTXTNegSVM.remove(DirFileTXTNegSVM.size()-4,4);
#endif
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

void MainWindow::ClickExtract1(void)
{
    std::vector<QPoint> PoinstS;
    LabelImagen->getPointsSelects(PoinstS);
    VideoLoadOCV.ExtractRegionPloygonal(PoinstS);
    ExtratbyPoints = true;
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

    QList<QRect> boxes = LabelImagen->getBoxes();
    if(boxes.isEmpty() && ui->radioButtonBoxPoints->isChecked()==false){
        msgBox->setWindowTitle("Box Selection...");
        msgBox->setText("Image Extraction.                                        ");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setInformativeText("Select a region to extract");
        msgBox->show();
        return;
    }

    if(PosXini=="0" && PosYini=="0" && HeightBox=="0" && WidthBox=="0" && ui->radioButtonBoxPoints->isChecked()==false){
        msgBox->setWindowTitle("Box Selection...");
        msgBox->setText("Image Extraction.                                        ");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setInformativeText("Select a region to extract");
        msgBox->show();
        return;
    }

    if(ExtratbyPoints == false && ui->radioButtonBoxPoints->isChecked()){
        msgBox->setWindowTitle("Box Selection...");
        msgBox->setText("Image Extraction.                                        ");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setInformativeText("Select a region to extract, Press Extract...");
        msgBox->show();
        return;
    }

    if(ui->radioButtonPositive->isChecked() || ui->radioButtonBoth->isChecked()){
        QString ImageLocationFromUser = ui->lineEditImageDirOutput->text();
        QString ImageLocationFromUser2 = ui->lineEditImageDirOutput->text();
        if(ui->radioButtonBoth->isChecked() && DirImagesNeg.size()==0){
            msgBox->setWindowTitle("Configuration Paths");
            msgBox->setText("File Negative location.                                        ");
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setInformativeText("Negative Dir Image location Bad");
            msgBox->show();
            return;
        }
        if(DirImages.size()!=0){
            if(DirFileTXT.size()!=0){
#ifdef Q_OS_WIN32
                   NamePos="\\"+NamePos+FramePos+".png";
                   NameSVM="\\"+NameSVM+FramePos+".png";
#else
                   NamePos="/"+NamePos+FramePos+".png";
                   NameSVM="/"+NameSVM+FramePos+".png";
#endif
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
                           if(!ui->radioButtonBoxPoints->isChecked()){
                                FrameD.save(ImageLocalization,"png");
                                QFile file(DirFileTXTPosHaar);
                                file.open(QIODevice::ReadWrite |QIODevice::Append | QIODevice::Text);
                                QTextStream outStream(&file);
                                outStream <<ImageLocationFromUser<<" "<<QString::number(boxes.size());
                                foreach (QRect val, boxes) {
                                    outStream <<" "<<val.x()<<" "<<val.y()<<" "<<val.width()<<" "<<val.height();
                                }
                                outStream <<"\n";
                                //outStream <<ImageLocationFromUser<<" "<<"1"<<" "<<PosXini<<" "<<PosYini<<" "<<WidthBox<<" "<<HeightBox<<"\n";
                                file.close();
                           }else{
                               if(ui->checkBoxImageEXtractOnly->isChecked()){
                                   FrameDROI.save(ImageLocalization, "png");
                               }else{
                                   ImagePoly.save(ImageLocalization,"png");
                               }
                           }
                           if(ui->radioButtonBoth->isChecked()){
                               NameNeg="/"+NameNeg+FramePos+".png";
                               ImageLocalization=DirImagesNeg+NameNeg;
                               regionBlacks(boxes, ImageLocalization);
                           }
                       }
                       if(ui->checkBoxSVM->isChecked()){
                           ImageLocationFromUser2 = ImageLocationFromUser2 + NameSVM;
                           QFile file(DirFileTXTPosSVM);
                           file.open(QIODevice::ReadWrite |QIODevice::Append | QIODevice::Text);
                           QTextStream outStream(&file);
                           if(ui->checkBoxImageEXtractOnly->isChecked()){
                               int cont = 1;
                               foreach (QImage img, LRois) {
                                   QString Name="/SVMPos"+FramePos+"_"+QString::number(cont)+".png";
                                   ImageLocalizationSVM=DirImages+Name;
                                   img.save(ImageLocalizationSVM, "png");
                                   outStream <<ImageLocalizationSVM<<"\n";
                                   cont++;
                               }
                           }else{
                               FrameD.save(ImageLocalizationSVM,"png");
                               outStream <<ImageLocationFromUser2<<" "<<QString::number(boxes.size());
                               foreach (QRect val, boxes) {
                                   outStream <<" "<<val.x()<<" "<<val.y()<<" "<<val.width()<<" "<<val.height();
                               }
                               outStream <<"\n";
                               //outStream <<ImageLocationFromUser2<<" "<<"1"<<" "<<PosXini<<" "<<PosYini<<" "<<WidthBox<<" "<<HeightBox<<" ";
                               //outStream <<AngleRotation<<" "<<ResizeWidth<<" "<<ResizeHeight<<"\n";
                               file.close();
                           }
                       }
                       ImageLocationAnt=ImageLocalization;
                   }else{
                       msgBox->setWindowTitle("Configuration Paths");
                       msgBox->setText("Image process.                                        ");
                       msgBox->setIcon(QMessageBox::Warning);
                       msgBox->setInformativeText("Image already procees....");
                       msgBox->show();
                       return;
                   }
            }else{
                msgBox->setWindowTitle("Configuration Paths");
                msgBox->setText("File positive location.                                        ");
                msgBox->setIcon(QMessageBox::Warning);
                msgBox->setInformativeText("Postive file TXT location Bad");
                msgBox->show();
                return;
                //qDebug()<<"DEBE INDICAR UN ARCHIVO .txt PARA ALMACENAR LA INFO...";
            }
        }else{
            msgBox->setWindowTitle("Configuration Paths");
            msgBox->setText("File Positive location.                                        ");
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setInformativeText("Postive Dir Image location Bad");
            msgBox->show();
            return;
            //qDebug()<<"DEBE INDICAR UN DIRECTORIO DONDE SE VA ALMACENAR LAS IMAGENES (POSITIVAS)...";
        }
    }

    if(ui->radioButtonNegative->isChecked()){
        QString ImageLocationFromUser = ui->lineEditImageDirOutputNeg->text();
        if(DirImagesNeg.size()!=0){
            if(DirFileTXTNeg.size()!=0){
#ifdef Q_OS_WIN32
                    NameNeg="\\"+NameNeg+FramePos+".png";
                    NameSVMNeg="\\"+NameSVMNeg+FramePos+".png";
#else
                NameNeg="/"+NameNeg+FramePos+".png";
                NameSVMNeg="/"+NameSVMNeg+FramePos+".png";
#endif

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
                            QFile file(DirFileTXTNegSVM);
                            file.open(QIODevice::ReadWrite |QIODevice::Append | QIODevice::Text);
                            QTextStream outStream(&file);
                            if(ui->checkBoxImageEXtractOnly->isChecked()){
                                //FrameDROI.save(ImageLocalizationSVM, "png");
                                //outStream <<ImageLocationFromUser<<"\n";
                                int cont = 1;
                                foreach (QImage img, LRois) {
                                    QString Name="/SVMNeg"+FramePos+"_"+QString::number(cont)+".png";
                                    ImageLocalizationSVM=DirImagesNeg+Name;
                                    img.save(ImageLocalizationSVM, "png");
                                    outStream <<ImageLocalizationSVM<<"\n";
                                    cont++;
                                }
                            }else{
                                FrameD.save(ImageLocalizationSVM,"png");
                                outStream <<ImageLocationFromUser<<" "<<"1"<<" "<<PosXini<<" "<<PosYini<<" "<<WidthBox<<" "<<HeightBox<<" ";
                                outStream <<AngleRotation<<" "<<ResizeWidth<<" "<<ResizeHeight<<"\n";
                            }
                            file.close();
                        }
                        ImageLocationAnt=ImageLocalization;
                    }
            }else{
                msgBox->setWindowTitle("Configuration Paths");
                msgBox->setText("File Negative location.                                        ");
                msgBox->setIcon(QMessageBox::Warning);
                msgBox->setInformativeText("Negtive file TXT location Bad");
                msgBox->show();
                return;
                //qDebug()<<"DEBE INDICAR UN ARCHIVO .txt PARA ALMACENAR LA INFO...";
            }
        }else{
            msgBox->setWindowTitle("Configuration Paths");
            msgBox->setText("File Negative location.                                        ");
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setInformativeText("Negative Dir Image location Bad");
            msgBox->show();
            return;
            //qDebug()<<"DEBE INDICAR UN DIRECTORIO DONDE SE VA ALMACENAR LAS IMAGENES (NEGATIVAS)...";
        }
    }
    ui->pushButtonNEXT->click();
    LRois.clear();
    LabelImagen->cleaBoxes();
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


    datoAnt=dato;

    //qDebug()<<ValorSpin;
}

void MainWindow::on_checkBoxHide_stateChanged(int arg1)
{
    if(arg1==0){
        ui->frame->setVisible(true);
        ui->frame_2->setVisible(true);
        ui->pushButtonOK2->setVisible(false);
        ui->pushButtonROIOK2->setVisible(false);
        ui->pushButtonClearRois->setVisible(false);
        ui->labelRegSave2->setVisible(false);
        ui->spinBoxINFOTXT2->setVisible(false);
    }else{
        ui->frame->setVisible(false);
        ui->frame_2->setVisible(false);
        ui->pushButtonOK2->setVisible(true);
        ui->pushButtonROIOK2->setVisible(true);
        ui->pushButtonClearRois->setVisible(true);
        ui->labelRegSave2->setVisible(true);
        ui->spinBoxINFOTXT2->setVisible(true);
    }
}

void MainWindow::on_pushButtonLoadDirectoryFil_clicked()
{
    QString fileName;

    if(VideoLoadOCV.isRunning()){
        //qDebug()<<"HILO YA ESTA CORRIENDO...";
        VideoLoadOCV.StopThread(true);
    }

    fileName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 QDir::homePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    FileNameLocation = fileName;
    if(fileName.size()>0){
       //qDebug()<<"FILE ANTES::  "<<fileName;
        VideoLoadOCV.SetParhImageDirectoty(fileName);
        VideoLoadOCV.start();

    }else{
        qDebug()<<"FILENAME VACIO";
    }
}

void MainWindow::on_pushButtonROIOK_clicked()
{
    if(ui->labelHeightBox->text().toInt()>0 && ui->labelWidthBox->text().toInt()>0){
        QRect ROI(ui->labelPosXi->text().toInt(),ui->labelPosYi->text().toInt(), ui->labelWidthBox->text().toInt(),ui->labelHeightBox->text().toInt());
        LabelImagen->addBox(ROI);
        LRois.push_back(FrameDROI);
    }
}

void MainWindow::on_pushButton_clicked()
{
    LabelImagen->cleaBoxes();
    LRois.clear();
}

void MainWindow::on_pushButtonROIOK2_clicked()
{
    ui->pushButtonROIOK->click();
}

void MainWindow::on_pushButtonClearRois_clicked()
{
    ui->pushButton->click();
}

void MainWindow::regionBlacks(QList<QRect> boxes, QString PathSave)
{
    QRgb value;
    value = qRgb(0, 0, 0);
    foreach (QRect box, boxes) {
        int xi=box.x();
        int yi=box.y();
        int width = box.width() + xi;
        int height = box.height() + yi;
        for(int i=xi;i<width;i++){
            for(int j=yi;j<height;j++){
                FrameD.setPixel(i,j,value);
            }
        }
    }
    FrameD.save(PathSave, "png");
    QFile file(DirFileTXTNegHaar);
    file.open(QIODevice::ReadWrite |QIODevice::Append | QIODevice::Text);
    QTextStream outStream(&file);
    outStream <<PathSave<<"\n";
    file.close();
}

void MainWindow::on_radioButtonBoth_clicked()
{
    //ui->radioButtonBoth->disconnect(ui->radioButtonBoth, SIGNAL(clicked()),this,SLOT(on_radioButtonBoth_clicked()));
    ui->pushButtonNegativeDir->setEnabled(true);
    ui->pushButtonPositiveDir->setEnabled(true);
    //ui->radioButtonBoth->connect(ui->radioButtonBoth, SIGNAL(clicked()),this,SLOT(on_radioButtonBoth_clicked()));
}

void MainWindow::on_checkBoxHaar_clicked()
{
    if(!ui->checkBoxHaar->isChecked()){
        ui->checkBoxHaar->setChecked(true);
    }
    if(ui->checkBoxSVM->isChecked()){
        ui->checkBoxSVM->setChecked(false);
    }
    if(ui->checkBoxImageEXtractOnly->isChecked()){
        ui->checkBoxImageEXtractOnly->setChecked(false);
    }
}

void MainWindow::on_checkBoxSVM_clicked()
{
    if(!ui->checkBoxSVM->isChecked()){
        ui->checkBoxSVM->setChecked(true);
    }
    if(ui->checkBoxHaar->isChecked()){
        ui->checkBoxHaar->setChecked(false);
    }
    if(!ui->checkBoxImageEXtractOnly->isChecked()){
        ui->checkBoxImageEXtractOnly->setChecked(true);
    }
}
