#include "ctrlmenu.h"

CtrlMenu::CtrlMenu(QWidget *parent) : QWidget(parent)
{
    //样式表不起作用，解决办法
    this->setAttribute(Qt::WA_StyledBackground, true);
    //根据背景色自动计算合适的前景色
//    double gray = (0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue()) / 255;
//    QColor textColor = gray > 0.5 ? Qt::black : Qt::white;
    init();
}

CtrlMenu::~CtrlMenu()
{

}

void CtrlMenu::setNetState(int state)
{
    switch (state) {
    case STATE_UNCONNECT:
    {
        labNetState->setStyleSheet("QPushButton{background-color:red;   border: none;}");
    }break;
    case STATE_CONNECTED:
    {
        labNetState->setStyleSheet("QPushButton{background-color:green; border: none;}");
    }break;

    }
    labNetState->update();
}

void CtrlMenu::init()
{
    this->setMinimumWidth(this->minWidth);
    this->setMaximumWidth(this->maxWidth);
    this->setStyleSheet(".QWidget{background-color:gray;}");
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->setMouseTracking(true);
    back = new QWidget(this);
    back->resize(this->size());
    back->installEventFilter(this);
    layout = new QGridLayout(back);
    stateWidget = new QWidget(this);
    labNetState = new QPushButton(stateWidget);
    labNetState->resize(14,14);


    btmWidget = new QWidget(this);
    //stateWidget->setMinimumHeight(100);
    initCtrl(this->ctrlCount);
    labNetState->move(0,  stateWidget->height() - labNetState->height());


}

void CtrlMenu::initCtrl(int ctrlcount)
{
    layout->addWidget(btmWidget ,0 ,0 ,2 ,1 );
    for(int i = 0;i < ctrlcount; i++){
        QPushButton *btn = new QPushButton(this);
        btn->setFixedHeight(40);
        layout->addWidget(btn, i + 1, 0, 1, 1);
        btns.append(btn);
    }
    layout->addWidget(stateWidget ,ctrlcount + 1,0 ,ctrlcount - 1 ,1);
}

void CtrlMenu::mouseMoveEvent(QMouseEvent *event)
{
    this->setFixedWidth(maxWidth);
}

void CtrlMenu::enterEvent(QEvent *event)
{
    this->setFixedWidth(maxWidth);
}

void CtrlMenu::resizeEvent(QResizeEvent *event)
{
    back->resize(this->size());
    labNetState->move(0,  stateWidget->height() - labNetState->height());
}

void CtrlMenu::leaveEvent(QEvent *event)
{
    this->setFixedWidth(minWidth);
}
