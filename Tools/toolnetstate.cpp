#include "toolnetstate.h"

ToolNetState::ToolNetState(QObject *parent) : QObject(parent)
{
    //通过访问指定网站测试网络连接
    QHostInfo::lookupHost(testNet, this, [=](QHostInfo host){
        if (host.error() != QHostInfo::NoError) {
            qDebug() << "Lookup failed:" << host.errorString();

            //网络未连接，发送信号通知
            emit signalNetState(false);
        } else {
            emit signalNetState(true);
        }
    });
    //QNetworkConfigurationManager实时监测到网络变化的信号
    connect(&mgr, &QNetworkConfigurationManager::onlineStateChanged, this, &ToolNetState::signalNetStateChange);
}

bool ToolNetState::isNetWorkOnline()
{
    QNetworkConfigurationManager mgr;
    return mgr.isOnline();
}

