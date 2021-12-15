#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>


#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240
#endif

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
#include "sockcmd.h"
using namespace std;
using namespace CMD;
class TCPClient : public QObject
{
    Q_OBJECT
    SOCKET _client;
    int _lastpos;//记录消息长度
    char _msgBuff[RECV_BUFF_SIZE * 10];
    char _recvBuff[RECV_BUFF_SIZE];
//    map<long long,Datas> data_buff; //大消息缓冲
//    map<long long,int> data_buff_size; //大消息缓冲单元大小
    std::thread *thread_msg;
    mutex _lock; //临界资源保护
    mutex _thread_status; //控制线程开始暂停
    bool thread_on = true;
    sockaddr_in _sin ={}; //IP地址
public:
    TCPClient();
    TCPClient(QObject *parent);
    virtual ~TCPClient();
    int initial_sock(QString &msg);
    int connect_server(char* ip, unsigned short port, QString &msg);
    int close_sock();
    int recv_data(QString &msg);
    bool run_select(QString &msg);
    bool is_run();
    int send_data(DataHeader *data);
signals:
    void signal_server_cmd();
    void signal_log_result(short result);
    void signal_recv_msg(long long id, QString msg, short cmd);
    void signal_get_friend(QVariant qvar);
private:
//    void thread_init();
//    void thread_data_fact();
//    void big_data_factory(long long recv_id,
//                          long long sour_id,
//                          char* data,int len,
//                          ENUM_CMD cmd_kind,
//                          ENUM_SEND_FORMAT format_kind);

    int data_factory(DataHeader *header);

};

#endif // TCPCLIENT_H
