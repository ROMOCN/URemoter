#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include "Client/udpclient.h"

#include <QBuffer>
#include <QTimer>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QWidget
{
    Q_OBJECT
    UDPClient *client = nullptr;
    QLabel *lab_video = nullptr;
    VideoCapture capture;
    QTimer *timer = nullptr;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void slotVideo(QImage img);
    QImage MatImageToQt(const Mat &src);
    Q_INVOKABLE  void start();
    void play();
    void init();
    static unsigned char* charToUc(char* read_buff)
    {
        return  reinterpret_cast<unsigned char*> (read_buff);
    }
    static char* ucToChar(unsigned char* u_read_buff)
    {
        return  reinterpret_cast<char*> (u_read_buff);
    }
    QByteArray imgToQByte(const QImage &image)
    {
        QByteArray imageData;
        QBuffer buffer(&imageData);
        image.save(&buffer, "jpg");
        imageData = imageData.toBase64();
        return imageData;
    }
    QImage qbyteToImg(const QString &data)
    {
        QByteArray imageData = QByteArray::fromBase64(data.toLatin1());
        QImage image;
        image.loadFromData(imageData);
        return image;
    }

private:
    Ui::MainWindow *ui;
    void resizeEvent(QResizeEvent *event) override;
};
#endif // MAINWINDOW_H
