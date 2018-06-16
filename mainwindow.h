#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<opencv2/opencv.hpp>


class QString;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_0_triggered();

    void on_action_G_triggered();

    void on_action_E_2_triggered();

    void on_action_M_triggered();

    void on_action_H_triggered();

    void on_action_S_triggered();

    void on_action_T_triggered();

    void on_action_Q_triggered();

    void on_action_R_triggered();

    void on_action_G_2_triggered();

private:
    Ui::MainWindow *ui;
    QString curfile;
    cv::Mat srcImage;
    cv::Mat greyImage;
    cv::Mat CannyImage;
    cv::Mat medianImage;
    cv::Mat houghImage;
    cv::Mat closeImage;
    cv::Mat contourImage;
    cv::Mat pointImage;
    vector<cv::Point> resultPoint;
    QImage *src;
    void loadimage(const QString &fileName);
    void cannyphoto();
    void toGray();
    void medianphoto();
    void houghphoto();
    void closephoto();
    void findCountour();
    void getPoint();
    void showresult();
    void showImage(const QString& str);

};

#endif // MAINWINDOW_H
