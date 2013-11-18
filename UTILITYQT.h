#ifndef UTILITYQT_H
#define UTILITYQT_H

#include <qcolor.h>
#include <qimage.h>

#include <opencv2/opencv.hpp>

QImage MatTOQImage(cv::Mat &MatImage);

#endif // UTILITYQT_H
