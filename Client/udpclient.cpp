#include "udpclient.h"

UDPClient::UDPClient(QObject *parent) : QObject(parent)
{
    _client = INVALID_SOCKET;
    _recvBuff[RECV_BUFF_SIZE] = {};
    _msgBuff[RECV_BUFF_SIZE * 10] = {};
    _lastpos = 0;
    //qRegisterMetaType<QByteArray>("QByteArray");

}

UDPClient::~UDPClient()
{
    close_sock();
}

int UDPClient::initial_sock(QString &msg)
{
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
    _client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(_client == INVALID_SOCKET)
    {
        msg = "初始化失败";
        return -1;
    }else
    {
        msg = "初始化成功";
    }
    qDebug()<<"udpClient"<<msg;

    return 0;
}

int UDPClient::bindPort(const char* ip, unsigned short port)
{
    QString errormsg;
    _addrServer.sin_family = AF_INET;
    _addrServer.sin_port = htons(port);//host to net unsigned short
#ifdef _WIN32
    if (ip)
    {
        _addrServer.sin_addr.S_un.S_addr = inet_addr(ip);
    }
    else
    {
        _addrServer.sin_addr.S_un.S_addr = INADDR_ANY;
    }
#else
    if (ip)
    {
        _sin.sin_addr.s_addr = inet_addr(ip);
    }
    else
    {
        _sin.sin_addr.s_addr = INADDR_ANY;
    }
#endif
    int ret = bind(_client, (sockaddr*)&_addrServer, sizeof(_addrServer));
    if (SOCKET_ERROR == ret){
        errormsg = "绑定失败...\n";
    }
    else{
        errormsg = "绑定成功.\n";
    }
    qDebug()<<"udpClient"<<errormsg;
    return ret;
}


int UDPClient::close_sock()
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

bool UDPClient::run_select(QString &msg)
{
    msg = "";
    if(is_run()){
        fd_set fdRead;
        FD_ZERO(&fdRead);
        FD_SET(_client, &fdRead);
        timeval over_time = { 1,0 };
        int ret = select(_client, &fdRead, 0, 0,&over_time);
        if(ret < 0)
        {
            msg = "select任务结束";
            return false;
        }
        if(FD_ISSET(_client, &fdRead))
        {
            FD_CLR(_client, &fdRead);
            QString rmsg;
            if(recv_data(rmsg) == -1)
            {
                msg = "select接收失败;" + rmsg;
                return false;
            }
        }
        return true;
    }
    return false;
}
int UDPClient::recv_data(QString &msg)
{
    //int nlen = (int)recv(_client, _recvBuff, RECV_BUFF_SIZE, 0);
    int addlen = sizeof(sockaddr_in);
    int reasult = 0;
#ifdef _WIN32
    int nlen = recvfrom(_client,_recvBuff, RECV_BUFF_SIZE, 0, (sockaddr*)&_sin, &addlen);
#else
    int nlen = recvfrom(_client,_recvBuff, RECV_BUFF_SIZE, 0, (sockaddr*)&_sin, (socklen_t*)&addlen);
#endif
    if(nlen < 0)
    {
        msg = "与服务器连接中断";
        qDebug()<<"udpClient"<<msg;
        reasult = -1;
    }else if(nlen ==0){

    }else
    {
        emit signalRecv(_recvBuff);
       // memset(_msgBuff, 0 , RECV_BUFF_SIZE);
        memset(_recvBuff, 0 , RECV_BUFF_SIZE);

//        QByteArray baseData = QByteArray::fromBase64(strData.toLatin1());
//        QImage image;
//        if(image.loadFromData(baseData))
//            emit signalAudio(image);
    }
    return reasult;

}


int UDPClient::sendData(char *ip, int nPort, char *pData, int size)
{
    if(is_run()){
        sockaddr_in ser;	// 服务器端地址
        ser.sin_family = AF_INET;	// IP协议
        ser.sin_port = htons(nPort);	// 端口号
        ser.sin_addr.s_addr = inet_addr(ip);	// IP地址
        int nLen = sizeof(ser);	// 服务器地址长度

        //QByteArray byte(pData);
    //     //字节数组 要进行传输必须先转换成这个格式
    //     QBuffer buff(&byte);
    //     // 建立一个用于IO读写的缓冲区
    //     image.save(&buff,"JPEG");
         // image先向下转为byte的类型，再存入buff
    //     QByteArray compressByte = qCompress(pData,1);
    //     //数据压缩算法
    //     QByteArray base64Byte = compressByte.toBase64();

        //合并包头包体
    //     DataHeader *header = new DataHeader();
    //     header->datalength = size;
    //     header->cmd = CMD_CREATROOM;
    //     char pack[sizeof (DataHeader) + size];
    //     memcpy(pack, (char*)header, sizeof (DataHeader));
    //     memcpy(pack + sizeof (DataHeader), pData, size);
        return sendto(_client, (const char*)pData, size, 0, (sockaddr*)&ser, nLen);	// 向服务器发送数据
    }
}

int UDPClient::sendData(char *ip, int nPort, DataHeader *data)
{
    const char* d = (const char*)data;
    sockaddr_in ser;	// 服务器端地址
    ser.sin_family = AF_INET;	// IP协议
    ser.sin_port = htons(nPort);	// 端口号
    ser.sin_addr.s_addr = inet_addr(ip);	// IP地址
    int nLen = sizeof(ser);	// 服务器地址长度
    if(is_run())
    {
        return sendto(_client, d, data->datalength, 0, (sockaddr*)&ser, nLen);	// 向服务器发送数据
    }
    return SOCKET_ERROR;
}


bool UDPClient::is_run()
{
    return _client != INVALID_SOCKET;
}

int UDPClient::dataFactory()
{
    while(true){
        if(is_run() && _lastpos){
            std::lock_guard<std::mutex> l(_lock);

            //        memcpy(_msgBuff, _recvBuff, nlen);
            //        _lastpos += nlen;
            while(_lastpos >= sizeof(DataHeader))
            {
                DataHeader* header = nullptr;
                char headerBuff[sizeof (DataHeader)];
                memcpy(headerBuff, _msgBuff, sizeof (DataHeader) );
                header = (DataHeader*)headerBuff;
                memcpy(_msgBuff, _msgBuff + sizeof (DataHeader),   header->datalength);
                _lastpos -= sizeof (DataHeader);
                if(_lastpos >= header->datalength )//&& header->datalength != 0
                {
                    int datalen = header->datalength;
                    int nsize = _lastpos - header->datalength;
                    {
                        char dataBuff[datalen];
                        memcpy(dataBuff, _msgBuff , datalen);
                        emit signalRecv(dataBuff);
                    }
                    memcpy(_msgBuff, _msgBuff + header->datalength, nsize);
                    _lastpos = nsize;
                }
                else
                {
                    break;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));

    }
}
