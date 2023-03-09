
#include "start.h"
#include "ui_start.h"
#define SLEEP(n) std::this_thread::sleep_for(std::chrono::milliseconds(n));
std::string GBKToUTF8(const char* str_GBK)
{
    int len = MultiByteToWideChar(CP_ACP, 0, str_GBK, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, str_GBK, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    std::string strTemp = str;
    if (wstr) delete[] wstr;
    if (str) delete[] str;
    return strTemp;
}


start::start(Parameter pa1,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::start)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    pa=pa1;
    ui->setupUi(this);
    wid=screen->size().width();
    hei=screen->size().height();
    dxm.init(pa.adapt);
    curpix.store(0);
    curpixf.store(0);
    isexit.store(false);
    taskingcount.store(0);
    void (start::*p)(QString)=&start::emitFPS;
    connect(this,p,[=](QString count){
        qDebug()<<"触发信号"<<count<<endl;
        ui->label->setText(QString(count+"fps"));
    });
    qDebug()<<"wid"<<pa.wid<<"hei:"<<pa.hei<<endl;
    //QString path="./firecv/"
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString data=current_date_time.toString("MM_dd_hh_mm_ss_");
#define AUDIO
#ifdef AUDIO
    if(pa.audio>0){
        if(pa.path!="."){
            audioPath=pa.path+"/"+data+"audio";
        }else{
            audioPath=QCoreApplication::applicationDirPath()+"/"+data+"audio";
        }
        au=std::make_shared<Audio>(audioPath,pa.audio);
        isCapAudio=true;
    }else{
        isCapAudio=false;
    }
#else
    if(pa.audio>0){
        are=new AudioRecoder(this);

        if(pa.path!="."){
            audioPath=pa.path+"/"+data+"audio";
            are->setOutputLocation(audioPath);
        }else{
            audioPath=QCoreApplication::applicationDirPath()+"/"+data+"audio";
            are->setOutputLocation(audioPath);
        }
        isCapAudio=true;
    }
#endif
    pa.path+="/"+data+"video.264";
    //pa.path+="/"+current_date_time.toString("MM_dd_hh_mm_ss_vi.264");
    //pa.path.toUtf8().data();
    qDebug()<<pa.path<<endl;
    filePath=pa.path;

    audio=pa.audio;
    //are.setOutputLocation(QString(pa.path.begin(),(pa.path.end()-3))+"pcm");
    qDebug()<<"图片帧数："<<pa.fps<<endl;
    tv=std::make_shared<ToVideo>(pa.path.toStdWString(),"libx264",wid,hei,15,pa.bit_rate);//libx264
    //tv=std::make_shared<ToVideo>(pa.path.toStdWString(),"libx264",wid,hei,pa.fps,pa.bit_rate);//libx264
    //tv=std::make_shared<ToVideo>(pa.path.toStdString().c_str(),"libx264",wid,hei,pa.fps);//libx264
        tv->gx=pa.x;
        tv->gy=pa.y;
        tv->gw=pa.wid;
        tv->gh=pa.hei;
        qDebug()<<tv->gx<<" "<<tv->gy<<" "<<tv->gw<<" "<<tv->gh<<endl;
    //connect(this,SIGNAL(emitFPS(QString)),this,SLOT(showFPS(QString)));
    //connect(this,&start::emitFPS,this,&start::showFPS);
    //qDebug()<<
    //connect(this,SIGNAL(emitFPS()),this,SLOT(showFPS()));
        ispass.store(true);
        setMouseTracking(true);
        QPixmap pix;
        pix.load(":/image/exit.png");
//        ui->pushButton_4->setMouseTracking(true);
//        pix=pix.scaled(30,20,Qt::KeepAspectRatio);
//        ui->pushButton_4->setFixedSize((pix.width()),pix.height());
//        ui->pushButton_4->setStyleSheet("QPushButton{border:0px;}");
//        ui->pushButton_4->setIcon(pix);
//        ui->pushButton_4->setIconSize(QSize(pix.width(),pix.height()));


        pix.load(":/image/start.png");
        pix=pix.scaled(60,60,Qt::KeepAspectRatio);
//        ui->pushButton->setFixedSize((pix.width()),pix.height());
//        ui->pushButton->setStyleSheet("QPushButton{border:0px;}");
//        ui->pushButton->setIcon(pix);
//        ui->pushButton->setIconSize(QSize(pix.width(),pix.height()));


//        ui->pushButton->setStyleSheet("QPushButton{border-image: url(:/new/prefix1/image/Main_preset_normal.bmp);}"
//                                      "QPushButton:hover{border-image: url(:/new/prefix1/image/Main_preset_hold.bmp);}"
//                                      "QPushButton:pressed{border-image: url(:/new/prefix1/image/Main_preset_down.bmp);}");


        //this->setFocusPolicy(Qt::NoFocus);//使按钮被点击时无虚线框
        //ui->pushButton_2->hide();
        //ui->pushButton_3->hide();
        //ui->label->hide();

//        stButton=new StartPushButton(this);
//        stButton->move(50,50);
        //connect(ui->label_2,&stlabel::clicked,this,&start::on_label_2_clicked);


        QPalette pal(this->palette());
        pal.setColor(QPalette::Background, Qt::white); //设置背景黑色
        this->setAutoFillBackground(true);
        this->setPalette(pal);

        //ui->start->setStyleSheet(QString::fromUtf8("border:3px solid blue"));

//        this->setStyleSheet(QString::fromUtf8("#start{border:3px solid red}"));
//        this->setStyleSheet("QWidget#start{border: 1px solid #FF00FF; border-radius: 5px;};");
        //this->update();
        setAttribute(Qt::WA_TranslucentBackground);
//            setStyleSheet(
//                            "background-color: white;\
//                             border-radius: 15px;   "
//                         );
        //ui->label_3->changeSize(0.05,0.06);
        ui->label_4->changeSize(0.1,0.11);
        ui->label_5->changeSize(0.05,0.06);
        ui->label->move(this->width()-ui->label->width()-ui->label_4->width()+7,35);
        ui->label_2->move(this->width()-ui->label->width()-ui->label_5->width()-ui->label_3->width()-ui->label_4->width()-ui->label_2->width()-10,5);
        ui->label_3->move(this->width()-ui->label->width()-ui->label_5->width()-ui->label_4->width()-ui->label_3->width()-10,5);
        ui->label_4->move(this->width()-ui->label->width()-ui->label_4->width(),-7);
        ui->label_5->move(this->width()-ui->label->width()-ui->label_5->width()-ui->label_4->width(),10);
        setFixedSize(this->width()-70,this->height());
}

