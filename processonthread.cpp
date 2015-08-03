#include "processonthread.h"
#include <QDebug>

ProcessOnThread::ProcessOnThread(QObject *parent) : QThread(parent)
{
    Proc = new QProcess();
    Ready = false;
    conexiones();
}

ProcessOnThread::~ProcessOnThread()
{

}

void ProcessOnThread::conexiones(void)
{
    connect(Proc,SIGNAL(readyReadStandardOutput()),this,SLOT(onSTDOUT()));
}

void ProcessOnThread::Start(void)
{
    if(this->isRunning()){
        this->quit();
    }
    this->start();
}

void ProcessOnThread::Stop(void)
{
    if(this->isRunning()){
        this->quit();
    }
}

void ProcessOnThread::run()
{
    qDebug()<<"ProcOnThread: Init....";
    if(Ready){
        foreach (QStringList Params, Parametros) {
            Proc->start(PathProgram, Params);
            if(Proc->waitForStarted()){
                Proc->waitForFinished();
            }else{
                emit emitError(PathProgram+" Fail Execution");
            }
        }
    }else{
        emit emitError("Pass Program Path and Params Execution");
    }
    Ready = false;
    qDebug()<<"ProcOnThread: Finish....";
}

void ProcessOnThread::passProgramAndParams(QString ProgramPath, QList<QStringList> Params)
{
    PathProgram = ProgramPath;
    Parametros = Params;
    Ready = true;
}

void ProcessOnThread::onSTDOUT(void)
{
    QString OutPuTConsole(Proc->readAllStandardOutput());
    if(OutPuTConsole.contains("Done")){
        emit emitEstateFinish("Done");
    }else{
        emit emitEstateFinish("Fail");
    }
}
