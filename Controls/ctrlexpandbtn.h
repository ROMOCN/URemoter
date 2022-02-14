#ifndef CTRLEXPANDBTN_H
#define CTRLEXPANDBTN_H

#include <QPushButton>
#include <QWidget>
#include <QHBoxLayout>
class CtrlExpandBtn : public QWidget
{
    Q_OBJECT
    QHBoxLayout *layout = nullptr;
    QPushButton *btnMain = nullptr, *btnFold = nullptr;
    bool openState = false;
public:
    CtrlExpandBtn(QWidget *parent = nullptr);
    void initBtn();
    void initStyle();
    bool isOpen();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void setImage(QPixmap map);
    void setImage(QString path);
signals:
    void clicked(bool state);
    void signalOpenStateChanged(bool state);
private:
    void setState(bool state);
    bool state();


};

#endif // CTRLEXPANDBTN_H
