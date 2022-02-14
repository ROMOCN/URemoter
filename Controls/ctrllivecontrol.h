#ifndef CTRLLIVECONTROL_H
#define CTRLLIVECONTROL_H

#include <QWidget>
#include <QPushButton>
#include "Controls/ctrlexpandbtn.h"
class CtrlLiveControl : public QWidget
{
    Q_OBJECT
    CtrlExpandBtn *btnVideo = nullptr, *btnShare = nullptr, *btnAudio = nullptr, *btnClose = nullptr;
public:
    explicit CtrlLiveControl(QWidget *parent = nullptr);

signals:
    void signalAudioStateChanged(bool state);
    void signalVideoStateChanged(bool state);
    void signalShareStateChanged(bool state);
    void signalClose();
private:
    void initCtrl();
    void initSignal();
    void initBtnStyle(CtrlExpandBtn *btn);
    void ctrlPosition();
    void resizeEvent(QResizeEvent *event) override;
};

#endif // CTRLLIVECONTROL_H
