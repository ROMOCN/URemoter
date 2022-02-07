#include "ctrlinfowidget.h"

CtrlInfoWidget::CtrlInfoWidget(QWidget *parent) : QWidget(parent)
{
    this->resize(maxSize);
    //this->setStyleSheet("QWidget{background-color: gray;}");
    initCtrl();
    initSignals();
}

void CtrlInfoWidget::setRoomID(int roomID)
{
    this->labSID->setText(QString("%1").arg(roomID));
}

void CtrlInfoWidget::initCtrl()
{
    QFont fontBig("微软雅黑", 20, 10);
    QFont fontSmall("微软雅黑", 12, 6);

    groupShow = new QGroupBox(this);
    groupInput = new QGroupBox(this);
    labFrameShow = new QLabel("创建房间" ,groupShow);
    labFrameInput = new QLabel("加入房间" ,groupInput);
    labSIDName = new QLabel("本机识别码" ,groupShow);
    labIIDName = new QLabel("房间识别码" ,groupInput);
    labSID = new QLabel("" ,groupShow);
    editIID = new QLineEdit(groupInput);
    btnSCreate = new QPushButton("创建房间", groupShow);
    btnIJoin = new QPushButton("加入房间", groupInput);
    groupShow->resize(maxSize.width()/2 ,maxSize.height());
    groupInput->resize(maxSize.width()/2 ,maxSize.height());
    groupShow->setMinimumSize(minGroupSize);
    groupInput->setMinimumSize(minGroupSize);
    groupShow->move(0,0);
    groupInput->move(groupShow->width() + groupShow->x(), 0);
    labFrameShow->move(10 ,0);
    labFrameShow->setFont(fontBig);
    labFrameShow->setAlignment(Qt::AlignCenter);
    labFrameInput->move(10 ,0);
    labFrameInput->setFont(fontBig);
    labFrameInput->setAlignment(Qt::AlignCenter);
    labSIDName->move(10 ,labSIDName->height() + 50);
    labSIDName->setFont(fontSmall);
    labIIDName->move(10 ,labFrameInput->height() + 50);
    labIIDName->setFont(fontSmall);
    labSID->move(10 ,labSIDName->y() + labSIDName->height() + 30);
    labSID->setFont(fontBig);
    editIID->move(10 ,labIIDName->y() + labIIDName->height() + 30);
    editIID->setFont(fontBig);
    editIID->setFixedWidth(groupInput->width() - 20 );
    btnSCreate->setMaximumSize(maxBtnSize);
    btnSCreate->move( (groupShow->width() - btnSCreate->width())/2 , (groupShow->height() - btnSCreate->height() - 20)  );
    btnIJoin->move( (groupInput->width() - btnIJoin->width())/2 , (groupInput->height() - btnIJoin->height() - 20)  );
    btnIJoin->setMaximumSize(maxBtnSize);

}

void CtrlInfoWidget::initSignals()
{
    connect(btnSCreate, &QPushButton::clicked, this, &CtrlInfoWidget::signalCreateRoom);
    connect(btnIJoin, &QPushButton::clicked, [=](){
        if(editIID->text().contains(QRegExp("^\\d+$"))){
            emit signalJoinRoom(editIID->text().toInt());
        }
    });
}
