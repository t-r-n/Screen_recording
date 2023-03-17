#include "stlabel.h"

stlabel::stlabel(QString st,QString en,QWidget *parent) : QLabel(parent){//那这不是子类指针指向父指针吗，支持吗

    stpath=st;
    enpath=en;
    pixpath=stpath;
    InitSvgWidget(pixpath,orsize,0);

}
void stlabel::InitSvgWidget(QString path,float __size,int flags)//0不移动 1放大 2缩小
{
    QSvgRenderer* m_svgRender = new QSvgRenderer;
    m_svgRender->load(path);
    QSize QpreSize=this->size();
    QSize size = m_svgRender->defaultSize(); //获取svg的大小
    QPixmap pix(size*__size);
    QSize Size=size*__size;
    //if(flags)pix.scaled(_size,_size,Qt::KeepAspectRatio);        // 在这给绘图设备重新设置大小
    //else pix.scaled(_size+bsize,_size+bsize,Qt::KeepAspectRatio);

    pix.fill(Qt::transparent);        // 像素清空,这一步必须有,否则背景有黑框

    QPainter painter(&pix);           // 画家和绘图设备建立联系
    painter.setRenderHints(QPainter::Antialiasing);     //反锯齿绘制
    m_svgRender->render(&painter);    //执行渲染操作
    setFixedSize(pix.width(),pix.height());
    if(flags==1)this->move(this->x()-((Size.width()-QpreSize.width())/2),this->y()-((Size.height()-QpreSize.height())/2));
    else if(flags==2)this->move(this->x()+((QpreSize.width()-Size.width())/2),this->y()+((QpreSize.height()-Size.height())/2));
    setPixmap(pix);   //将pix加载到QLabel上
    setAlignment(Qt::AlignCenter);  //设置居中显示
}
//进入窗口区域
void stlabel::enterEvent(QEvent *){
//        this->setFixedSize((pix1.width()),pix1.height());
//        this->setPixmap(pix1);
//        this->move(this->x()-(bsize/2),this->y()-(bsize/2));
        qDebug()<<this->x()<<"y:"<<this->y()<<endl;
        InitSvgWidget(pixpath,bisize,1);
}
//离开窗口区域
void stlabel::leaveEvent(QEvent *){
//        this->setFixedSize((pix.width()),pix.height());
//        this->setPixmap(pix);
//        this->move(this->x()+(bsize/2),this->y()+(bsize/2));
        qDebug()<<this->x()<<"y:"<<this->y()<<endl;

    InitSvgWidget(pixpath,orsize,2);
}
void stlabel::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
    if(ev->button() == Qt::LeftButton)
    {
        chagePix();
        emit clicked();	// 发射信号
    }
}
void stlabel::chagePix(){
    if(isst){
       isst=false;
       pixpath=enpath;
       InitSvgWidget(pixpath,orsize,2);
    }else{
       pixpath=stpath;
       InitSvgWidget(pixpath,orsize,2);
       isst=true;

    }
}
void stlabel::changeStpath(QString s){
    stpath=s;
}
void stlabel::changeEnpath(QString s)
{
    enpath=s;
}
void stlabel::changeSize(float orz,float bi){

    orsize=orz;
    bisize=bi;
    InitSvgWidget(pixpath,orsize,0);//重画空间
}
