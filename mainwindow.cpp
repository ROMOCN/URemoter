#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>



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
    delete centre;
    delete ui;
}

QString MainWindow::getLocalIP() const
{
    QString ip;
    QString localHostName = "";
    localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);
    qDebug()<<info.addresses();
    foreach(QHostAddress address,info.addresses()){
         if(address.protocol() == QAbstractSocket::IPv4Protocol){
            ip = address.toString();
         }
    }
    qDebug() << ip;
    return ip;
}



void MainWindow::slotRecVideo(QImage img)
{
    lab_video->setVideo(img, 1);
}

void MainWindow::slotRecvAudio(QByteArray audio)
{
    centre->recvAudio(audio);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    lab_video->resize(this->size().width() - 80, this->size().height());
    menuEdge->resize(80, this->size().height());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    centre->close();

}


void MainWindow::init()
{
    this->resize(1200 ,900 );
    QString msg;

    centre = new CtrlLiveCentre();
    centre->show();

    udpClient = new UDPClient();
    tcpClient = new TCPClient();
    menuEdge = new CtrlMenu(this);
    infoWidget = new CtrlInfoWidget(this);
    lab_video = new VideoWidget(this);

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

    QString ip = getLocalIP();
//    qDebug()<<"local ip is : " << ip;
//    qDebug()<<"do you want to change it? y/n";
//    std::string ret;
//    scanf("%s", &ret);
//    if(ret == "y" || ret == "yes" ||ret == "Y" || ret == "YES"){
//        qDebug()<<"new ip is:";
//        scanf("%s", &ip);
//    }

    tcpClient->connect_server(ip.toLocal8Bit().data(), 8000, msg);
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
    //connect(udpClient, &UDPClient::signalAudio, this, &MainWindow::slotRecVideo);
    connect(tcpClient,&TCPClient::signalNetState, menuEdge, &CtrlMenu::setNetState);
    connect(centre, &CtrlLiveCentre::signalPushImg, this, &MainWindow::slotPushVideo);
    connect(centre, &CtrlLiveCentre::signalPushAudio, this, &MainWindow::slotPushAudio);


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
    static int cmdId = 0;
    static QVector<QByteArray> Datas;
    static long time = 0;
    if(data.size() > 0){
        CMD::ENUM_STREAM myStream;
        QByteArray anaData = CMD::JsonTool::analyseData(data, "Data", myStream);
        if(anaData.size() > 0){
            switch (myStream) {
            case ENUM_STREAM::STREAM_VIDEO:
            {
                int tempCID = CMD::JsonTool::analyseID(data,"CMDID");
                int packNum = CMD::JsonTool::analyseID(data,"Number");
                int packIndex = CMD::JsonTool::analyseID(data,"Index");
                if(tempCID == 0){
                    QByteArray baseData = QByteArray::fromBase64(anaData);
                    baseData = qUncompress(baseData);
                    QImage image;
                    if(image.loadFromData(baseData))
                        slotRecVideo(image);
                }else{
                    if(tempCID != cmdId){
                        mergeDatas(Datas);
                        time = CMD::GetTime();
                        cmdId = tempCID;
                        Datas.clear();
                        Datas.resize(packNum);
                        Datas[packIndex] = anaData.data();

                    }else{
                        long tempTime = CMD::GetTime();
                        Datas[packIndex] = anaData.data();

                    }
                }



            }
                break;
            case ENUM_STREAM::STREAM_AUDIO:
            {
                QByteArray baseData = QByteArray::fromBase64(anaData);
                slotRecvAudio(baseData);
            }
                break;
            }

        }
    }
}
void MainWindow::mergeDatas(QVector<QByteArray> Datas){
    if(Datas.size() > 0){
        QByteArray data;
        for(auto i : Datas){
            if(i.isEmpty() || i.isNull()){
                return;
            }
            data.push_back(i);
        }
        QByteArray baseData = QByteArray::fromBase64(data);
        baseData = qUncompress(baseData);
        QImage image;
        if(image.loadFromData(baseData))
            slotRecVideo(image);
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

void MainWindow::slotPushVideo(QImage img)
{
        int size = 0;
        QByteArray buff = imgToQByte(img);
        //img.save("C:/Users/49776/Desktop/jietu.jpg");
        size = buff.size();
//        //数据加密
//        QByteArray base64Byte = buff.toBase64();
//        //数据压缩算法
//        QByteArray compressByte = qCompress(base64Byte,1);

//        //-----------------json-------------------------
//        QByteArray byteArray = CMD::JsonTool::sendStream(buff);
//        //-----------------json-------------------------
//        //udpClient->sendData("192.168.5.5",8888, byteArray.data(), byteArray.size());
//        udpClient->sendData("192.168.5.5",8001, byteArray.data(), byteArray.size());
//        //lab_video->setVideo(img.mirrored(true, false));
//        lab_video->setVideo(img);
//        //lab_video->setVideo(screen,1);

        QQueue<QByteArray> datas = CMD::JsonTool::sendBigStream(buff);

        while(datas.count() > 0){
            QByteArray jsondata =  datas.front();
            datas.pop_front();
            udpClient->sendData("192.168.5.5",8001, jsondata.data(), jsondata.size());//8001
        }

}

void MainWindow::slotPushAudio(QByteArray data)
{
    int size = 0;
    QByteArray buff(data);
    //slotRecvAudio(data);
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
    udpClient->sendData("192.168.5.5",8001, byteArray.data(), byteArray.size());
}

