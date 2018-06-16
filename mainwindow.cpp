#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QMessageBox>
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    src =new QImage;
    ui->action_G->setDisabled(true);
    ui->action_M->setDisabled(true);
    ui->action_E_2->setDisabled(true);
    ui->action_H->setDisabled(true);
    ui->action_S->setDisabled(true);
    ui->action_T->setDisabled(true);
    ui->action_Q->setDisabled(true);
    ui->action_R->setDisabled(true);
}

MainWindow::~MainWindow()
{
    QString str=tr("result.jpg");
    if(src->load(str))
    {
        remove("result.jpg");
    }
    delete ui;
}

void MainWindow::on_action_0_triggered()
{
    curfile = QFileDialog::getOpenFileName();
    if(!curfile.isEmpty())
    {
        loadimage(curfile);
        setWindowTitle(curfile);
    }

}

void MainWindow::loadimage(const QString &fileName)
{
    src = new QImage ;
    if(!src->load(fileName))
    {
        QMessageBox::warning(this,tr("错误"),tr("无法读取%1 \n ").arg(curfile));
        delete src;
        return ;

    }
    String filename =fileName.toStdString();
    srcImage = imread(filename,1);
    QPixmap pixmap=QPixmap::fromImage(*src);
    cv::resize(srcImage, srcImage, Size(1280,760));
    QPixmap fitpimap=pixmap.scaled(640,480,Qt::IgnoreAspectRatio);
    //src->scaled(ui->label->width(),ui->label->height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   //ui->label->setScaledContents(true);
    ui->label->setPixmap(fitpimap);
    ui->action_G->setEnabled(true);
}

void MainWindow::cannyphoto()
{
    remove("medianImage.jpg");
    Canny(medianImage, CannyImage, 20, 40, 3);//进行一此canny边缘检测
    imwrite("CannyImage.jpg",CannyImage);
    ui->action_E_2->setDisabled(true);
    ui->action_H->setEnabled(true);
    QString str=tr("CannyImage.jpg");
    showImage(str);


}

void MainWindow::toGray()
{
    cvtColor(srcImage, greyImage, CV_BGR2GRAY);
    imwrite("greyImage.jpg",greyImage);
    ui->action_G->setDisabled(true);
    ui->action_M->setEnabled(true);
    QString str=tr("greyImage.jpg");
    showImage(str);
}

void MainWindow::medianphoto()
{
    remove("greyImage.jpg");
    medianBlur(greyImage, medianImage, 5);  //中值滤波
    imwrite("medianImage.jpg",medianImage);

    QString str=tr("medianImage.jpg");
    showImage(str);
    ui->action_M->setDisabled(true);
    ui->action_E_2->setEnabled(true);



}

void MainWindow::houghphoto()
{
    remove("CannyImage.jpg");
    vector<Vec4i> lines;
    HoughLinesP(CannyImage, lines, 1, CV_PI / 180, 40, 10, 30);
    Mat photo1(srcImage.rows, srcImage.cols, CV_8UC1, Scalar(0, 0, 0));
    for (size_t i = 0; i < lines.size(); i++)
    {
        Vec4i l = lines[i];
        line(photo1, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 2);
     }
    houghImage = photo1;
    imwrite("houghImage.jpg",houghImage);
    QString str=tr("houghImage.jpg");
    showImage(str);
    ui->action_H->setDisabled(true);
    ui->action_S->setEnabled(true);
}

void MainWindow::closephoto()
{
    remove("houghImage.jpg");
    //二值化
    threshold(houghImage, closeImage, 50, 255, CV_THRESH_BINARY);
    //闭运算
    Mat element5(5, 5, CV_8U, cv::Scalar(1));//5*5正方形，8位uchar型，全1结构元素  ;
    morphologyEx(closeImage, closeImage, cv::MORPH_CLOSE, element5, Point(-1, -1), 1);
    imwrite("closeImage.jpg",closeImage);
    QString str=tr("closeImage.jpg");
    showImage(str);
    ui->action_S->setDisabled(true);
    ui->action_T->setEnabled(true);

}

