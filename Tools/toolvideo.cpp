#include "toolvideo.h"

ToolVideo::ToolVideo(QObject *parent) : QObject(parent)
{
    init();
}

ToolVideo::~ToolVideo()
{
    capture.release();
}

void ToolVideo::init()
{
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&ToolVideo::running);
    connect(this, &ToolVideo::signalStart, this, &ToolVideo::startCamera);
    //        //# 获取视频帧的宽
    //        int w = capture.get(CAP_PROP_FRAME_WIDTH);
    //        //# 获取视频帧的高
    //        int h = capture.get(CAP_PROP_FRAME_HEIGHT);
    //        //# 获取视频帧的帧率
    //        int fps = capture.get(CAP_PROP_FPS);
    //    //# 设置摄像头分辨率的高
    //    capture.set(CAP_PROP_FRAME_HEIGHT, this->size().width());
    //    //# 设置摄像头分辨率的宽
    //    capture.set(CAP_PROP_FRAME_WIDTH, this->size().height());
}

void ToolVideo::start(int interval)
{
    emit signalStart(interval);
}

void ToolVideo::stop()
{
    if(capture.isOpened()){
        capture.release();
        timer->stop();
    }

}

bool ToolVideo::isRunning()
{
    return capture.isOpened();
}

void ToolVideo::running()
{
    if(capture.isOpened()){
        Mat frame;
        capture >> frame;
        QImage img = MatImageToQt(frame);
        img = img.mirrored(true, false);
        emit pullImage(img);
    }
}

void ToolVideo::startCamera(int interval)
{
    if(!capture.isOpened()){
        std::thread t =  std::thread([=](){
            //capture.open(0);
            //2.一定要先设置采集格式！！！
            capture.set(6, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));  //CV_CAP_PROP_FOURCC	6	4个字符表示的视频编码器格式
            //3.然后再设置高清采集分辨率
    //        capture.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
    //        capture.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);

            //1.打开摄像机设备（默认第一个）
            capture= cv::VideoCapture(0);
        });
        timer->start(interval);
        t.detach();
//        t->join();
//        delete  t;
    }
}


QImage ToolVideo::MatImageToQt(const Mat &src)
{
    //CV_8UC1 8位无符号的单通道---灰度图片
    if(src.type() == CV_8UC1)
    {
        //使用给定的大小和格式构造图像
        //QImage(int width, int height, Format format)
        QImage qImage(src.cols,src.rows,QImage::Format_Indexed8);
        //扩展颜色表的颜色数目
        qImage.setColorCount(256);

        //在给定的索引设置颜色
        for(int i = 0; i < 256; i ++)
        {
            //得到一个黑白图
            qImage.setColor(i,qRgb(i,i,i));
        }
        //复制输入图像,data数据段的首地址
        uchar *pSrc = src.data;
        //
        for(int row = 0; row < src.rows; row ++)
        {
            //遍历像素指针
            uchar *pDest = qImage.scanLine(row);
            //从源src所指的内存地址的起始位置开始拷贝n个
            //字节到目标dest所指的内存地址的起始位置中
            memcmp(pDest,pSrc,src.cols);
            //图像层像素地址
            pSrc += src.step;
        }
        return qImage;
    }
    //为3通道的彩色图片
    else if(src.type() == CV_8UC3)
    {
        //得到图像的的首地址
        const uchar *pSrc = (const uchar*)src.data;
        //以src构造图片
        QImage qImage(pSrc,src.cols,src.rows,src.step,QImage::Format_RGB888);
        //在不改变实际图像数据的条件下，交换红蓝通道
        return qImage.rgbSwapped();
    }
    //四通道图片，带Alpha通道的RGB彩色图像
    else if(src.type() == CV_8UC4)
    {
        const uchar *pSrc = (const uchar*)src.data;
        QImage qImage(pSrc, src.cols, src.rows, src.step, QImage::Format_ARGB32);
        //返回图像的子区域作为一个新图像
        return qImage.copy();
    }
    else
    {
        return QImage();
    }
}
