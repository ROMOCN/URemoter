#ifndef SOCKCMD_H
#define SOCKCMD_H
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
namespace CMD
{
enum ENUM_CMD: short{
    CMD_NULL = 0,
    CMD_GETUID = 18,
    CMD_SENDUID,
    CMD_CREATROOM,
    CMD_JOINROOM,
    CMD_LEAVEROOM,
    CMD_CLOSEROOM,
};
    enum ENUM_STREAM :short{
        STREAM_VIDEO = 101,
        STREAM_AUDIO = 102,
        STREAM_FILE = 103,
    };

    class DataHeader{
    public:
        DataHeader(){

        }
        ~DataHeader(){

        }
        ENUM_CMD cmd;
        int datalength;
    };
    class Data: public DataHeader{
    public:
        Data(){
            datalength = sizeof(Data);
            cmd = CMD_NULL;
        }
        ~Data(){

        }
    };
    class JsonTool{
    public:
        QByteArray static sendUID(int UID){
            QJsonObject json;
            json.insert("ENUM_CMD",ENUM_CMD::CMD_GETUID);
            json.insert("UID", UID);
            QJsonDocument doc;
            doc.setObject(json);
            return doc.toJson(QJsonDocument::Compact);
        }
        QByteArray static getUID(){
            QJsonObject json;
            json.insert("ENUM_CMD",ENUM_CMD::CMD_GETUID);
            QJsonDocument doc;
            doc.setObject(json);
            return doc.toJson(QJsonDocument::Compact);
        }
        QByteArray static creatRoom(int UID){
            QJsonObject json;
            json.insert("UID",UID);
            json.insert("ENUM_CMD",ENUM_CMD::CMD_CREATROOM);
            QJsonDocument doc;
            doc.setObject(json);
            return doc.toJson(QJsonDocument::Compact);
        }
        QByteArray static joinRoom(int RoomID){
            QJsonObject json;
            json.insert("RoomID",RoomID);
            json.insert("ENUM_CMD",ENUM_CMD::CMD_JOINROOM);
            QJsonDocument doc;
            doc.setObject(json);
            return doc.toJson(QJsonDocument::Compact);
        }
        QByteArray static leaveRoom(){
            QJsonObject json;
            json.insert("ENUM_CMD",ENUM_CMD::CMD_LEAVEROOM);
            QJsonDocument doc;
            doc.setObject(json);
            return doc.toJson(QJsonDocument::Compact);
        }
        QByteArray static sendStream(const QByteArray data, ENUM_STREAM streamType = ENUM_STREAM::STREAM_VIDEO){
            QJsonObject json;

//            QString strData = QString(data.toBase64());
            //QString strData = QString(data.toStdString().c_str());

            json.insert("Data",QString(data));
            json.insert("Size",data.size());
            json.insert("ENUM_STREAM",streamType);
            QJsonDocument doc;
            doc.setObject(json);
            return doc.toJson(QJsonDocument::Compact);

        }


        QByteArray static analyseData(QByteArray byteArray, QString type, ENUM_STREAM &enumStream){
            QJsonParseError jsonError;
            QByteArray baseData = NULL;
            QJsonDocument doucment = QJsonDocument::fromJson(byteArray, &jsonError);  // 转化为 JSON 文档
            if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {  // 解析未发生错误
                QJsonObject object = doucment.object();  // 转化为对象
                if (object.contains(type)) {  // 包含指定的 key
                    QJsonValue value = object.value(type);  // 获取指定 key 对应的 value
                    if (value.isString()) {  // 判断 value 是否为字符串
                        QString strName = value.toString();  // 将 value 转化为字符串
                        baseData = strName.toLatin1();
                    }
                }

                if (object.contains("ENUM_STREAM")) {  // 包含指定的 key
                    QJsonValue value = object.value("ENUM_STREAM");  // 获取指定 key 对应的 value
                    if (value.isDouble()) {  // 判断 value 是否为字符串
                        int retEnum = value.toDouble();  // 将 value 转化为字符串
                        enumStream = (ENUM_STREAM)retEnum;
                    }
                }
            }
            qDebug()<<"jsonError:"<<jsonError.errorString();
            return baseData;
        }

        int static analyseID(QByteArray byteArray, QString type){
            QJsonParseError jsonError;
            int ID = 0;
            QJsonDocument doucment = QJsonDocument::fromJson(byteArray, &jsonError);  // 转化为 JSON 文档
            if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {  // 解析未发生错误
                QJsonObject object = doucment.object();  // 转化为对象
                if (object.contains(type)) {  // 包含指定的 key
                    QJsonValue value = object.value(type);  // 获取指定 key 对应的 value
                    if (value.isDouble()) {  // 判断 value 是否为字符串
                        ID = value.toDouble();  // 将 value 转化为字符串
                    }
                }
            }
            qDebug()<<jsonError.errorString();
            return ID;
        }
    };

}

#endif // SOCKCMD_H