void MainWindow::findCountour()
{
    remove("closeImage.jpg");
    Mat dst_contour1, dst_contour2;
    dst_contour1 = closeImage.clone();
    dst_contour2 = closeImage.clone();
        //检测所有外轮廓
    vector<vector<Point> > contours_out;
    vector<Vec4i> hierarchy_out;
        findContours(dst_contour1, contours_out, hierarchy_out, RETR_EXTERNAL, CHAIN_APPROX_NONE);//第二个参数存储着每条轮廓上所有点的坐标，第三个参数存储着轮廓的其他信息（子、父轮廓之类的）

        //检测所有轮廓(包括内、外轮廓）
    vector<vector<Point> > contours_all;
    vector<Vec4i> hierarchy_all;
    findContours(dst_contour2, contours_all, hierarchy_all, RETR_TREE, CHAIN_APPROX_NONE);
    if (contours_all.size() == contours_out.size()) return ;//没有内轮廓，则提前返回，此时所有轮廓都是外轮廓

    for (int i = 0; i < contours_out.size(); i++)
        {
            int conloursize = contours_out[i].size();
            for (int j = 0; j < contours_all.size(); j++)
            {
                int tem_size = contours_all[j].size();
                if (conloursize == tem_size)
                {
                    swap(contours_all[j], contours_all[contours_all.size() - 1]);
                    contours_all.pop_back();
                    break;
                }
            }
        }

        //contours_all中只剩下内轮廓
        //查找最大轮廓
        double maxarea = 0;
        int maxAreaIdx = 0;
        for (int index = contours_all.size() - 1; index >= 0; index--)
        {
            double tmparea = fabs(contourArea(contours_all[index])); //fab求绝对值 contourArea计算图像轮廓的面积
            if (tmparea>maxarea)
            {
                maxarea = tmparea;
                maxAreaIdx = index;//记录最大轮廓的索引号
            }
        }

        vector<Point> contourlist;
        contourlist = contours_all[maxAreaIdx];

        //查找第二大的轮廓

        double secondarea = 0;
        int secondAreaidx = 0;
        vector<vector<Point> > contours_all1 = contours_all;
        swap(contours_all1[maxAreaIdx], contours_all1[contours_all1.size() - 1]);
        contours_all1.pop_back();   //去掉最大的轮廓
        for (int index0 = contours_all1.size() - 1; index0 >= 0; index0--)
        {
            double tmparea0 = fabs(contourArea(contours_all1[index0]));
            if (tmparea0>secondarea)
            {
                secondarea = tmparea0;
                secondAreaidx = index0;
            }
        }
        vector<Point> contourlist0;
        contourlist0 = contours_all1[secondAreaidx];
        //画出轮廓区域
        vector<vector<Point> > result_contour;
        result_contour.push_back(contourlist);
        result_contour.push_back(contourlist0);
        Mat contour_photo(srcImage.rows, srcImage.cols, CV_8UC1, Scalar(0, 0, 0));
        drawContours(contour_photo, result_contour, -1, Scalar(255, 255, 0), -1, 8);

        Mat contour111 = contour_photo.clone();
        //闭运算连接两个区域
        Mat element6(7, 7, CV_8U, cv::Scalar(1));//5*5正方形，8位uchar型，全1结构元素
        morphologyEx(contour_photo, contour_photo, cv::MORPH_CLOSE, element6, Point(-1, -1), 1);
        contourImage=contour_photo;
        imwrite("contourImage.jpg",contourImage);
        QString str=tr("contourImage.jpg");
        showImage(str);
        ui->action_T->setDisabled(true);
        ui->action_Q->setEnabled(true);


}

