#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QBuffer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMetaObject>
#include <QPushButton>
#include "Client/udpclient.h"
#include "Client/tcpclient.h"

#include "VideoWidget/videowidget.h"
#include "Controls/ctrlmenu.h"
#include "Controls/ctrlinfowidget.h"
#include "Controls/ctrllivecentre.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QWidget
{
    Q_OBJECT
    UDPClient *udpClient = nullptr;
    TCPClient *tcpClient = nullptr;
    VideoWidget *lab_video = nullptr;

    CtrlMenu *menuEdge = nullptr;
    CtrlInfoWidget *infoWidget = nullptr;
    CtrlLiveCentre *centre = nullptr;
    int UID = 0;
    int RoomID = 0;
     Ui::MainWindow *ui;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void slotRecVideo(QImage img);
    void slotRecvAudio(QByteArray audio);
    void slotPushVideo(QImage img);
    void slotPushAudio(QByteArray data);
    static unsigned char* charToUc(char* read_buff)
    {
        return  reinterpret_cast<unsigned char*> (read_buff);
    }
    static char* ucToChar(unsigned char* u_read_buff)
    {
        return  reinterpret_cast<char*> (u_read_buff);
    }
    QByteArray imgToQByte(const QImage &image)
    {
        QByteArray imageData;
        QBuffer buffer(&imageData);
        image.save(&buffer, "jpg");    

        imageData = qCompress(imageData);//使用默认的zlib压缩
        qDebug() << "压缩后字节大小:" << buffer.size();

        imageData = imageData.toBase64();


        return imageData;
    }
    QImage qbyteToImg(const QString &data)
    {
        QByteArray dataUnCom = data.toLatin1();
        dataUnCom = qUncompress(dataUnCom);
        QByteArray imageData = QByteArray::fromBase64(dataUnCom);

        imageData = qUncompress(imageData);
        qDebug() << "解压缩后字节大小:" << imageData.size();

        QImage image;
        image.loadFromData(imageData);
        return image;
    }
    QImage qbyteToImg(const QByteArray data)
    {
        QByteArray imageData = qUncompress(data);
        imageData = QByteArray::fromBase64(imageData);

        imageData = qUncompress(imageData);
        qDebug() << "解压缩后字节大小:" << imageData.size();

        QImage image;
        image.loadFromData(imageData);
        return image;
    }
signals:
    /*将跨线程信号转为同线程信号*/
    void signalTCPRecv(QByteArray data);
    void signalUDPRecv(QByteArray datar);

private slots:
    void slotTCPRecv(QByteArray data);
    void slotUDPRecv(QByteArray data);
    void slotCreateRoom();
    void slotJoinRoom(int roomID);
private:
    void init();
    void initThread();
    void initSignal();
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void mergeDatas(QVector<QByteArray> Datas);
};
#endif // MAINWINDOW_H
