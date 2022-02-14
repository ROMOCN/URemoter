#include "ctrlexpandbtn.h"

CtrlExpandBtn::CtrlExpandBtn(QWidget *parent) : QWidget(parent)
{
    this->setLayout(layout);
    initBtn();
    initStyle();
    layout = new QHBoxLayout(this);
    layout->addWidget(btnMain);
    layout->addWidget(btnFold);
    connect(btnMain, &QPushButton::clicked, this, [=](){
        setState(!state());
        emit this->clicked(openState);
    });
}

void CtrlExpandBtn::initBtn()
{
    btnMain = new QPushButton(this);
    btnFold = new QPushButton("V",this);
    btnMain->resize(40, 40);
    btnMain->setMinimumSize(30, 30);
    btnFold->setMaximumWidth(10);
    btnFold->setFixedHeight(btnMain->height());
}

void CtrlExpandBtn::initStyle()
{
    QString style = "QPushButton{background-color:transparent; border:none;}";
    btnFold->setStyleSheet(style);
    btnMain->setStyleSheet(style);

}

void CtrlExpandBtn::mousePressEvent(QMouseEvent *event)
{

}

void CtrlExpandBtn::mouseMoveEvent(QMouseEvent *event)
{

}

void CtrlExpandBtn::mouseReleaseEvent(QMouseEvent *event)
{

}

void CtrlExpandBtn::enterEvent(QEvent *event)
{

}

void CtrlExpandBtn::leaveEvent(QEvent *event)
{

}

void CtrlExpandBtn::setImage(QPixmap map)
{
    btnMain->setIcon(QIcon(map));
    btnMain->setIconSize(btnMain->size());
}

void CtrlExpandBtn::setImage(QString path)
{
    btnMain->setIcon(QIcon(path));
    //btnMain->setIconSize(pix.size());

}

void CtrlExpandBtn::setState(bool state)
{
    if(state != this->openState){
        emit this->signalOpenStateChanged(state);
    }
    openState = state;
}

bool CtrlExpandBtn::state()
{
    return openState;
}