start::~start()
{
    qDebug()<<"start被销毁"<<endl;
    delete ui;
}
//void start::showFPS(QString count){

//    ui->label->setText(QString(count+"fps"));
//}
void start::showFPS(){

    //ui->label->setText(QString("fps"));
}
//void start::emitFPS(QString count){}
//void start::emitFPS(){}
void start::mousePressEvent(QMouseEvent *ev){
    movePoint = ev->globalPos() - pos();
}
void start::mouseMoveEvent(QMouseEvent *ev)
{
    // 这里必须使用buttons()
    if(ev->buttons() & Qt::LeftButton){  //进行的按位与
        //根据数据做出框图动画
        QPoint movePos = ev->globalPos();
        move(movePos - movePoint);
        //move(ev->globalPos()-pos());
        //this->move(ev->globalX(),ev->globalY());
    }

}
void start::paintEvent(QPaintEvent*pe){
#if 1
    QStyleOption opt;
//        opt.init(this);
        QPainter painter(this);
//        style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

#endif

#if 1
    //QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); //抗锯齿
    QPen pen(QColor(0,0,0));
    pen.setWidth(2);
    //painter.setPen(QColor(0, 0, 0));
    painter.setPen(pen);
    painter.setBrush(QColor(255, 255, 255));
    //painter.drawRoundRect(100, 100, 600, 300, 4, 4);
    painter.drawRoundRect(0,0,this->width(),this->height(),4,4);
#endif
#if 0
    QPainter painter(this);
    QPainterPath path;
    path.addRoundedRect(QRect(100, 100, 600, 300), 4.0, 4.0);
    painter.fillPath(path, QBrush(QColor(255, 182, 193)));
#endif
#if 0
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); //抗锯齿
    painter.setPen(QColor(255, 182, 193));
    painter.setBrush(QColor(255, 182, 193));
    QPainterPath path;
    path.addRoundedRect(QRect(100, 100, 600, 300), 10.0, 10.0);
    painter.drawPath(path);
