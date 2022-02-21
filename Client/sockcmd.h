#ifndef SOCKCMD_H
#define SOCKCMD_H
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QQueue>
#include <QtMath>
#include <thread>
#include <mutex>
using namespace std::chrono;
namespace CMD
{
const int udpPackLen = 65000;

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
    /**
     * @brief 生成命令ID
     * @return
     */
    long static GetID(){
        auto c_time= duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        return c_time.count() % 1000000;
    }
    /**
     * @brief 毫秒级时间戳
     * @return
     */
    long static GetTime(){
        auto c_time= duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        return c_time.count();
    }
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
        QQueue<QByteArray> static sendBigStream(QByteArray data, ENUM_STREAM streamType = ENUM_STREAM::STREAM_VIDEO){

            QJsonObject json;
            QQueue<QByteArray> resultData;
//            QString strData = QString(data.toBase64());
            //QString strData = QString(data.toStdString().c_str());
            int dataSize = data.size();
            int num = qCeil( (double)dataSize / (double)udpPackLen);
            if(num == 1){
                QByteArray json = generateJson(data, streamType);
                resultData.push_back(json);
            }else if(num > 1){
                int packId = GetID();
                for(int i = 0; i < num; i++){
                    int resourceLen = (i == num - 1 ? dataSize%udpPackLen : udpPackLen);
                    QByteArray dataPack;
                    dataPack.resize(resourceLen);
                    memcpy_s(dataPack.data(), resourceLen, data.data() + (i * udpPackLen), resourceLen);
                    QByteArray json = generateJson(dataPack, streamType, num, i, packId);
                    resultData.push_back(json);
                }
            }
            return resultData;

        }
        /**
         * @brief generateJson 生成Json语句
         * @param data 数据包
         * @param streamType 数据包类型
         * @param num 数据分包数量
         * @param index 当前分包索引
         * @return
         */
        QByteArray static  generateJson(const QByteArray data,
                                        ENUM_STREAM streamType = ENUM_STREAM::STREAM_VIDEO,
                                        int num = 1, int index = 0 ,int cmdID = 0){

            QJsonObject json;
            json.insert("ENUM_STREAM",streamType);
            if(num != 1){
                json.insert("CMDID", cmdID);
                json.insert("Number", num);
                json.insert("Index", index);
            }
            json.insert("Size",data.size());
            json.insert("Data",QString(data));
            QJsonDocument doc;
            doc.setObject(json);
            return  doc.toJson(QJsonDocument::Compact);
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
        /**
         * @brief analyseCMD 命令解析
         * @param byteArray Json数据
         * @param type
         * @return
         */
        QJsonValue static analyseCMD(QByteArray byteArray, QString type){
            QJsonValue result;
            if(byteArray.size() > 0){
                QJsonParseError jsonError;
                QJsonDocument doucment = QJsonDocument::fromJson(byteArray, &jsonError);  // 转化为 JSON 文档
                if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {  // 解析未发生错误
                    QJsonObject object = doucment.object();  // 转化为对象
                    if (object.contains(type)) {  // 包含指定的 key
                        QJsonValue value = object.value(type);  // 获取指定 key 对应的 value
                        result = value;  // 将 value 转化为字符串
                    }
                }
                qDebug()<<jsonError.errorString();
            }
            return  result;
        }

        int static analyseID(QByteArray byteArray, QString type){
            int ID = 0;
            QJsonValue val = analyseCMD(byteArray, type);
            if(!val.isUndefined() && !val.isNull() && val.isDouble()){
                 ID =val.toInt();
            }
            return ID;
        }
    };

}

#endif // SOCKCMD_H
