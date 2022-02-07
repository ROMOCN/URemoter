#include "videowidget.h"



VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent)
{
    init();
    setWindowFlags(windowFlags() | Qt::WindowStaysOnBottomHint);

}

VideoWidget::~VideoWidget()
{

}

void VideoWidget::init()
{
    labelRoom = new QLabel(this);
    labelRoom->move(0,0);
    panel = new VideoPanel(this);
    panel->move(0,0);
    panel->resize(this->size());
}

void VideoWidget::setRoomID(int roomID)
{
    this->labelRoom->setText(QString("%1").arg(roomID));
}

void VideoWidget::setVideo(QImage img, int index)
{
    panel->setVideo(img, index);
}

void VideoWidget::resizeEvent(QResizeEvent *event)
{
    panel->resize(this->size());
}
