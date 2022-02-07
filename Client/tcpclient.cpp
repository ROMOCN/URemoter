#include "tcpclient.h"

TCPClient::TCPClient(QObject *parent) : QObject(parent)
{
    //qRegisterMetaType<QByteArray>("QByteArray");

    _client = INVALID_SOCKET;
    _recvBuff[RECV_BUFF_SIZE] = {};
    _msgBuff[RECV_BUFF_SIZE * 10] = {};
    _lastpos = 0;

}

TCPClient::TCPClient()
{
    //qRegisterMetaType<QByteArray>("QByteArray");

    _client = INVALID_SOCKET;
    _recvBuff[RECV_BUFF_SIZE] = {};
    _msgBuff[RECV_BUFF_SIZE * 10] = {};
    _lastpos = 0;
    //thread_init();
}

TCPClient::~TCPClient()
{
    close_sock();
    thread_on = false;
}

int TCPClient::initial_sock(QString &msg)
{
    msg = "";
#ifdef _WIN32
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver,&dat);
#endif //_WIN32
    if(_client != INVALID_SOCKET)
    {
        close_sock();
        msg = "旧链接已关闭";
        return -1;
    }
    _client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_client == INVALID_SOCKET)
    {
        msg = "初始化失败";
        return -1;
    }else
    {
        msg = "初始化成功";
    }
    qDebug()<<msg;

    return 0;
}

int TCPClient::connect_server(char* ip, unsigned short port, QString &msg)
{
    int result = 0;
    msg = "";
    if(_client == INVALID_SOCKET){
        QString rmsg;
        if(initial_sock(rmsg) == -1)
            msg = "连接失败，初始化失败;";
        msg += rmsg;
        qDebug()<<msg;
        result = -1;
    }else{
        _sin.sin_family = AF_INET;
        _sin.sin_port = htons(port);
    #ifdef _WIN32
        _sin.sin_addr.S_un.S_addr = inet_addr(ip);
    #else
        _sin.sin_addr.s_addr = inet_addr(ip);
    #endif //_WIN32
        int ret = ::connect(_client, (sockaddr*)&_sin, sizeof(sockaddr_in)); // ::  winsock2中connect与qobject中connect冲突问题
        if(ret == SOCKET_ERROR){
            msg = "连接服务器失败";
            qDebug()<<msg;
            result = -1;
        }
        else{
           msg = "成功连接服务器";
           qDebug()<<msg.toStdString().c_str();

        }
    }
    emit signalNetState(result + 1);
    return result;
}

int TCPClient::close_sock()
{
    if(_client != INVALID_SOCKET)
    {
#ifdef _WIN32
        closesocket(_client);
        WSACleanup();
#else
        close(_client);
#endif
        _client = INVALID_SOCKET;
    }
    return 0;
}

bool TCPClient::run_select(QString &msg)
{
    bool result = true;
    if(is_run())
    {
        fd_set fdRead;
        FD_ZERO(&fdRead);
        FD_SET(_client, &fdRead);
        timeval over_time = { 1,0 };
        int ret = select(_client, &fdRead, 0, 0,&over_time);
        if(ret < 0)
        {
            msg = "select任务结束";
            result = false;
        }else{
            if(FD_ISSET(_client, &fdRead))
            {
                FD_CLR(_client, &fdRead);
                QString rmsg;
                if(recv_data(rmsg) == -1)
                {
                    msg = "select接收失败;" + rmsg;
                    result = false;
                }
            }
        }
    }else{
        result = false;
    }
    return result;
}
int TCPClient::recv_data(QString &msg)
{
    memset(_msgBuff, 0 , RECV_BUFF_SIZE);
    memset(_recvBuff, 0 , RECV_BUFF_SIZE);

    int nlen = (int)recv(_client, _recvBuff, RECV_BUFF_SIZE, 0);
    if(nlen < 0)
    {
        msg = "Lost connection to server";
        emit signalNetState(0);
        qDebug()<<msg;
        connect_server("192.168.1.102", 8000, msg);
        return -1;
    }

    emit signalRecv(_recvBuff);
    memset(_msgBuff, 0 , RECV_BUFF_SIZE);
    memset(_recvBuff, 0 , RECV_BUFF_SIZE);
//    memcpy(_msgBuff, _recvBuff, nlen);
//    _lastpos += nlen;
//    while(_lastpos >= sizeof(DataHeader))
//    {

//        DataHeader* header = nullptr;
//        char bbb[sizeof (DataHeader)];
//        memcpy(bbb, _msgBuff, sizeof (DataHeader) );
//        header = (DataHeader*)bbb;
//        if(_lastpos >= header->datalength )//&& header->datalength != 0
//        {
//            int t = header->datalength;
//            int nsize = _lastpos - header->datalength;
//            memcpy(_msgBuff, _msgBuff + header->datalength, nsize);
//            _lastpos = nsize;
//        }
//        else
//        {
//            break;
//        }
//    }
    return 0;

}



int TCPClient::send_data(DataHeader *data)
{
    const char* d = (const char*)data;
    if(is_run())
    {
        return send(_client, d, data->datalength, 0);
    }
    return SOCKET_ERROR;
}

int TCPClient::sendData(char *data, int len)
{
    if(is_run())
    {
        return send(_client, data, len, 0);
    }
    return SOCKET_ERROR;
}


bool TCPClient::is_run()
{
    return _client != INVALID_SOCKET;
}





//void UDPClient::thread_init()
//{
//    thread_msg = new std::thread(&UDPClient::thread_data_fact,this);
//}


//void UDPClient::thread_data_fact()
//{
//    while(thread_on)
//    {
//        if(!data_buff.empty())
//        {
//            lock_guard<mutex> lo(_lock);
//            auto iter = data_buff.begin();
//            long long id = iter->first;
//            int size = data_buff_size.find(id)->second;
//            if(iter->second.size() == size)
//            {
//                char* buff = new char[size * 1024];

//                while(!iter->second.empty())
//                {
//                    Data_Buff d_buff = iter->second.front();
//                    iter->second.pop();

//                    memcpy(buff + (d_buff.data_count * 1024),d_buff.data,1024);

//                }
//            }

//            //emit signal_recv_msg(id,bd.data,CMD_BIG_MSG);
//        }
//        else
//        {
//            _thread_status.lock();
//        }
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//    }
//}


//void UDPClient::big_data_factory(long long recv_id,long long sour_id,char* data,int len, ENUM_CMD cmd_kind,ENUM_SEND_FORMAT format_kind)
//{
//    double d = ((double)len / BigData::get_data_maxsize());
//    int count = ceil(d);
//    char *datac = new char[len];
//    strncpy(datac,data,len);
//    long long _cmd_id = Tool_Qt::get_ctime_int();
//    for(int i = 0; i < count ; i++)
//    {
//        BigData bigdata = BigData(cmd_kind);
//        bigdata.data_count = count;
//        bigdata.dest_id = recv_id;
//        bigdata.sour_id = sour_id;
//        bigdata.format_kind = format_kind;
//        bigdata.cmd = cmd_kind;
//        bigdata.data_index = i;
//        bigdata.cmd_id = _cmd_id;
//        if(len > BigData::get_data_maxsize())
//        {
//            strncpy_s(bigdata.data, BigData::get_data_maxsize(), datac, BigData::get_data_maxsize());
//            send_data(&bigdata);
//            memcpy(datac , datac + BigData::get_data_maxsize(), BigData::get_data_maxsize());
//        }else
//        {
//            strncpy_s(bigdata.data, BigData::get_data_maxsize(), datac, strlen(datac));
//            send_data(&bigdata);
//            break;
//        }

//    }

//}
