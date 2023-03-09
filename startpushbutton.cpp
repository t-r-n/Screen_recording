#include "startpushbutton.h"
#include<QDebug>
StartPushButton::StartPushButton(QWidget *parent) : QPushButton(parent)
{
    //this->setMouseTracking(true);
//    pixPa1.load(":/image/sttt.png");
//    pixPa1=pixPa1.scaled(size,size,Qt::KeepAspectRatio);
//    pixPa.load(":/image/sttt.png");
//    pixPa=pixPa.scaled(size+bsize,size+bsize,Qt::KeepAspectRatio);
    pix.load(":/image/sttt.png");
    qDebug()<<"pix size:"<<pix.size().width()<<pix.size().height()<<endl;
    pix=pix.scaled(size,size,Qt::KeepAspectRatio);
    this->setFixedSize((pix.width()),pix.height());
    this->setStyleSheet("QPushButton{border:10px;}");
    this->setIcon(pix);
    this->setIconSize(QSize(pix.width(),pix.height()));
    connect(this,&StartPushButton::clicked,this,&StartPushButton::chagePix);
    this->move(this->x()+10,this->y());
}
//void mousePressEvent(QMouseEvent *ev1){

//}
//void mouseMoveEvent(QMouseEvent *ev1){
//    if(ev1->)

//}
//进入窗口区域
void StartPushButton::enterEvent(QEvent *){

//        if(isst)pix=pix.scaled(110,110,Qt::KeepAspectRatio);
//        else pix=pix.scaled(110,110,Qt::KeepAspectRatio);
        pix=pix.scaled(size+bsize,size+bsize,Qt::KeepAspectRatio);
        this->setFixedSize((pix.width()),pix.height());
        this->setStyleSheet("QPushButton{border:100px;}");
        this->setIcon(pix);
        this->setIconSize(QSize(pix.width(),pix.height()));
        this->move(this->x()-(bsize/2),this->y()-(bsize/2));
        qDebug()<<this->x()<<"y:"<<this->y()<<endl;
}
//离开窗口区域
void StartPushButton::leaveEvent(QEvent *){
    //for(int i=0;i<10;++i){
    //if(isst){
        //if(isst)pix=pix.scaled(100,100,Qt::KeepAspectRatio);
        //else pix=pix.scaled(100,100,Qt::KeepAspectRatio);
        pix=pix.scaled(size,size,Qt::KeepAspectRatio);
        this->setFixedSize((pix.width()),pix.height());
        this->setStyleSheet("QPushButton{border:10px;}");
        this->setIcon(pix);
        this->setIconSize(QSize(pix.width(),pix.height()));
        this->move(this->x()+(bsize/2),this->y()+(bsize/2));
//    }else{
//        pix=pix.scaled(60,60,Qt::KeepAspectRatio);
//        //else pix=pix.scaled(60,60,Qt::KeepAspectRatio);
//        this->setFixedSize((pix.width()),pix.height());
//        this->setStyleSheet("QPushButton{border:10px;}");
//        this->setIcon(pix);
//        this->setIconSize(QSize(pix.width(),pix.height()));
//        this->move(this->x()+5,this->y()+5);
//    }
        qDebug()<<this->x()<<"y:"<<this->y()<<endl;
}
void StartPushButton::chagePix(){
    if(isst){
       pix.load(":/image/sspass.png");
       pix=pix.scaled(size,size,Qt::KeepAspectRatio);
       this->setIcon(pix);
       isst=false;
       qDebug()<<this->x()<<"y:"<<this->y()<<endl;
       this->move(this->x()-10,this->y());
    }else{
       pix.load(":/image/sttt.png");
       pix=pix.scaled(size,size,Qt::KeepAspectRatio);
       this->setIcon(pix);
       isst=true;
       this->move(this->x()+10,this->y());
       qDebug()<<this->x()<<"y:"<<this->y()<<endl;
    }
}

#if 0
//这里我使用另一个函数来完成判断鼠标是否在一个按钮区域内，如果在区域内只返回真，否则返回假
bool MainWindow::enterBtn(QPoint pp, QPushButton *btn)
{
   int height = btn->height();
   int width = btn->width();
   QPoint btnMinPos = btn->pos();
   QPoint btnMaxPos = btn->pos();
   btnMaxPos.setX(btn->pos().x()+width);
   btnMaxPos.setY(btn->pos().y()+height);
   if(pp.x() >= btnMinPos.x() && pp.y() >= btnMinPos.y()
       && pp.x() <= btnMaxPos.x() && pp.y() <= btnMaxPos.y())
       return true;
   else
       return false;
}
#endif
