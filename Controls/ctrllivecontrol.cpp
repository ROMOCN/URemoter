#include "ctrllivecontrol.h"

CtrlLiveControl::CtrlLiveControl(QWidget *parent) : QWidget(parent)
{
    initCtrl();
    initSignal();
}

void CtrlLiveControl::initCtrl()
{
    this->resize(400, 80);
    this->setMaximumHeight(80);
    this->setMinimumHeight(80);
    btnAudio = new CtrlExpandBtn(this);
    btnShare = new CtrlExpandBtn(this);
    btnVideo = new CtrlExpandBtn(this);
    btnClose = new CtrlExpandBtn(this);
    btnAudio->setImage(QPixmap(":/resource/image/imgAudio.png"));
    btnShare->setImage(QPixmap(":/resource/image/imgShare.png"));
    btnVideo->setImage(QPixmap(":/resource/image/imgVideo.png"));
    ctrlPosition();
    initBtnStyle(btnAudio);
    initBtnStyle(btnVideo);
    initBtnStyle(btnShare);
    initBtnStyle(btnClose);

}

void CtrlLiveControl::initSignal()
{
    connect(btnAudio, &CtrlExpandBtn::clicked, this, &CtrlLiveControl::signalAudioStateChanged);
    connect(btnVideo, &CtrlExpandBtn::clicked, this, &CtrlLiveControl::signalVideoStateChanged);
    connect(btnShare, &CtrlExpandBtn::clicked, this, &CtrlLiveControl::signalShareStateChanged);
    connect(btnClose, &CtrlExpandBtn::clicked, this, &CtrlLiveControl::signalClose);
}

void CtrlLiveControl::initBtnStyle(CtrlExpandBtn *btn)
{
    btn->resize(60, 90);
    //btn->setStyleSheet("");
}

void CtrlLiveControl::ctrlPosition()
{
    btnAudio->move(60,4);
    btnVideo->move(btnAudio->x() + btnAudio->width() + 20, btnAudio->y());
    btnShare->move(btnVideo->x() + btnVideo->width() + 20, btnVideo->y());
    btnClose->move(this->width() - btnClose->width() - 60, btnShare->y());

}

void CtrlLiveControl::resizeEvent(QResizeEvent *event)
{
    ctrlPosition();
}
