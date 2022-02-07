#ifndef TOOLVIDEO_H
#define TOOLVIDEO_H

#include <QObject>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QTimer>
#include <QImage>
#include <thread>
using namespace cv;
class ToolVideo : public QObject
{
    Q_OBJECT
    VideoCapture capture;
    QTimer *timer = nullptr;

public:
    explicit ToolVideo(QObject *parent = nullptr);
    ~ToolVideo();
    void init();
    void start();
    void stop();
    bool isRunning();
    QImage MatImageToQt(const Mat &src);
signals:
    void pullImage(QImage img);
private:
    void running();


};

#endif // TOOLVIDEO_H
