#ifndef LABELEVENTMOUSE_H
#define LABELEVENTMOUSE_H

#include <QtGui>
#include <QMouseEvent>
#include <QObject>
#include <qlabel.h>
#include <qwidget.h>
#include <qrect.h>
#include <qpoint.h>

class LabelEventMouse:public QLabel
{
    //Q_OBJECT
public:
    LabelEventMouse(QWidget *parent=0);
    ~LabelEventMouse();

protected:
    void mouseMoveEvent(QMouseEvent *evn);
    void mousePressEvent(QMouseEvent *evn);
    void mouseReleaseEvent(QMouseEvent *evn);
    void paintEvent(QPaintEvent *evn);
private:
    bool pintaCaja;
    bool pintaPunto;
    bool BotonLeft;
    bool BotonRight;
    QRect *Caja;
    QPoint puntoInicio;
    QPoint PosMouse;
    int XInit;
    int YInit;
    int XEnd;
    int YEnd;
    int HeightBox,WidthBox;
};

#endif // LABELEVENTMOUSE_H
