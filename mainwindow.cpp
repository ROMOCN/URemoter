#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    udpClient->close_sock();
    delete ui;
}

void MainWindow::slotRecVideo(QImage img)
{
    img = img.mirrored(true, false);
    lab_video->setVideo(img, 1);
}

void MainWindow::slotRecvAudio(QByteArray audio)
{
    tAudio->soltPlay(audio);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    lab_video->resize(this->size().width() - 80, this->size().height());
    menuEdge->resize(80, this->size().height());
}


void MainWindow::init()
{
    this->resize(1200 ,900 );
    QString msg;
    t_video = new ToolVideo(this);
    udpClient = new UDPClient();
    tcpClient = new TCPClient();
    menuEdge = new CtrlMenu(this);
    infoWidget = new CtrlInfoWidget(this);
    lab_video = new VideoWidget(this);
    tAudio = new ToolAudio(this);
    infoWidget->move(menuEdge->minWidth, 0);
    {
        QSize size = this->size();
        size -= infoWidget->size();
        infoWidget->move(size.width()/2, size.height()/2);
    }
    menuEdge->move(0, 0);
    menuEdge->resize(menuEdge->minWidth, this->width());
    lab_video->hide();
    initSignal();

    udpClient->initial_sock(msg);
    udpClient->bindPort("0.0.0.0", 8888);
    tcpClient->initial_sock(msg);
    tcpClient->connect_server("192.168.1.103", 8000, msg);
    lab_video->move(menuEdge->minWidth,0);
    lab_video->setWindowFlags(lab_video->windowFlags() | Qt::WindowStaysOnBottomHint);


    initThread();
}

void MainWindow::initThread()
{
    std::thread *tcpThread = new std::thread([=](){
        QString msg;
        while(true){
            tcpClient->run_select(msg);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    tcpThread->detach();
    std::thread *udpThread = new std::thread([=](){
        QString msg;
        while(true){
            udpClient->run_select(msg);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    udpThread->detach();
}

void MainWindow::initSignal()
{
    //qRegisterMetaType<QByteArray>("QByteArray");
    connect(tcpClient, &TCPClient::signalRecv, this, &MainWindow::signalTCPRecv);
    connect(udpClient, &UDPClient::signalRecv, this, &MainWindow::signalUDPRecv);
    connect(this, &MainWindow::signalTCPRecv, this, &MainWindow::slotTCPRecv);
    connect(this, &MainWindow::signalUDPRecv, this, &MainWindow::slotUDPRecv);
    connect(infoWidget, &CtrlInfoWidget::signalCreateRoom, this, &MainWindow::slotCreateRoom);
    connect(infoWidget, &CtrlInfoWidget::signalJoinRoom, this, &MainWindow::slotJoinRoom);
    connect(udpClient, &UDPClient::signalAudio, this, &MainWindow::slotRecVideo);
    connect(tcpClient,&TCPClient::signalNetState, menuEdge, &CtrlMenu::setNetState);
    QPushButton *btn = new QPushButton("录制",this);
    btn->move(80,0);
    connect(btn, &QPushButton::clicked,[=](){
        if(t_video->isRunning()){
            t_video->stop();
            tAudio->stopRecord();
        }else{
            t_video->start();
            tAudio->startRecord();

        }
    });
    connect(t_video, &ToolVideo::pullImage, this, &MainWindow::slotPullVideo);
    connect(tAudio, &ToolAudio::signalPushSound, this ,&MainWindow::slotPullAudio);
}

void MainWindow::slotTCPRecv(QByteArray data)
{
    const QString cmd = "ENUM_CMD";
    const QString streamType = "ENUM_STREAM";
    ENUM_CMD c = (ENUM_CMD)CMD::JsonTool::analyseID(data, cmd);
    switch (c) {
    case CMD_SENDUID:{
        int uid = CMD::JsonTool::analyseID(data,"UID");
        UID = uid == 0? 0:  uid;
        infoWidget->setRoomID(uid);
    }
        break;
    }
}

void MainWindow::slotUDPRecv(QByteArray data)
{
    CMD::ENUM_STREAM myStream;
    QByteArray baseData = QByteArray::fromBase64(CMD::JsonTool::analyseData(data, "Data", myStream));
    switch (myStream) {
    case ENUM_STREAM::STREAM_VIDEO:
    {
        QImage image;
        if(image.loadFromData(baseData))
            slotRecVideo(image);
    }
        break;
    case ENUM_STREAM::STREAM_AUDIO:
        slotRecvAudio(baseData);
        break;
    }

}

void MainWindow::slotCreateRoom()
{
    infoWidget->hide();
    lab_video->show();
    QByteArray s = CMD::JsonTool::creatRoom(UID);
    tcpClient->sendData(s.data(), s.size());
}

void MainWindow::slotJoinRoom(int roomID)
{
    infoWidget->hide();
    lab_video->show();
    QByteArray data = CMD::JsonTool::joinRoom(roomID);
    int sendret =  tcpClient->sendData(data.data(), data.size());
}

void MainWindow::slotPullVideo(QImage img)
{
        int size = 0;
        QByteArray buff = imgToQByte(img);
        size = buff.size();
//        //数据加密
//        QByteArray base64Byte = buff.toBase64();
//        //数据压缩算法
//        QByteArray compressByte = qCompress(base64Byte,1);
        //-----------------json-------------------------
        QByteArray byteArray = CMD::JsonTool::sendStream(buff);
        //-----------------json-------------------------
        //udpClient->sendData("127.0.0.1",8888, byteArray.data(), byteArray.size());
//        udpClient->sendData("192.168.1.107",8888, byteArray.data(), byteArray.size());
        udpClient->sendData("192.168.1.103",8001, byteArray.data(), byteArray.size());
        lab_video->setVideo(img.mirrored(true, false));
}

void MainWindow::slotPullAudio(QByteArray data)
{
    int size = 0;
    QByteArray buff(data);
    size = buff.size();
//  //数据加密
//  QByteArray base64Byte = buff.toBase64();
//  //数据压缩算法
//  QByteArray compressByte = qCompress(base64Byte,1);
    //-----------------json-------------------------
    QByteArray byteArray = CMD::JsonTool::sendStream(buff.toBase64(), ENUM_STREAM::STREAM_AUDIO);
    //-----------------json-------------------------
    //udpClient->sendData("127.0.0.1",8888, byteArray.data(), byteArray.size());
//        udpClient->sendData("192.168.1.107",8888, byteArray.data(), byteArray.size());
    udpClient->sendData("192.168.1.103",8001, byteArray.data(), byteArray.size());
}

