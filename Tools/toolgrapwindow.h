#ifndef TOOLGRAPWINDOW_H
#define TOOLGRAPWINDOW_H

#include <QObject>
#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
//#include "QtWinExtras/qwinfunctions.h"
#include <QtWin>
class ToolGrapWindow : public QObject
{
    Q_OBJECT
    QTimer *timer = nullptr;
public:
    explicit ToolGrapWindow(QObject *parent = nullptr);
    QPixmap static grapWithOutMouse(WId window = QApplication::desktop()->winId(), int x = 0, int y = 0, int width = -1, int height = -1);
    QPixmap static grapWithMouse(HWND winId= (HWND)QApplication::desktop()->winId(), int x = 0, int y = 0, int width = 1920, int height = 1080);
    void start(int interval = 30);
    void changeInterval(int interval);
    void stop();
    void running();
    bool isRunning(){
        return timer->isActive();
    }
signals:
    void signalPushSreen(QImage img);
};

#endif // TOOLGRAPWINDOW_H
