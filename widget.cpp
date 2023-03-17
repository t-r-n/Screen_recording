#include "widget.h"
#include "ui_widget.h"
#include "windows.h"
#include<QTimer>
#include<QColorDialog>
#include<QClipboard>
#if _MSC_VER >= 1600	// MSVC2015 > 1899,	MSVC_VER = 14.0
#pragma execution_character_set("utf-8")
#endif

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
//    //设置无边界窗口并置顶
//    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
//    setMouseTracking(true);

//    //窗口设置透明
//    setAttribute(Qt::WA_TranslucentBackground,true);
//    //setFixedSize(1280,800);
    //设置无边界窗口并置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setMouseTracking(true);
    //setAttribute(Qt::WA_TransparentForMouseEvents,false);
    ui->pushButton_2->hide();
    //st->hide();
//    //窗口设置透明
//    setAttribute(Qt::WA_TranslucentBackground,true);

    QPalette pal(this->palette());

    pal.setColor(QPalette::Background, Qt::black); //设置背景黑色
    this->setAutoFillBackground(true);
    this->setPalette(pal);


    this->showFullScreen();
    setWindowOpacity(0.6);
    std::vector<QString>qve= DxgiManager::selectAdapt();
    for(size_t i=0;i<qve.size();++i){
        ui->comboBox->addItem(qve[i]);
    }
    ui->comboBox_2->addItem(QString("30fps"));
    ui->comboBox_2->addItem(QString("60fps"));
    ui->comboBox_2->addItem(QString("90fps"));
    ui->comboBox_3->addItem(QString("全屏"));
    ui->comboBox_3->addItem(QString("选定区域"));
    ui->comboBox_4->addItem(QString("超清"));
    ui->comboBox_4->addItem(QString("标清"));
    ui->comboBox_4->addItem(QString("普通"));
    //bar=new QMenuBar(this);//菜单栏创建函数,鼠标放在函数上发现返回值类型是QMenuBar*,所以应该初始化一个此类型的指针接受这个函数创建的菜单栏
    //现在创建了这个栏,要把她放在MainWindow窗口上；//记得声明QMenuBar这个类的头文件

    QPixmap pix;
    pix.load(":/image/fff.png");
    pix=pix.scaled(60,40,Qt::KeepAspectRatio);
    ui->pushButton_4->setFixedSize((pix.width()),pix.height());
    ui->pushButton_4->setStyleSheet("QPushButton{border:0px;}");
    //this->setIcon(pix);
    ui->pushButton_4->setIcon(pix);

    ui->pushButton_4->setIconSize(QSize(pix.width(),pix.height()));
    this->setFocusPolicy(Qt::NoFocus);//使按钮被点击时无虚线框

    pix.load(":/image/true.png");
    pix=pix.scaled(60,40,Qt::KeepAspectRatio);
    ui->pushButton_2->setFixedSize((pix.width()),pix.height());
    ui->pushButton_2->setStyleSheet("QPushButton{border:0px;}");
    //this->setIcon(pix);
    ui->pushButton_2->setIcon(pix);
    ui->pushButton_2->setIconSize(QSize(pix.width(),pix.height()));
    ui->pushButton_2->move(300,300);
    this->setFocusPolicy(Qt::NoFocus);//使按钮被点击时无虚线框





    auto al=QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    ui->comboBox_5->addItem("不录制音频");
    for(auto a:al){
        ui->comboBox_5->addItem(a.deviceName());
        qDebug()<<"音频输入设备："<<a.deviceName()<<endl;
    }

}

Widget::~Widget()
{
    qDebug()<<"widget被销毁"<<endl;
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    // 如果是鼠标左键按下
    if(event->button() == Qt::LeftButton){
        //关闭窗口只执行画图
        //记录截图的起始坐标
        //setAttribute(Qt::WA_TranslucentBackground,false);
        ui->pushButton_2->hide();
        rect_left_x=event->x();
        rect_left_y=event->y();
        rect_right_x=event->x();
        rect_right_y=event->y();
    }
    // 如果是鼠标右键按下
    else if(event->button() == Qt::RightButton){

    }
}
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    // 这里必须使用buttons()
    if(event->buttons() & Qt::LeftButton){  //进行的按位与
        //根据数据做出框图动画

        rect_right_x=event->x();
        rect_right_y=event->y();
        update();
    }
}
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
        //记录截图结束坐标，如果用户确定则对图片进行处理和保存
