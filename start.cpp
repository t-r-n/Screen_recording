
#include "start.h"
#include "ui_start.h"
#define SLEEP(n) std::this_thread::sleep_for(std::chrono::milliseconds(n));

#if _MSC_VER >= 1600	// MSVC2015 > 1899,	MSVC_VER = 14.0
#pragma execution_character_set("utf-8")
#endif

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
    tv=std::make_shared<ToVideo>(pa.path.toStdWString(),"libx264",wid,hei,16,pa.bit_rate);//libx264
    //tv=std::make_shared<ToVideo>(pa.path.toStdWString(),"libx264",wid,hei,pa.fps,pa.bit_rate);//libx264
    //tv=std::make_shared<ToVideo>(pa.path.toStdString().c_str(),"libx264",wid,hei,pa.fps);//libx264
        tv->gx=pa.x;
        tv->gy=pa.y;
        tv->gw=pa.wid;
        tv->gh=pa.hei;
        qDebug()<<tv->gx<<" "<<tv->gy<<" "<<tv->gw<<" "<<tv->gh<<endl;

        ispass.store(true);
        setMouseTracking(true);
        QPixmap pix;
        pix.load(":/image/exit.png");

        pix.load(":/image/start.png");
        pix=pix.scaled(60,60,Qt::KeepAspectRatio);

        QPalette pal(this->palette());
        pal.setColor(QPalette::Background, Qt::white); //设置背景黑色
        this->setAutoFillBackground(true);
        this->setPalette(pal);


        setAttribute(Qt::WA_TranslucentBackground);

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

