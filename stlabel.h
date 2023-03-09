#ifndef STLABEL_H
#define STLABEL_H

#include <QWidget>
#include<QLabel>
#include<QMouseEvent>
#include<QPixmap>
#include<QDebug>
#include<QMouseEvent>
#include<QSvgRenderer>
#include<QPainter>
class stlabel : public QLabel
{
    Q_OBJECT
public:
    explicit stlabel(QString st,QString en,QWidget *parent = nullptr);
    //进入窗口区域
    void enterEvent(QEvent *);
    //离开窗口区域
    void leaveEvent(QEvent *);
    virtual void mouseReleaseEvent(QMouseEvent * ev);  // 声明鼠标左键点击事件
    void chagePix();
    void setPix(QString st,QString en);
    void InitSvgWidget(QString path,float __size,int flags);
    void changeStpath(QString s);
    void changeEnpath(QString s);
    void changeSize(float orz=0.1,float bi=0.11);
signals:
    void clicked(void); // 声明鼠标左击中信号


private:
    QPixmap pix;
    QPixmap pix1;
    QString stpath;
    QString enpath;
    QString pixpath;
    float orsize=0.1;
    float bisize=0.11;
    int _size=30;
    int bsize=6;
    bool isst=true;
};
/*
        label_2 = new stlabel(":/image/sttt.svg",":/image/sspass.svg",start);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 10, 41, 41));
        label_2->setTextFormat(Qt::AutoText);
        label_2->setScaledContents(true);
        label_3 = new stlabel(":/image/end.svg",":/image/end.svg",start);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(90, 20, 51, 16));
        label_4 = new stlabel(":/image/small.svg",":/image/small.svg",start);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(150, 110, 72, 15));
        label_5 = new stlabel(":/image/exit.svg",":/image/exit.svg",start);

        label_2 = new stlabel(":/image/sttt.svg",":/image/sspass.svg",start);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 10, 41, 41));
        label_2->setTextFormat(Qt::AutoText);
        label_2->setScaledContents(true);
        label_3 = new stlabel(":/image/end.svg",":/image/end.svg",start);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(90, 20, 51, 16));
        label_4 = new stlabel(":/image/small.svg",":/image/small.svg",start);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(310, 10, 72, 15));
        label_5 = new stlabel(":/image/exit.svg",":/image/exit.svg",start);
*/
//stlabel::orsize=0.1;
#endif // STLABEL_H
