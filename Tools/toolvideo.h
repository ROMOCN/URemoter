#ifndef TOOLVIDEO_H
#define TOOLVIDEO_H

#include <QObject>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QTimer>
#include <QImage>
#include <thread>
#include <QBuffer>
#include <QPixmap>
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
    void start(int interval = 30);
    void stop();
    bool isRunning();
    void changeInterval(int interval)
    {
        timer->setInterval(interval);
    }

    QImage MatImageToQt(const Mat &src);
    QImage Compress(Mat mat);
signals:
    void pullImage(QImage img);
    void signalStart(int interval = 30);
private:
    void running();
    void startCamera(int interval = 30);

//    bool g_needstop =false;
//    void Record()
//    {
//         RECT rect;
//         //获取窗体位置大小
//         GetWindowRect(hd,&rect);
//         cv::Size frameSize;
//         frameSize.width=rect.right-rect.left;
//         frameSize.height=rect.bottom-rect.top;
//         cv::VideoWriter VideoWriter;
//         if(!VideoWriter.open("d:\\1.avi",CV_FOURCC('M', 'J', 'P', 'G'),40,frameSize))
//             return;
//        while(!g_needstop)
//        {
//            QPixmap pm;
//            GetGDIBitmap(hd,pm,0,0,frameSize.width,frameSize.height);
//            VideoWriter.write(ImageToMat(pm.toImage()));
//        }
//        VideoWriter.release();
//    }

//    Mat ImageToMat(QImage img,QString imgFormat)
//    {
//        if(img.isNull())
//            return Mat();
//        QByteArray ba;
//        QBuffer buffer(&ba);
//        buffer.open(QIODevice::WriteOnly);
//        img.save(&buffer,imgFormat.toLatin1().data());
//        _InputArray arrSrc(ba.data(), ba.size());
//        Mat mat = cv::imdecode(arrSrc, CV_LOAD_IMAGE_COLOR);
//        return mat;
//    }
//    void Record(Mat mat){
//        cv::VideoWriter VideoWriter;

//        if(!VideoWriter.open("d:\\1.avi",CV_FOURCC('M', 'J', 'P', 'G'),40,frameSize))
//            return;
//       while(!g_needstop)
//       {

//           VideoWriter.write(mat);
//       }
//       VideoWriter.release();
//    }


};

#endif // TOOLVIDEO_H