void MainWindow::getPoint()
{
    remove("contourImage.jpg");
    vector<Vec2f> lines1;
        Canny(contourImage, contourImage, 20, 40, 3);//进行一此canny边缘检测
        HoughLines(contourImage, lines1, 1, CV_PI / 180, 100, 0, 0);
        //直线拟合
        int A = 10;
        double B = CV_PI /18 ;
        while (1)
        {
            for (size_t i = 0; i < lines1.size(); i++)
            {
                for (size_t j = 0; j < lines1.size(); j++)
                {
                    double rho1 = lines1[i][0];
                    double threta1 = lines1[i][1];
                    if (j != i)
                    {
                        double rho2 = lines1[j][0];
                        double threta2 = lines1[j][1];
                        if (abs(rho1 - rho2) < A && abs(threta1 - threta2) < B)
                        {
                            swap(lines1[j], lines1[lines1.size() - 1]);
                            lines1.pop_back();
                            break;
                        }

                    }
                }
            }
            if (lines1.size() > 4)
            {
                A = A + 1;
                B = B + CV_PI / 180;

            }
            else break;

        }
        Mat angle_point(srcImage.rows, srcImage.cols, CV_8UC3, Scalar(0, 0, 0));

            for (size_t i = 0; i < lines1.size(); i++)
            {
                double rho = lines1[i][0], theta = lines1[i][1];
                Point pt1, pt2;
                {
                    double a = cos(theta), b = sin(theta);
                    double x0 = a*rho, y0 = b*rho;
                    pt1.x = cvRound(x0 + 1000 * (-b));
                    pt1.y = cvRound(y0 + 1000 * (a));
                    pt2.x = cvRound(x0 - 1000 * (-b));
                    pt2.y = cvRound(y0 - 1000 * (a));
                    line(angle_point, pt1, pt2, Scalar(55, 100, 195), 1, CV_AA);
                }
            }
            //求交点
                vector<Point> point;
                for (size_t m = 0; m < lines1.size(); m++)
                {
                    double Rho1 = lines1[m][0];
                    double Threta1 = lines1[m][1];
                    for (size_t n = 0; n < lines1.size(); n++)
                    {
                        if (n != m)
                        {
                            double Rho2 = lines1[n][0];
                            double Threta2 = lines1[n][1];
                            //两直线之间的角度在某一范围内，两直线的交点才算是有效的，利用数学公式求得交点
                            if ((abs(Threta1 - Threta2) > CV_PI / 3 )&& (abs(Threta1 - Threta2) < CV_PI *5/ 6))
                            {
                                double a1 = cos(Threta1); double b1 = sin(Threta1);
                                double a2 = cos(Threta2); double b2 = sin(Threta2);
                                Point pt;
                                pt.x = static_cast<int>((Rho2*b1 - Rho1*b2) / (a2*b1 - a1*b2));
                                if (b1!=0)
                                pt.y = static_cast<int>((Rho1 - a1*pt.x) / b1);
                                else pt.y = static_cast<int>((Rho2 - a2*pt.x) / b2);
                                point.push_back(pt);
                                /*circle(angle_point, pt, 3, Scalar(255, 0, 0), 1, CV_AA);*/
                            }

                        }
                    }
                }



                //点的拟合
                int C = 5;
                while (1)
                {

                    for (size_t n = 0; n < point.size(); n++)
                    {
                        double xx1 = point[n].x;
                        double yy1 = point[n].y;
                        for (size_t m = 0; m < point.size(); m++)
                        {
                            if (m != n)
                            {
                                double xx2 = point[m].x;
                                double yy2 = point[m].y;
                                if (sqrt((xx1 - xx2)*(xx1 - xx2) + (yy1 - yy2)*(yy1 - yy2)) < C)
                                {
                                    swap(point[m], point[point.size() - 1]);
                                    point.pop_back();
                                    break;
                                }
                            }

                        }
                    }
                    if (point.size() > 4)
                    {
                        C += 1;

                    }
                    else break;
                }

                //圈出四个点
                for (size_t n = 0; n < point.size(); n++)
                {
                    resultPoint.push_back(point[n]);
                    circle(angle_point, point[n], 3, Scalar(255, 0, 0), 5, CV_AA);
                }
            imwrite("pointImage.jpg",angle_point);
            QString str=tr("pointImage.jpg");
            showImage(str);
            ui->action_Q->setDisabled(true);
            ui->action_R->setEnabled(true);


}

void MainWindow::showresult()
{
    remove("pointImage.jpg");
    Mat &&result=srcImage.clone();
    for (size_t n = 0; n < resultPoint.size(); n++)
        {
            circle(result, resultPoint[n], 3, Scalar(255, 0, 0), 1, CV_AA);
        }
    imwrite("result.jpg",result);
    QString str=tr("result.jpg");
    showImage(str);
    ui->action_R->setDisabled(true);
}

void MainWindow::on_action_G_triggered()
{
    toGray();
}

void MainWindow::on_action_E_2_triggered()
{
    cannyphoto();
}

void MainWindow::on_action_M_triggered()
{
    medianphoto();
}

void MainWindow:: showImage(const QString &str)
{

    if(!src->load(str))
    {
        QMessageBox::warning(this,tr("错误"),tr("无法读取%1").arg(str));
        delete src;
        return ;
    }
    QPixmap pixmap=QPixmap::fromImage(*src);
    QPixmap fitpimap=pixmap.scaled(640,480,Qt::IgnoreAspectRatio);
    ui->label->setPixmap(fitpimap);
}

void MainWindow::on_action_H_triggered()
{
    houghphoto();
}

void MainWindow::on_action_S_triggered()
{
    closephoto();
}

void MainWindow::on_action_T_triggered()
{
    findCountour();
}

void MainWindow::on_action_Q_triggered()
{
    getPoint();
}

void MainWindow::on_action_R_triggered()
{
    showresult();
}

void MainWindow::on_action_G_2_triggered()
{
    toGray();
    medianphoto();
    cannyphoto();
    houghphoto();
    closephoto();
    findCountour();
    getPoint();
    showresult();

}

