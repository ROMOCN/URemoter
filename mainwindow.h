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
#include "Tools/toolvideo.h"
#include "Tools/toolaudio.h"
#include "VideoWidget/videowidget.h"
#include "Controls/ctrlmenu.h"
#include "Controls/ctrlinfowidget.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QWidget
{
    Q_OBJECT
    UDPClient *udpClient = nullptr;
    TCPClient *tcpClient = nullptr;
    VideoWidget *lab_video = nullptr;
    ToolVideo *t_video = nullptr;
    ToolAudio *tAudio = nullptr;
    CtrlMenu *menuEdge = nullptr;
    CtrlInfoWidget *infoWidget = nullptr;
    int UID = 0;
    int RoomID = 0;
     Ui::MainWindow *ui;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void slotRecVideo(QImage img);
    void slotRecvAudio(QByteArray audio);
    void slotPullVideo(QImage img);
    void slotPullAudio(QByteArray data);
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
        imageData = imageData.toBase64();
        return imageData;
    }
    QImage qbyteToImg(const QString &data)
    {
        QByteArray imageData = QByteArray::fromBase64(data.toLatin1());
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
};
#endif // MAINWINDOW_H