void start::showFPS(){

    //ui->label->setText(QString("fps"));
}

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
        if(isCapAudio){
            au->exit();
        }
        std::thread(&start::handleSource,this).detach();
    }else{}

}
void start::on_pushButton_clicked()
{
//    this->timerid1=startTimer(std::chrono::milliseconds(1));
//    this->timerid2=startTimer(std::chrono::milliseconds(1000));
//    //if(isCapAudio)ado->start();

//    if(isCapAudio){
//        //are->toggleRecord(QAudioDeviceInfo::availableDevices(QAudio::AudioInput).at(audio-1).deviceName());
//        au->start();
//    }
//    //else are->toggleRecord(QString());
//    std::thread th1(&start::screenCapture,this);
//    th1.detach();
//    this->showMinimized();
}
void start::on_label_2_clicked(){
    qDebug()<<"调用按钮"<<endl;
    if(first){//这边就可以用static静态变量
        this->timerid1=startTimer(std::chrono::milliseconds(1));
        this->timerid2=startTimer(std::chrono::milliseconds(1000));
        this->timerid4=startTimer(std::chrono::milliseconds(250));//每多少ms发送一次内存
        if(isCapAudio){
            au->start();
        }
        std::thread th1(&start::screenCapture,this);
        th1.detach();
        this->showMinimized();
        first=false;
    }else{
#if 0
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
#endif
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
        //if(iss)ui->label->setText(QString(QString::number(sleepTimes.load())));
        //emit emitFPS(QString::number(count));//信号槽方式
        isNextFrame.store(true);

    }if(e->timerId()==timerid4){

    }
}


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
void start::sleepFunc(int times){
    std::this_thread::sleep_for(std::chrono::milliseconds(times));
}
void start::threadpix(std::shared_ptr<std::vector<QPixmap>>buffer,unsigned int id){//

    auto beforeTime = std::chrono::steady_clock::now();
    std::shared_ptr<std::vector<std::vector<std::string>>>ve=std::make_shared<std::vector<std::vector<std::string>>>();
#if 1 //恒定帧率
    for (auto it=(*buffer).begin();it!=(*buffer).end();it++) {
        if((*it).isNull()){//帧有问题，补帧
            if(it!=(*buffer).begin()){//它前面的帧肯定是好的,如果不是第一帧
                (*it)=(*(it-1));
            }else{//从后面找一张好的
                for(auto itt=it+1;itt!=(*buffer).end();++itt){
                    if(!(*itt).isNull()){
                        (*it)=(*itt);
                        break;
                    }
                }
                if((*it).isNull()){//遍历后面也没找到好的这一整个就有问题
                    qDebug()<<"执行："<<__LINE__<<endl;
                    break;
                }
            }
        }
    }
#endif
    int greatFrameCount=0;//描述容器内好的帧数
    for (auto it=(*buffer).begin();it!=(*buffer).end();it++) {
        if(!(*it).isNull()){
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
#if 1
    /****/
    qDebug()<<"greatFrame:"<<greatFrameCount<<endl;
    buffer->resize(greatFrameCount);
    //int tmpframe=pa.fps;//******************************************帧率在这边改
    int tmpframe=16;
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
#endif

    for(int i=0;i<greatFrameCount;++i){
        if((*buffer)[i].isNull()){
            qDebug()<<"图片错误"<<endl;
            continue;
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(55));//70最合理保证在下一次线程到来前处理完目前的
        sleepFunc(sleepTimes.load());
        if(pa.x||pa.y){
            tv->sendFrame((*buffer)[i].copy(pa.x,pa.y,pa.wid,pa.hei),*ve);
        }else{
            tv->sendFrame(((*buffer)[i]),*ve);
        }

    }

    std::async(std::launch::async,&start::threadSave,this,ve,id);
    auto afterTime = std::chrono::steady_clock::now();
    //毫秒级
    double duration_millsecond = std::chrono::duration<double, std::milli>(afterTime - beforeTime).count();
    //sleepTimes.store(duration_millsecond);//test
//    if(duration_millsecond>960){
//        sleepTimes-=5;
//    }else if(duration_millsecond<940){
//        sleepTimes+=5;
//    }
    if(duration_millsecond>990){
        sleepTimes-=3;
    }else if(duration_millsecond<960){
        sleepTimes+=3;
    }
}

void start::screenCapture(){//还有一个想法不按顺序每秒进行一次264编码记录时间顺序，最后按顺序重新整合
    int times=0;
    while(1){
        std::unique_lock<std::mutex>sbguard(my_pass);
        semu_pass.wait(sbguard, [this]() {return ispass.load();});
        std::shared_ptr<std::vector<QPixmap>>ve=std::make_shared<std::vector<QPixmap>>(30);
        ve->clear();
        for(int i=0;i<30;++i){
            if(isNextFrame.load()){break;}
            try{
                ve->emplace_back(dxm.grabScreen());//emplace时间可以忽略不计，占大头应该是return时的拷贝需要的时间
                if(ve->back().isNull()){
                    qDebug()<<"截图失败"<<endl;
                }
            }catch(std::exception &e){
                qDebug()<<e.what()<<endl;
            }catch(...){}
            ++curpixf;
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
        }
        qDebug()<<"screen size:"<<ve->size()<<endl;
        isNextFrame.store(false);
        int tmp=ve->size();
        auto autoptr = std::mem_fn(&start::threadpix);//可以理解成把成员函数转化为普通函数被线程池里的完美转发识别
        executor.commit(autoptr,this,ve,id++);
        //executor.commit(&start::threadpix,this,ve,id++);
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


    this->ui->label->setText(QString("waiting"));
    this->executor.joinall();//这句可能有问题，线程池还没结束被这边嘎住了 //问题在这joinall提前退出了，线程池里还有任务没有结束

    //tv.reset();//释放ffmpeg合成对象
    this->ui->label->setText(QString("ok"));
    //qDebug()<<"error on"<<__LINE__<<endl;
    QString tmp=audioPath+".aac";
    QString tmp1=audioPath+".pcm";
    pcm_to_aac pta((wchar_t*)audioPath.toStdWString().c_str(),tmp.toStdString());
    pta.start();//中文路径问题

    //pcm_to_aac::pcmToAac(audioPath.toLocal8Bit().data(),tmp.toLocal8Bit().data());
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


