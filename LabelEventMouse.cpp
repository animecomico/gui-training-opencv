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
    XEnd=100;
    YEnd=100;
    HeightBox=100;
    WidthBox=100;
    Caja=new QRect(0,0,0,0);
    Caja2 = new QRect(0,0,0,0);
    TypeB = 0; //0 Dynamica, 1 Estatica
}

LabelEventMouse::~LabelEventMouse()
{}

void LabelEventMouse::setTypeBox(unsigned char Type)
{
    TypeB = Type;
}

void LabelEventMouse::UpdateHeightandWidthBox(int H, int W)
{
    HeightBox = H;
    WidthBox = W;
}

void LabelEventMouse::mouseMoveEvent(QMouseEvent *evn)
{
    if(TypeB == 0){
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
    }else{
        this->update();
        if(pintaCaja == true){
            puntoInicio=evn->pos();
            puntoFinal.setX(puntoInicio.x()+WidthBox);
            puntoFinal.setY(puntoInicio.y()+HeightBox);
            XInit=puntoInicio.x();
            YInit=puntoInicio.y();


            QString Height1=QString::number(HeightBox)+":HeightBox";
            QString Width1=QString::number(WidthBox)+":WidthBox";

            emit linkActivated(Height1);
            emit linkActivated(Width1);

            XEnd = puntoFinal.x();
            YEnd = puntoFinal.y();

            Caja->setX(puntoInicio.x());
            Caja->setY(puntoInicio.y() - HeightBox);
            Caja->setWidth(puntoFinal.x()-puntoInicio.x());
            Caja->setHeight(puntoFinal.y()-puntoInicio.y());
        }
    }


}///EVENTO DE MOVIMIENTO DEL MOUSE

void LabelEventMouse::mousePressEvent(QMouseEvent *evn)
{
    //this->update();
    PosMouse=evn->pos();///OBTNEGO LA POSICION DEL MOUSE
    if(TypeB == 0){
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
    }else{
        puntoInicio=evn->pos();
        XInit=puntoInicio.x();
        YInit=puntoInicio.y() - HeightBox;
        puntoFinal.setX(puntoInicio.x()+WidthBox);
        puntoFinal.setY(puntoInicio.y()+HeightBox);

        Caja2->setX(XInit);
        Caja2->setY(YInit);
        Caja2->setWidth(puntoFinal.x()-puntoInicio.x());
        Caja2->setHeight(puntoFinal.y()-puntoInicio.y());

        QString PosXIn=QString::number(XInit)+":Xinit";
        QString PosYIn=QString::number(YInit)+":Yinit";
        emit linkActivated(PosXIn);
        emit linkActivated(PosYIn);
        BotonLeft=true;
    }
}///EVENTO DE PRESION DE BOTON DEL MOUSE

void LabelEventMouse::mouseReleaseEvent(QMouseEvent *evn)
{
    if(pintaCaja==true)
    {
        //cout<<"INICIO X: "<<puntoInicio.x()<<" INICIO Y: "<<puntoInicio.y()<<endl;
        //cout<<"HEIGHT:  "<<Caja->height()<<"  ALTO:  "<<Caja->width()<<endl;
        emit EmitRegionExtract();
        BotonLeft=false;
    }
    //pintaCaja=false;
    //delete(Caja);
}///EVENTO DE LIBERACION DEL MOUSE

void LabelEventMouse::paintEvent(QPaintEvent *evn)
{
    QLabel::paintEvent(evn);
    QPainter pintor(this);
    QString PosMouseM = QString::number(XInit) + "," + QString::number(YInit);
    QString PosMouseM2 = QString::number(XInit) + "," + QString::number(YInit-HeightBox);

    if(pintaCaja==true && pintaPunto==false && BotonLeft==true && TypeB == 0){
        pintor.setPen(Qt::blue);
        pintor.drawRect(*Caja);
        pintor.drawText(XEnd,YEnd,PosMouseM);
    }else if(pintaCaja==true && pintaPunto==false && TypeB == 1){
        pintor.setPen(Qt::blue);
        pintor.drawRect(*Caja);        
        pintor.drawText(XInit,YInit,PosMouseM);
        pintor.setPen(Qt::red);
        pintor.drawText(XInit, YInit-HeightBox, PosMouseM2);
        if(BotonLeft == false){
            pintor.setPen(Qt::red);
            pintor.drawRect(*Caja2);
        }
    }

    if(pintaCaja==false && pintaPunto==true && BotonLeft==true)
    {
        pintor.setPen(Qt::black);
        pintor.drawEllipse(PosMouse,10,10);
    }
}///EVENTO DE PINTAR