#endif
}
#if 0
void start::start_convert(){
    QString tmpFile= toAviFilePath+QDateTime::currentDateTime().toString("yyyy-MM-dd--hh:mm:ss")+".avi";
        filename = tmpFile.toUtf8().data();//以当前时间定义文件名和路径
        out_fd = AVI_open_output_file(filename); //把文件描述符绑定到此文件上
        if(out_fd == NULL)
        {
            qDebug()<<"open file erro";
        }
        AVI_set_video(out_fd,320,240,15,"MJPG");//设置视频文件的格式

        //QPixmap image("d:/aaa.png");
          QByteArray bytearray;
          QBuffer buffer(&bytearray);
          buffer.open(QIODevice::WriteOnly);
          //bool bOk = image.save(&buffer,"PNG",20);
          QPixmap image;
          for(int i=0;;++i){
              if(image.load(QString("C:/Users/trn123456/Desktop/firecv/%1.jpg").arg(i))){
                  QByteArray bytearray;
                  QBuffer buffer(&bytearray);
                  buffer.open(QIODevice::WriteOnly);
                  bool bOk = image.save(&buffer,"JPG",20);//20是图片质量-1--100即图像压缩率
                  if(bOk){
                      frameBuffer = bytearray.data();
                      bytes = bytearray.length();
                      if(frameBuffer != NULL && bytes !=0)
                      {

                          if(AVI_write_frame(out_fd,frameBuffer,bytes,1)<0)//向视频文件中写入一帧图像
                          {
                             qDebug()<<"write erro";
                          }else{
                              frameBuffer = NULL;
                              bytes = 0;
                         }
                      }else{
                          break;
                      }
                  }else{
                      break;
                  }
              }else{
                  break;
              }
          }
       AVI_close (out_fd);  //关闭文件描述符，并保存文件
}

#endif

#if 0
avi_t* AVI_open_output_file(char * filename); //创建一个操作输出文件的对象

void AVI_set_video(avi_t *AVI, int width, int height, double fps, char *compressor);  //设置视频文件的分辨率， 多少帧一秒， 编码方式

int  AVI_write_frame(avi_t *AVI, char *data, long bytes, int keyframe); //保存一帧图像, keyframe表示是否为关键帧(即是否完整的一帧图像数据， jpg每张都是完整的)

