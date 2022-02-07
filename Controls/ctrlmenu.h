#ifndef CTRLMENU_H
#define CTRLMENU_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QMouseEvent>
#include <QLabel>
#include <QGroupBox>
class CtrlMenu : public QWidget
{
    Q_OBJECT
    QGridLayout *layout = nullptr;
    QList<QPushButton *> btns;
    QWidget *stateWidget = nullptr, *btmWidget = nullptr;
    QWidget *back = nullptr;
    QPushButton* labNetState = nullptr;
public:
    const static int ctrlCount = 4;
    const static int minWidth = 70;
    const static int maxWidth = 160;
    enum ENUM_NETSTATE{
        STATE_UNCONNECT = 0,
        STATE_CONNECTED = 1,
    };

    explicit CtrlMenu(QWidget *parent = nullptr);
     ~CtrlMenu();
    void setNetState(int state);
signals:
private:
    void init();
    void initCtrl(int ctrlcount);
    void mouseMoveEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void leaveEvent(QEvent *event) override;

};

#endif // CTRLMENU_H
