#include "ctrllivecentre.h"

CtrlLiveCentre::CtrlLiveCentre(QWidget *parent) : QWidget(parent)
{
    initCtrl();
    initSignal();
}

void CtrlLiveCentre::recvAudio(QByteArray data)
{
    audio->soltPlay(data);
}

void CtrlLiveCentre::initCtrl()
{
    this->resize(400, 80);
    net = new ToolNetState(this);
    video = new ToolVideo(this);
    audio = new ToolAudio(this);
    window = new ToolGrapWindow(this);
    control = new CtrlLiveControl(this);
}

void CtrlLiveCentre::initSignal()
{
    connect(net, &ToolNetState::signalNetStateChange, this, [=](bool state){
        qDebug()<<"Net State:"<<state;
    });
    connect(net, &ToolNetState::signalNetState, this, [=](bool state){
        qDebug()<<"Baidu State"<<state;
    });

    connect(video, &ToolVideo::pullImage, this, [=](QImage img){
        if(!window->isRunning()){
            emit signalPushImg(img);
        }
    });
    connect(window, &ToolGrapWindow::signalPushSreen, this, [=](QImage img){
        if(window->isRunning()){
            emit signalPushImg(img);
        }
    });
    connect(audio, &ToolAudio::signalPushSound, this ,&CtrlLiveCentre::signalPushAudio);
    connect(control, &CtrlLiveControl::signalAudioStateChanged, this, &CtrlLiveCentre::slotAudioStateChanged);
    connect(control, &CtrlLiveControl::signalVideoStateChanged, this, &CtrlLiveCentre::slotVideoStateChanged);
    connect(control, &CtrlLiveControl::signalShareStateChanged, this, &CtrlLiveCentre::slotShareStateChanged);
    connect(control, &CtrlLiveControl::signalClose, this, &CtrlLiveCentre::slotClose);
}

void CtrlLiveCentre::slotVideoStateChanged(bool state)
{
    if(state && !video->isRunning()){
        video->start();
    }else{
        video->stop();
    }
}

void CtrlLiveCentre::slotAudioStateChanged(bool state)
{
    if(state){
        audio->startRecord();
    }else{
        audio->stopRecord();
    }
}

void CtrlLiveCentre::slotShareStateChanged(bool state)
{
    if(state){
        window->start();
    }else{
        window->stop();
    }
}

void CtrlLiveCentre::slotClose()
{

}
