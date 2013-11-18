#include "LabelEventMouse.h"

#include<iostream>
using namespace std;

LabelEventMouse::LabelEventMouse(QWidget *parent):QLabel(parent)
{
    this->setAlignment(Qt::AlignCenter);
    //this->setText("Ningun Valor");
    this->setMouseTracking(true);
    pintaCaja=true;
    pintaPunto=false;
    BotonLeft=false;
    BotonRight=false;

    XInit=0;
    YInit=0;
    XEnd=0;
    YEnd=0;
    HeightBox=0;
    WidthBox=0;

}

LabelEventMouse::~LabelEventMouse()
{}

void LabelEventMouse::mouseMoveEvent(QMouseEvent *evn)
{
    //QString X,Y;
    //int xp,yp;
    XEnd = evn->x();
    YEnd = evn->y();
    //X=QString::number(evn->x());///OBTENCION DEL VALOR DE X DEL MOUSE SOBRE LE LABEL
    //Y=QString::number(evn->y());///OBTENCION DEL VALOR DE Y DEL MOUSE SOBRE LE LABEL
    //this->setText("X:  "+X+"---Y:  "+Y);////PONE TEXTO EL EL LABEL
    if(BotonLeft==true){ ///PARA MANTENER DIBUJADO EL RECUADRO
        this->update();
        if(pintaCaja==true && pintaPunto==false && BotonLeft==true)
        {
            PosMouse=evn->pos();
            HeightBox=(PosMouse.y()-puntoInicio.y());
            WidthBox=(PosMouse.x()-puntoInicio.x());
            Caja->setWidth((PosMouse.x()-puntoInicio.x()));
            Caja->setHeight((PosMouse.y()-puntoInicio.y()));

            QString Height1=QString::number(HeightBox)+":HeightBox";
            QString Width1=QString::number(WidthBox)+":WidthBox";

            emit linkActivated(Height1);
            emit linkActivated(Width1);

        }
    }

}///EVENTO DE MOVIMIENTO DEL MOUSE

void LabelEventMouse::mousePressEvent(QMouseEvent *evn)
{
    //this->update();
    PosMouse=evn->pos();///OBTNEGO LA POSICION DEL MOUSE
    if(evn->button()==Qt::LeftButton)
    {
        if(pintaCaja==true && pintaPunto==false)
        {
            puntoInicio=evn->pos();
            XInit=puntoInicio.x();
            YInit=puntoInicio.y();
            QString PosXIn=QString::number(XInit)+":Xinit";
            QString PosYIn=QString::number(YInit)+":Yinit";

            emit linkActivated(PosXIn);
            emit linkActivated(PosYIn);

            Caja=new QRect(puntoInicio.x(),puntoInicio.y(),0,0);
            //pintaCaja=true;
            BotonLeft=true;
        }
        if(pintaCaja==false && pintaPunto==true)
        {
            BotonLeft=true;
        }
    }
}///EVENTO DE PRESION DE BOTON DEL MOUSE

void LabelEventMouse::mouseReleaseEvent(QMouseEvent *evn)
{
    if(pintaCaja==true)
    {
        //cout<<"INICIO X: "<<puntoInicio.x()<<" INICIO Y: "<<puntoInicio.y()<<endl;
        //cout<<"HEIGHT:  "<<Caja->height()<<"  ALTO:  "<<Caja->width()<<endl;
        BotonLeft=false;
    }
    //pintaCaja=false;
    //delete(Caja);
}///EVENTO DE LIBERACION DEL MOUSE

void LabelEventMouse::paintEvent(QPaintEvent *evn)
{
    QLabel::paintEvent(evn);
    QPainter pintor(this);
    QString PosMouseM = QString::number(XEnd) + "," + QString::number(YEnd);

    if(pintaCaja==true && pintaPunto==false && BotonLeft==true)
    {
        pintor.setPen(Qt::blue);
        pintor.drawRect(*Caja);
        pintor.drawText(XEnd,YEnd,PosMouseM);

    }
    if(pintaCaja==false && pintaPunto==true && BotonLeft==true)
    {
        pintor.setPen(Qt::black);
        pintor.drawEllipse(PosMouse,10,10);
    }
}///EVENTO DE PINTAR
