#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>

#include "videopanel.h"

class VideoWidget : public QWidget
{
    Q_OBJECT
    VideoPanel *panel = nullptr;
    QLabel *labelRoom = nullptr;
public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget();
    void init();
    void setRoomID(int roomID);
    void setVideo(QImage img, int index = 0);
signals:

private:
    void resizeEvent(QResizeEvent *event) override;

};

#endif // VIDEOWIDGET_H
