#ifndef CTRLLIVECENTRE_H
#define CTRLLIVECENTRE_H

#include <QWidget>
#include "ctrllivecontrol.h"
#include "Tools/toolvideo.h"
#include "Tools/toolaudio.h"
#include "Tools/toolnetstate.h"
#include "Tools/toolgrapwindow.h"
class CtrlLiveCentre : public QWidget
{
    Q_OBJECT
    CtrlLiveControl *control = nullptr;
    ToolVideo *video = nullptr;
    ToolAudio *audio = nullptr;
    ToolGrapWindow *window = nullptr;
    ToolNetState *net = nullptr;
public:
    explicit CtrlLiveCentre(QWidget *parent = nullptr);
    void recvAudio(QByteArray data);
signals:
    void signalPushImg(QImage img);
    void signalPushAudio(QByteArray data);
private slots:
    void slotVideoStateChanged(bool state);
    void slotAudioStateChanged(bool state);
    void slotShareStateChanged(bool state);
    void slotClose();
private:
    void initCtrl();
    void initSignal();
};

#endif // CTRLLIVECENTRE_H
