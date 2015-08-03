#ifndef PROCESSONTHREAD_H
#define PROCESSONTHREAD_H

#include <QThread>
#include <QProcess>

class ProcessOnThread : public QThread
{
    Q_OBJECT
public:
    explicit ProcessOnThread(QObject *parent = 0);
    ~ProcessOnThread();
    virtual void run();
    void passProgramAndParams(QString ProgramPath, QList<QStringList> Params);
    void Start(void);
    void Stop(void);
signals:
    void emitError(QString Error);
    void emitEstateFinish(QString Mess);
public slots:
    void onSTDOUT(void);
private:
    QProcess *Proc;
    QString PathProgram;
    QList<QStringList> Parametros;
    bool Ready;

    void conexiones(void);
};

#endif // PROCESSONTHREAD_H