//    rect_right_x=event->x();
//    rect_right_y=event->y();
//    update();
    ui->pushButton_2->move(rect_left_x,rect_left_y);
    ui->pushButton_2->show();

    //screen->grabWindow(0).save("123","jpg");
}
void Widget::paintEvent(QPaintEvent *event1){

    QPainter painter(this);


         // 反走样
          painter.setRenderHint(QPainter::Antialiasing, true);
          // 设置画笔颜色、宽度
          painter.setPen(QPen(QColor(0, 160, 230), 2));
         //pen.setColor(QColor(255,0,0,255));
         QBrush brush;   //画刷。填充几何图形的调色板，由颜色和填充风格组成
         brush.setColor(QColor(255,255,255,225));
         brush.setStyle(Qt::SolidPattern);
                //可在QPaintDevice上绘制各种图形。QPaintDevice有之类QWidget、QImage、QOpenGLPaintDevice等
         //painter.setPen(pen);
         painter.setBrush(brush);
             //painter.drawRect(50,50,200,100);
         painter.drawRect(rect_left_x,rect_left_y, rect_right_x-rect_left_x,rect_right_y-rect_left_y);




}

void Widget::on_pushButton_clicked()
{
    this->close();
}

void Widget::on_pushButton_2_clicked()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("Screen_yyyy_MM_dd_hh_mm_ss");
    QString s("C:/Users/trn123456/Desktop/");
    s+=current_date+".png";
    //获取当前屏幕截图
    this->hide();
    this->screen->grabWindow(0).save(s,"png");

    //开始裁剪屏幕
    QImage b;
    b.load(s);
    //加个超边界判定
    QImage b1=b.copy(((rect_left_x<rect_right_x)?rect_left_x:rect_right_x),((rect_left_y<rect_right_y)?rect_left_y:rect_right_y), abs(rect_right_x-rect_left_x),abs(rect_right_y-rect_left_y));
    b1.save(s,"png");


     //复制图像，可粘贴到画图中
       QImage *image=new QImage();
       image->load(s);

       QClipboard *clip=QApplication::clipboard();

       clip->setImage(*image);

    this->close();
}

void Widget::on_pushButton_3_clicked()
{
    //st=new start(this);//帮到父窗口上，就粘到父窗口上了，试试非模态
    //st=new start(ui->comboBox->currentIndex());
    Parameter pa;
    if(ui->comboBox_3->currentIndex()==0){
        //pa.wid=screen->
        pa.x=0;
        pa.y=0;
        pa.wid=screen->size().width();
        pa.hei=screen->size().height();
    }else{
        pa.x=(rect_left_x<rect_right_x)?rect_left_x:rect_right_x;
        pa.y=(rect_left_y<rect_right_y)?rect_left_y:rect_right_y;
        pa.wid=abs(rect_left_x-rect_right_x);
        pa.hei=abs(rect_left_y-rect_right_y);
    }
    pa.fps=(ui->comboBox_2->currentIndex()+1)*30;
    pa.adapt=ui->comboBox->currentIndex();
    //pa.bit_rate=ui->comboBox_4->currentIndex()
    if(ui->comboBox_4->currentIndex()==0){
        pa.bit_rate=18;
    }else if(ui->comboBox_4->currentIndex()==1){
        pa.bit_rate=4;
    }else{
        pa.bit_rate=0.8;
    }
    if(filename!=""){
        pa.path=filename;
    }else{
        pa.path='.';

    }
    pa.audio=ui->comboBox_5->currentIndex();
    //pa.audio=QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    //qDebug()<<"pa.x:"<<pa.x<<"pa.y:"<<pa.y<<endl;
    this->hide();//问题在这如果hide了就子窗口就调用不了槽函数,有毒吧又可以了
    st=new start(pa);

    //this->showMinimized();//这个子窗口可以继续使用信号和槽机制，即使不把子窗口绑到当前窗口上
    st->show();

}

void Widget::on_pushButton_4_clicked()
{
    QFileDialog *fd=new QFileDialog(this);
    fd->setFileMode(QFileDialog::Directory);
    //QUrl ur=fd->getSaveFileUrl(this,tr("选择文件保存路径"));
    filename=fd->getExistingDirectory(this,QString("选择文件保存路径"));

}


void Widget::on_pushButton_5_clicked()
{
    if(ishide){
        this->ui->comboBox->hide();
        this->ui->comboBox_2->hide();
        this->ui->comboBox_3->hide();
        this->ui->comboBox_4->hide();
        this->ui->comboBox_5->hide();
        this->ui->pushButton->hide();
        this->ui->pushButton_2->hide();
        this->ui->pushButton_3->hide();
        this->ui->pushButton_4->hide();
        ishide=false;
    }else{
        this->ui->comboBox->show();
        this->ui->comboBox_2->show();
        this->ui->comboBox_3->show();
        this->ui->comboBox_4->show();
        this->ui->comboBox_5->show();
        this->ui->pushButton->show();
        this->ui->pushButton_2->show();
        this->ui->pushButton_3->show();
        this->ui->pushButton_4->show();
        ishide=true;
    }
}
