#ifndef CTRLINFOWIDGET_H
#define CTRLINFOWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
class CtrlInfoWidget : public QWidget
{
    Q_OBJECT
    QSize maxSize = QSize(600, 400);
    QSize minGroupSize = QSize(maxSize.width()/2 - 10, maxSize.height()/2);
    QSize maxBtnSize = QSize(120, 40);


    QGroupBox *groupShow = nullptr, *groupInput = nullptr;
    QLabel *labFrameShow = nullptr, *labFrameInput = nullptr,
    *labSIDName = nullptr, *labIIDName = nullptr,
    *labSID = nullptr;
    QLineEdit *editIID = nullptr;;
    QPushButton *btnIJoin = nullptr, *btnSCreate = nullptr;
public:
    explicit CtrlInfoWidget(QWidget *parent = nullptr);
    void setRoomID(int roomID);

signals:
    void signalCreateRoom();
    void signalJoinRoom(int RoomID);
    void signalLeaveRoom();
    void signalCloseRoom();
private:
    void initCtrl();
    void initSignals();

};

#endif // CTRLINFOWIDGET_H