int  AVI_close(avi_t *AVI); //结束保存视频，并会写入文件索引的。
#endif //api解释
#if 0
void start::thread_avi()
{
    framcount=curpix/time;
    qDebug()<<"一共录了"<<time<<"秒"<<framcount<<" "<<curpix<<endl;

    std::unique_lock<std::mutex>sbguard(my_mutex);
    semu_cond.wait(sbguard, [this]() {
        if (isend == true)return true;
        qDebug()<<"avi转换线程阻塞中"<<endl;
        return false;
        });
    qDebug()<<"avi转换线程被唤醒被唤醒"<<endl;
    frameBuffer = NULL;
    bytes = 0;
    stopFlag = false;
    QDateTime current_date_time =QDateTime::currentDateTime();
    toAviFilePath = "./avi";
    framcount=curpix/time;
    qDebug()<<"一共录了"<<time<<"秒"<<framcount<<" "<<curpix<<endl;

//    std::thread th(&start::start_convert,this);

//    th.detach();

    //while(true);
    QString tmpFile= toAviFilePath+QDateTime::currentDateTime().toString("hh_mm_ssello")+".avi";
        filename = tmpFile.toUtf8().data();//以当前时间定义文件名和路径
        out_fd = AVI_open_output_file(filename); //把文件描述符绑定到此文件上
        if(out_fd == NULL)
        {
            qDebug()<<"open file erro";
        }
        AVI_set_video(out_fd,320,240,framcount,"MJPG");//设置视频文件的格式//15是帧率

        //QPixmap image("d:/aaa.png");
          QByteArray bytearray;
          QBuffer buffer(&bytearray);
          buffer.open(QIODevice::WriteOnly);
          //bool bOk = image.save(&buffer,"PNG",20);
          QPixmap image;
          int i=0;
          for(;;++i){
              if(image.load(QString("C:/Users/trn123456/Desktop/firecv/%1.jpg").arg(i))){
              //image=screen->grabWindow(0);
                  QByteArray bytearray;
                  QBuffer buffer(&bytearray);
                  buffer.open(QIODevice::WriteOnly);
                  bool bOk = image.save(&buffer,"JPG",100);//20是图片质量-1--100即图像压缩率
                  if(bOk){
                      frameBuffer = bytearray.data();
                      bytes = bytearray.length();
                      if(frameBuffer != NULL && bytes !=0)
                      {

                          if(AVI_write_frame(out_fd,frameBuffer,bytes,1)<0)//向视频文件中写入一帧图像
                          {
                             qDebug()<<"write erro";
                          }else{
                              frameBuffer = NULL;
                              bytes = 0;
                         }
                      }else{
                          break;
                      }
                  }else{
                      break;
                  }
              }else{
                  break;
              }
          }
       AVI_close (out_fd);  //关闭文件描述符，并保存文件
        //qDebug()<<"一共产生"<<i<<"张图像"<<endl;

    //this->close();

}
#endif
#if 0
void start::threadpix1(){//
//    for (;!buffer.empty();) {
//        buffer.front().save(QString("./firecv/%1.jpg").arg(index++),"jpg",50);
//        buffer.pop();
//    }
    int ret=1;
    while(1){
        std::unique_lock<std::mutex>sbguard1(mu_pix);
        semu_cond.wait(sbguard1, [this]() {
            if (isend == true)return true;
            return false;
            });
        //std::unique_lock<std::mutex>lo(mu_pix,std::defer_lock);
        //if(lo.try_lock()){
            for(;!qu.empty();){
                ret=qu.front().save(QString("./firecv/%1.jpg").arg(curpix++),"jpg",50);
                //qDebug()<<curpix<<endl;
                if(ret==0){
                    curpix--;
                }
                qu.pop();
            }
            isend=false;
        //}
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

#endif

#if 1
QImage start::grabWindow(HWND winId, int x, int y, int w, int h)
{

    RECT r;
    GetClientRect(winId, &r);

    if (w < 0) w = r.right - r.left;
    if (h < 0) h = r.bottom - r.top;

    HDC display_dc = GetDC(winId);
    HDC bitmap_dc = CreateCompatibleDC(display_dc);
    HBITMAP bitmap = CreateCompatibleBitmap(display_dc, w, h);
    HGDIOBJ null_bitmap = SelectObject(bitmap_dc, bitmap);

    BitBlt(bitmap_dc, 0, 0, w, h, display_dc, x, y, SRCCOPY | CAPTUREBLT);

    CURSORINFO ci;
    ci.cbSize = sizeof(CURSORINFO);
    GetCursorInfo(&ci);
    if ((ci.ptScreenPos.x > x) && (ci.ptScreenPos.y > y) && (ci.ptScreenPos.x < (x + w)) && (ci.ptScreenPos.y < (y + h)))
        DrawIcon(bitmap_dc, ci.ptScreenPos.x - x, ci.ptScreenPos.y - y, ci.hCursor);
     //QPixmap pixmap = QPixmap::fromImage(QImage::fromHICON(ci.hCursor));

    // clean up all but bitmap
    ReleaseDC(winId, display_dc);
    SelectObject(bitmap_dc, null_bitmap);
    DeleteDC(bitmap_dc);

    QPixmap pixmap = QtWin::fromHBITMAP(bitmap);

    DeleteObject(bitmap);

    return pixmap.toImage();

}
#endif
void start::on_pushButton_2_clicked()
{
    if(iss){
        isexit.store(true);
        qDebug()<<"视频录制时长times:"<<time<<endl;
        iss=false;
        //ado->exit();
        qDebug()<<"执行到此"<<__LINE__<<endl;
        if(isCapAudio){
            qDebug()<<"执行到此"<<__LINE__<<endl;
            au->exit();
        }//are->toggleRecord(QString());
        std::thread(&start::handleSource,this).detach();
    }else{
        //tv->~ToVideo();
    }

}
void start::on_pushButton_clicked()
{
    this->timerid1=startTimer(std::chrono::milliseconds(1));
    this->timerid2=startTimer(std::chrono::milliseconds(1000));
    //if(isCapAudio)ado->start();

    if(isCapAudio){
        //are->toggleRecord(QAudioDeviceInfo::availableDevices(QAudio::AudioInput).at(audio-1).deviceName());
        au->start();
    }
    //else are->toggleRecord(QString());
    std::thread th1(&start::screenCapture,this);
    th1.detach();
    this->showMinimized();
}
void start::on_label_2_clicked(){
    qDebug()<<"调用按钮"<<endl;
    if(first){//这边就可以用static静态变量
        this->timerid1=startTimer(std::chrono::milliseconds(1));
        this->timerid2=startTimer(std::chrono::milliseconds(1000));
        if(isCapAudio){
            //are->toggleRecord(QAudioDeviceInfo::availableDevices(QAudio::AudioInput).at(audio-1).deviceName());
            au->start();
        }
        std::thread th1(&start::screenCapture,this);
        th1.detach();
        this->showMinimized();
        first=false;
    }else{
        if(iscontinue){//现在是继续状态
            //if(isCapAudio)//are->togglePause();
            ispass.store(false);//截屏线程挂起
            iscontinue=false;

        }else{
            ispass.store(true);//截屏线程启动
            //if(isCapAudio)//are->togglePause();
            iscontinue=true;
            semu_pass.notify_all();
        }
    }
}

void start::timerEvent(QTimerEvent *e){
    if(e->timerId()==timerid1){
    }
    if(e->timerId()==timerid2){
        time++;
        int tmp=curpixf.load();
        int count=tmp-prepix;
        prepix=tmp;
        if(iss)ui->label->setText(QString(QString::number(count)+"fps"));
        //emit emitFPS(QString::number(count));//信号槽方式
        isNextFrame.store(true);
    }
}
//void start::threadpix(std::shared_ptr<std::vector<QPixmap>>buffer,unsigned int index){//
//    int frames=0;
//    QPixmap preSuccPix;
//    for (size_t i=0;i<(*buffer).size();++i) {//怎么补帧
//        if((*buffer)[i].save(QString("./firecv/%1.jpg").arg(index++),"jpg",50)){
//            preSuccPix=(*buffer)[i];
//            frames++;
//        }else {
//            if(!preSuccPix.isNull()){
//                --index;
//                preSuccPix.save(QString("./firecv/%1.jpg").arg(index++),"jpg",50);
//            }else{
//                --index;
//                for (size_t j=i;j<(*buffer).size();++j) {//前面没有好的帧从后面找好的帧
//                    if(!(*buffer)[j].isNull()){
//                        if((*buffer)[j].save(QString("./firecv/%1.jpg").arg(index++),"jpg",50)){
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//    }
//    //qDebug()<<"成功存入"<<frames<<"帧"<<endl;
//}

void start::threadSave(std::shared_ptr<std::vector<std::vector<std::string>>>ve,unsigned int id){
    std::unique_lock<std::mutex>sbguard(my_mutex);
    semu_cond.wait(sbguard, [id,this]() {
        if (id== curid)return true;
        return false;
        });

    tv->writeFile(ve);

    ++curid;
    semu_cond.notify_all();
}
void start::threadpix(std::shared_ptr<std::vector<QImage>>buffer,unsigned int id){//
//void start::threadpix(std::shared_ptr<std::vector<QImage>>buffer,unsigned int id){//
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //tv->setFrames((*buffer).size());

    //qDebug()<<"指针绑定数"<<buffer.use_count()<<endl;
    qDebug()<<"执行："<<__LINE__<<endl;
    qDebug()<<"当前save id:"<<id<<endl;
    auto beforeTime = std::chrono::steady_clock::now();
    std::shared_ptr<std::vector<std::vector<std::string>>>ve=std::make_shared<std::vector<std::vector<std::string>>>();
#if 1 //运行恒帧算法
    for (auto it=(*buffer).begin();it!=(*buffer).end();it++) {
        if(it->isNull()){//帧有问题，补帧
            if(it!=(*buffer).begin()){//它前面的帧肯定是好的,如果不是第一帧
                (*it)=*(it-1);
                qDebug()<<"执行："<<__LINE__<<endl;
            }else{//从后面找一张好的
                for(auto itt=it+1;itt!=(*buffer).end();++itt){
                    if(!itt->isNull()){
                        (*it)=(*itt);
                        qDebug()<<"执行："<<__LINE__<<endl;
                        break;
                    }
                }
                if(it->isNull()){//遍历后面也没找到好的这一整个就有问题
                    qDebug()<<"执行："<<__LINE__<<endl;
                    break;
                }
            }
        }
    }
    int greatFrameCount=0;//描述容器内好的帧数
    for (auto it=(*buffer).begin();it!=(*buffer).end();it++) {
        //qDebug()<<"执行："<<__LINE__<<endl;
        if(!it->isNull()){
            greatFrameCount++;
        }else{
            break;
        }
    }
    if(greatFrameCount==0){
        ++curid;
        semu_cond.notify_all();
        return;
    }
    qDebug()<<"greatFrame:"<<greatFrameCount<<endl;
    buffer->resize(greatFrameCount);
    //int tmpframe=pa.fps;//******************************************帧率在这边改
    int tmpframe=15;
    if(greatFrameCount<tmpframe){//补帧,间断补帧
        while(greatFrameCount<tmpframe){
            qDebug()<<"执行："<<__LINE__<<endl;
            int interval=tmpframe-greatFrameCount;
            for (auto it=(*buffer).begin()+greatFrameCount-1;it>=(*buffer).begin();it-=3) {
                if(interval){
                    buffer->insert(it,*it);//当前帧前面插一帧当前帧
                    interval--;
                    greatFrameCount++;
                }else{
                    break;
                }
                //SLEEP(100);
            }
        }
    }else if(greatFrameCount>tmpframe){//间断抽帧
        while(greatFrameCount>tmpframe){
            int interval=greatFrameCount-tmpframe;
            for (auto it=(*buffer).begin();it<(*buffer).end();it+=3) {
                if(interval){
                    it=(*buffer).erase(it);
                    interval--;
                    greatFrameCount--;
                }else{
                    break;
                }
                //SLEEP(100);
            }
        }
    }
#if 0
    int i=0;
    for(;i<buffer->size();++i){
        if((*buffer)[i].isNull()){
            break;
        }
    }
    qDebug()<<"完好的图片帧数"<<i<<" "<<greatFrameCount<<endl;
#endif
    for(int i=0;i<greatFrameCount;++i){
        if((*buffer)[i].isNull()){
            qDebug()<<"图片错误"<<endl;
            continue;
        }

        if(pa.x||pa.y){
            tv->sendFrame((*buffer)[i].copy(pa.x,pa.y,pa.wid,pa.hei),*ve);
        }else{
            tv->sendFrame((*buffer)[i],*ve);
        }
        //SLEEP(10);
        qDebug()<<"commit id:"<<id<<endl;
    }
#endif
#if 0
    static int curPixs=0;
    for(std::size_t i=0;i<(*buffer).size();++i){
        if(!((*buffer)[i].isNull())){
            //QPainter pa(&((*buffer)[i]));
            //(*buffer)[i].save(QString("D:/firecv/%1_%2.jpg").arg(id).arg(curPixs++),"JPG");
            //问题找到了，传递到这个函数里时有些图片黑屏了

            tv->sendFrame((*buffer)[i],*ve);
        }
    }
#endif
#if 0
    static int curPixs=0;
    for(std::size_t i=0;i<buffer->size();++i){
        if(!(((*buffer)[i]).isNull())){
            //QPainter pa(&(buffer[i]));
            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
            //pa.drawLine(1,1,2,2);
            ((*buffer)[i]).save(QString("D:/firecv/%1_%2.jpg").arg(id).arg(curPixs++),"JPG");
            //问题找到了，传递到这个函数里时有些图片黑屏了

            tv->sendFrame(((*buffer)[i]),*ve);
        }
    }
#endif
    auto autoptr = std::mem_fn(&start::threadSave);
    executor1.commit(autoptr,this,ve,id);

    auto afterTime = std::chrono::steady_clock::now();
    //毫秒级
    double duration_millsecond = std::chrono::duration<double, std::milli>(afterTime - beforeTime).count();
    qDebug() << duration_millsecond << "毫秒" << endl;
}

void start::screenCapture(){//还有一个想法不按顺序每秒进行一次264编码记录时间顺序，最后按顺序重新整合
    int times=0;
    while(1){
        //暂停继续功能在这里加用条件变量
        std::unique_lock<std::mutex>sbguard(my_pass);
        semu_pass.wait(sbguard, [this]() {return ispass.load();});
        std::shared_ptr<std::vector<QImage>>ve=std::make_shared<std::vector<QImage>>(100);
        //std::shared_ptr<std::vector<QPixmap>>ve=std::make_shared<std::vector<QPixmap>>(100);//每隔一段时间对一部分图片ffmpeg合成视频然后删除
        ve->clear();
        for(int i=0;i<100;++i){
            if(isNextFrame.load()){break;}
            try{
                ve->emplace_back(std::move(dxm.grabScreen()));//emplace时间可以忽略不计，占大头应该是return时的拷贝需要的时间
                if(ve->back().isNull()){
                    qDebug()<<"截图失败"<<endl;
                }
            }catch(std::exception &e){
                qDebug()<<e.what()<<endl;
            }catch(...){}
            ++curpixf;
            std::this_thread::sleep_for(std::chrono::milliseconds(70));
        }
        isNextFrame.store(false);
        int tmp=ve->size();
        auto autoptr = std::mem_fn(&start::threadpix);//可以理解成把成员函数转化为普通函数被线程池里的完美转发识别
        executor.commit(autoptr,this,ve,id++);
        curpix+=tmp;
        if(isexit.load())break;
        times++;
        //break;//测试一秒的截屏用
    }
}

void start::on_pushButton_3_clicked()
{
    if(iscontinue){//现在是继续状态
        ispass.store(false);//截屏线程挂起
        iscontinue=false;

    }else{
        ispass.store(true);//截屏线程启动
        iscontinue=true;
        semu_pass.notify_all();
    }
}

void start::on_pushButton_4_clicked()
{
    exit(1);
}

void start::handleSource(){//pcm 和h264文件 转aac转 mp4

    this->ui->label->setText(QString("处理中"));
    this->executor.joinall();//这句可能有问题，线程池还没结束被这边嘎住了 //问题在这joinall提前退出了，线程池里还有任务没有结束
    this->ui->label->setText(QString("ok"));
    qDebug()<<"error on"<<__LINE__<<endl;
//    while(taskingcount)
//    {
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//    }
    //outfile.close();
    while(1){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::this_thread::yield();
    }
    tv->~ToVideo();
    this->ui->label->setText(QString("over"));
    qDebug()<<"error on"<<__LINE__<<endl;
    QString tmp=audioPath+".aac";
    QString tmp1=audioPath+".pcm";
    pcm_to_aac pta((wchar_t*)audioPath.toStdWString().c_str(),tmp.toStdString());
    pta.start();//中文路径问题   //貌似打开文件时文件还在被占用，打开失败
    pta.~pcm_to_aac();
    QString outputFile=audioPath+".mp4";
    //以上获得aac文件和wav，现在需要学习ffmpeg和mp4和aac获得最终结果，合成视屏差了两秒
    muxer_main((char*)filePath.toStdString().c_str(),(char*)tmp.toStdString().c_str(),(char*)outputFile.toStdString().c_str());
}
void start::on_label_3_clicked(){
    if(iss){
        isexit.store(true);
        qDebug()<<"视频录制时长times:"<<time<<endl;
        iss=false;
        //ado->exit();
        //if(isCapAudio)au->exit();
        if(isCapAudio){
            au->exit();//pcm
        }//are->toggleRecord(QString());
            //are->toggleRecord(QString());
        std::thread(&start::handleSource,this).detach();
    }else{
        //tv->~ToVideo();
    }
}
void start::on_label_4_clicked(){
    this->showMinimized();
}
void start::on_label_5_clicked(){
    exit(1);
}

void start::threadPKT(std::shared_ptr<std::vector<QPixmap>>buffers,unsigned int id){
    for(int i=0;i<buffers->size();++i){
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        (*buffers)[i].save(&buffer, "PNG",10); // writes pixmap into bytes in PNG format
        qDebug()<<"buffer size"<<bytes.size();
        //(*buffers)[i].loadFromData(bytes,"PNG");
        QPixmap p;

        p.loadFromData(bytes,"PNG");
        (*buffers)[i]=p;
//        QByteArray bytes1;
//        QBuffer buffer1(&bytes1);
//        buffer1.open(QIODevice::WriteOnly);
//        (*buffers)[i].save(&buffer1, "JPG",100); // writes pixmap into bytes in PNG format
//        //QPixmap().swap((*buffers)[i]);
//        qDebug()<<"buffer1 size"<<bytes1.size();
//        //(*buffers)[i].loadFromData(bytes,"JPG");

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    //qDebug()<<"线程池1内任务数"<<executor1.taskCount()<<endl;
    auto autoptr = std::mem_fn(&start::threadpix);//可以理解成把成员函数转化为普通函数被线程池里的完美转发识别
    //executor.commit(autoptr,this,buffers,id);
    semu_cond.notify_all();
}
//void start::threadSave(std::shared_ptr<std::vector<QPixmap>>buffers,unsigned int id){
//    std::lock_guard<std::mutex> lock(mu_file);
//    std::string sbuf;
//    sbuf.resize(10);
////    char idb[4];
////    int tmpbytes;
//    Sbuf ss;
//    for(int i=0;i<buffers->size();++i){
//        QByteArray bytes;
//        QBuffer buffer(&bytes);
//        buffer.open(QIODevice::WriteOnly);
//        (*buffers)[i].save(&buffer, "PNG",10); // writes pixmap into bytes in PNG format
//        //"&id-i-size&"  //定长四个字节存id 一个字节存i 四个字节存size 每次先读9个字节再分析再读数据再读9个字节
//        sbuf.clear();
//        //memcpy(idb,(char*)id,4);
//        ss.id=id;
//        ss.size=bytes.size();
//        ss.i=i;
////        sbuf+=std::string((char*)&ss,9);
////        outfile.write(sbuf.c_str(),9);
//        outfile.write((char*)&ss,9);
//        outfile.write(bytes.data(),bytes.size());
//        std::this_thread::sleep_for(std::chrono::milliseconds(20));
//    }
//}
