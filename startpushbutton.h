#ifndef STARTPUSHBUTTON_H
#define STARTPUSHBUTTON_H

#include <QWidget>
#include<QPushButton>
#include <QMouseEvent>
#include<QPixmap>

class StartPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit StartPushButton(QWidget *parent = nullptr);
    //进入窗口区域
    void enterEvent(QEvent *);
    //离开窗口区域
    void leaveEvent(QEvent *);
    void chagePix();
signals:




private:
    int size=50;
    int bsize=6;
    bool isst=true;
    bool firstEnter=true;
    QPixmap pix;
    QPixmap pixPa;
    QPixmap pixPa1;
};

#endif // STARTPUSHBUTTON_H
