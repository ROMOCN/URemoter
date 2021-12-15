#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMetaObject>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    client->close_sock();
    // 释放VideoCapture资源
    capture.release();
    delete ui;
}

void MainWindow::slotVideo(QImage img)
{
    img = img.mirrored(true, false);
    lab_video->setPixmap(QPixmap::fromImage(img.scaled(this->size())));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    lab_video->resize(this->size());
//    //# 设置摄像头分辨率的高
//    capture.set(CAP_PROP_FRAME_HEIGHT, this->size().width());
//    //# 设置摄像头分辨率的宽
//    capture.set(CAP_PROP_FRAME_WIDTH, this->size().height());
}


void MainWindow::init()
{
    lab_video = new QLabel(this);
    lab_video->move(0,0);
    client = new UDPClient();
    timer = new QTimer();
    QPushButton *btn = new QPushButton("录制",this);
    QString msg;
//    //# 设置摄像头分辨率的高
//    capture.set(CAP_PROP_FRAME_HEIGHT, this->size().width());
//    //# 设置摄像头分辨率的宽
//    capture.set(CAP_PROP_FRAME_WIDTH, this->size().height());
    btn->move(0,0);
    client->initial_sock(msg);
    client->bindPort("0.0.0.0", 8888);
    connect(client, &UDPClient::signalAudio, this, &MainWindow::slotVideo);
    connect(timer,&QTimer::timeout,this,&MainWindow::play);
//    connect(btn, &QPushButton::clicked,[=](){
//        QMetaObject::invokeMethod(this, "start", Qt::AutoConnection);  //需添加标记 Q_INVOKABLE  Qt::DirectConnection
//    });
    connect(btn, &QPushButton::clicked,this, &MainWindow::start);
    std::thread *t = new std::thread([=](){
        QString msg;
        while(true)
        {
            client->run_select(msg);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    });
    t->detach();

}
void MainWindow::start()
{

    if(capture.isOpened())
    {
        capture.release();
        timer->stop();
        lab_video->clear();
    }else
    {
        std::thread *t = new std::thread([=](){
            capture.open(0);
        });
        timer->start(30);
    }
}

void MainWindow::play()
{
    if(capture.isOpened()){
        Mat frame;
        capture >> frame;
        QImage imag = MatImageToQt(frame);
        int size = 0;
        QByteArray buff = imgToQByte(imag);
        size = buff.size();
//        //数据加密
//        QByteArray base64Byte = buff.toBase64();
//        //数据压缩算法
//        QByteArray compressByte = qCompress(base64Byte,1);
//        //# 获取视频帧的宽
//        int w = capture.get(CAP_PROP_FRAME_WIDTH);
//        //# 获取视频帧的高
//        int h = capture.get(CAP_PROP_FRAME_HEIGHT);
//        //# 获取视频帧的帧率
//        int fps = capture.get(CAP_PROP_FPS);
        client->sendData("127.0.0.1",8888, buff.data(), buff.size());
        client->sendData("192.168.1.107",8888, buff.data(), buff.size());


    }

}

QImage MainWindow::MatImageToQt(const Mat &src)
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

