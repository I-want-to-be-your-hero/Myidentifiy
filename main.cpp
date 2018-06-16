#include "mainwindow.h"
#include <QApplication>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;

int main(int argc, char *argv[])
{ 

   // VideoCapture capture(0);

    //while(1)
    //{
    //    Mat frame;
    //    capture>>frame;
    //    imshow("video",frame);
    //    waitKey(39);
    //}

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
