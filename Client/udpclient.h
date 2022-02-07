#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>


//#ifndef RECV_BUFF_SIZE
//#define RECV_BUFF_SIZE 102400
//#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <QList>

#else
#include <unistd.h>
#include <arpa/inet.h>
#include <QString.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR			(~1)
#endif // _WIN32
#define BIG_DATA_SIZE 10240
#include <QString>
#include <QObject>
#include <QVariant>
#include <math.h>
#include <queue>
#include <thread>
#include <mutex>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

#include "sockcmd.h"
using namespace std;
using namespace CMD;
class UDPClient : public QObject
{
    Q_OBJECT
    const static int RECV_BUFF_SIZE = 102400;
    SOCKET _client;
    int _lastpos;//记录消息长度
    char _msgBuff[RECV_BUFF_SIZE * 10];
    char _recvBuff[RECV_BUFF_SIZE];
    mutex _lock; //临界资源保护
    mutex _thread_status; //控制线程开始暂停
    sockaddr_in _sin ={}; //IP地址
    sockaddr_in _addrServer = {};
public:
    UDPClient(QObject *parent = nullptr);
    virtual ~UDPClient();
    int initial_sock(QString &msg);
    int close_sock();
    int recv_data(QString &msg);
    bool run_select(QString &msg);
    bool is_run();
    int sendData(char *ip, int nPort, char *pData,int size);
    int sendData(char *ip, int nPort, DataHeader *data);
    int bindPort(const char *ip, unsigned short port);
signals:
    void signalAudio(QImage img);
    void signalRecv(QByteArray data);
private:
    int dataFactory();

};


#endif // UDPCLIENT_H
