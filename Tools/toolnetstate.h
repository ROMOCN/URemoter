#ifndef TOOLNETSTATE_H
#define TOOLNETSTATE_H

#include <QObject>
#include <QNetworkConfigurationManager>
#include <QHostInfo>
class ToolNetState : public QObject
{
    Q_OBJECT
    QNetworkConfigurationManager mgr;
    const QString testNet = "www.baidu.com";
public:
    explicit ToolNetState(QObject *parent = nullptr);
    /**
     * @brief isNetWorkOnline
     * 只需要判断网络是否有连接，不一定能上网
     */
    bool static isNetWorkOnline();
signals:
    /**
     * @brief signalNetState
     * 通过访问指定网站测试网络连接
     */
    void signalNetState(bool);
    /**
     * @brief signalNetStateChange
     * 实时监测网络变化的信号
     */
    void signalNetStateChange(bool);

};

#endif // TOOLNETSTATE_H
