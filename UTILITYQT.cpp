#include "UTILITYQT.h"
#include <QDebug>

QImage MatTOQImage(cv::Mat &MatImage)
{
    QImage Temp;
    QColor pix;
    int canales=0,profundidad=0,filas=0,colum=0;
    canales=MatImage.channels();
    profundidad=MatImage.depth();
    filas=MatImage.rows;
    colum=MatImage.cols;
    pix.setRgb(0,0,255,255);
    //qDebug()<<"filas:  "<<filas<<" Profund:"<<profundidad<<"  colum:"<<colum<<" CANALES:  "<<canales;
    if(filas>0 && colum >0){
        if(profundidad==CV_8U){///PROFUNDIDAD DE 8 BITS SIN SIGNO
            ///qDebug()<<"8 BITS";
            Temp=QImage(colum,filas,QImage::Format_RGB888);
            uchar r=0,g=0,b=0;
            if(canales==1){
                //qDebug()<<"1 CANALES";
                for(int fil=0;fil<filas;fil++)
                {
                    for(int col=0;col<colum;col++)
                    {
                        b=MatImage.at<uchar>(fil,col);
                        g=MatImage.at<uchar>(fil,col);
                        r=MatImage.at<uchar>(fil,col);
                        pix.setRgb(r,g,b,255);
                        Temp.setPixel(col,fil,pix.rgb());
                    }
                }
            }else{
                if(canales==3){
                    //qDebug()<<"AQUI";
                    std::vector<cv::Mat> planos;
                    cv::split(MatImage,planos); ///TENGO PROBLEMAS AQUI SIN LA INTERFAZ DE QTCREATOR
                    //qDebug()<<"AQUI2";

                    for(int fil=0;fil<filas;fil++)
                    {
                        for(int col=0;col<colum;col++)
                        {
                            b=planos[0].at<uchar>(fil,col);
                            g=planos[1].at<uchar>(fil,col);
                            r=planos[2].at<uchar>(fil,col);
                            pix.setRgb(r,g,b,255);
                            Temp.setPixel(col,fil,pix.rgb());
                        }
                    }
                    //qDebug()<<"3 CANALES";
                }else{
                    qDebug()<<"Conversion de canales mayores a 3 no implementada....";
                }
            }
        }else{
            qDebug()<<"Conversion aun no implementada, solo valida para imagenes de profundidad de 8 bits...";
        }
    }else{
        qDebug()<<"LA IMAGEN NO SE CARGO CORRCETAMENTE.....";
    }

    return(Temp);
}
