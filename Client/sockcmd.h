#ifndef SOCKCMD_H
#define SOCKCMD_H
#include <QImage>
namespace CMD
{
    enum ENUM_CMD{
        CMD_START = 0x11,
        CMD_CONNECT,
        CMD_STOP,
    };

    class DataHeader{
    public:
        DataHeader(){
        }
        ~DataHeader(){

        }
        ENUM_CMD cmd;
        int datalength;
        int buffsize = 0;
        char *buff = nullptr;

    };
    class Data :public DataHeader{
    public:
        Data(){
            datalength = sizeof (Data);
            cmd = CMD_CONNECT;
        }


    };
}

#endif // SOCKCMD_H
