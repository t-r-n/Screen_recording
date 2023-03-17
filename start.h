#ifndef START_H
#define START_H
//#pragma comment(lib, “Gdi32.lib”)
#include <QWidget>
#include<QPixmap>
#include<QRect>
#include<QtWin>
#include<QTimerEvent>
#include<QScreen>
#include<QGuiApplication>
#include<QDebug>
#include<QDateTime>
#include<thread>
#include<QPixmap>
#include<QBuffer>
#include<queue>
#include<mutex>
#include<condition_variable>
#include"dxgimanager.h"
#include"thread_pool.h"
#include<QImage>
#include "tovideo.h"
#include<chrono>
#include<QDateTime>
#include <QMouseEvent>
#include"startpushbutton.h"
#include"audio.h"
//#include"audiorecoder.h"
#include<QAudioDeviceInfo>
#include<QAudio>
#include<stdio.h>
#include<stdlib.h>
#include"pcm_to_aac.h"
#include<fstream>
#include<string>
#include<QPaintEvent>
#include <QStyleOption>
#include "mix.h"
#include<QBuffer>
#include<fstream>
#include<inttypes.h>
#include <functional>
#include<future>
/*
*该项目到此暂停，先学习ffmpegh264编码的东西
* 现在是线程池中每次只有一个线程在运行
* 在rgb转yuv那一部分压内存，这边要么空间换时间要么时间换空间
* 是一条不可行路
* ，看h264编码找到是否可以让ffmpeg多线程压缩的方法，
*知道每一帧的压缩后的顺序，想办法先压缩不写文件记录每一个packet的顺序，永陵外的线程去按顺序的写文件
*/


#pragma pack(1)
struct Sbuf{
    uint32_t id;
    uint32_t size;
    uint8_t i;
};
#pragma pack()
struct Parameter{
    int fps;  //帧率
    int adapt; //显卡
    int audio;
    int wid; //捕获区域宽高
    int hei;
    int x;  //捕获区域左上角坐标
    int y;
    float bit_rate;//画质
    QString path;
};
namespace Ui {
class start;
}

struct Mybuffer{
    std::queue<QPixmap>qu;
};
class start : public QWidget
{
    Q_OBJECT

public:
    explicit start(Parameter pa,QWidget *parent = nullptr);
    void timerEvent(QTimerEvent *e);
    ~start();
    QScreen *screen = QGuiApplication::primaryScreen();
    void threadpix(std::shared_ptr<std::vector<QPixmap>>buffer,unsigned int id);
    //void threadpix(std::shared_ptr<std::vector<QImage>>buffer,unsigned int index);
    //void threadPKT(std::shared_ptr<std::vector<QPixmap>>buffer,unsigned int id);
    void threadSave(std::shared_ptr<std::vector<std::vector<std::string>>>ve,unsigned int id);
    //void threadpix1();
    //void thread_avi();
    static QImage grabWindow(HWND winId, int x, int y, int w, int h);
    DxgiManager dxm;
    void tmpTest(std::shared_ptr<std::vector<QImage>>vce,int id);
    void tmpTest1(int id);
    ilovers::TaskExecutor executor{ 1};
    ilovers::TaskExecutor executor1{ 4};
    //int selectAdapt=0;//设置要选择的显卡号
    void sleepFunc(int times);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent*pe);
    void handleSource();
signals:
    void emitFPS(QString count);
    //void emitFPS();
private slots:
    void on_pushButton_2_clicked();
    //void showFPS(QString conut);
    void showFPS();
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();
    void on_label_2_clicked();
    void on_label_3_clicked();
    void on_label_4_clicked();
    void on_label_5_clicked();




private:
    std::atomic<int> sleepTimes={60};//ms
    std::ifstream infile;
    std::ofstream outfile;
    std::mutex mu_file;
    //QPixmap grabWindow(HWND winId, int x, int y, int w, int h);//抓取一张图片
    //ToVideo tv=ToVideo("ou.mp4","h264");
    //StartPushButton*stButton;
    Parameter pa;
    std::shared_ptr<ToVideo> tv;
    int index=0;
    Ui::start *ui;
    int timerid1;
    int timerid2;
    int timerid3;
    int timerid4;
    bool is=true;
    bool isCapAudio=false;
    bool first=true;
    bool _start=false;
    //bool isit=false;
    //int zhenlv=30;//抓取图片帧率
    std::atomic<unsigned int> curpix;
    std::atomic<unsigned int> curpixf;
    std::atomic<bool> isexit;
    std::atomic<bool> isNextFrame;
    unsigned int prepix=0;
    //int framcount;
    int time=0;
    //void start::mDealWithFrame(QByteArray& frame)
    //std::queue<QPixmap>qu;
    //void start_convert();
    void screenCapture();
    std::mutex mu_pix;
    std::shared_ptr<Audio>ado;
//    char *frameBuffer;  //一帧的图片缓冲区，保存从其他线程发来的图片
//        long bytes;         //帧的长度
//        avi_t *out_fd;      //avilib库中定义的文件数据结构
//        char  *filename;    //要保存的文件名
//        volatile bool stopFlag;
//        QString toAviFilePath;
    std::mutex my_mutex;
    //bool isend=false;
    std::condition_variable semu_cond;

    bool iscontinue=true;

    //AudioRecoder *are;
    int audio=0;

    std::shared_ptr<Audio>au;

    std::mutex my_pass;
    std::atomic<bool> ispass;
    std::condition_variable semu_pass;
    std::atomic<int>taskingcount;  //描述正在运行的任务数；
    QString filePath;
    QString audioPath;
    QPoint movePoint;
    bool iss=true;
    int wid;
    int hei;
    unsigned int id=0;
    unsigned int curid=0;

    std::vector<std::shared_ptr<std::vector<QImage>>>testVe;
    //QPixmap preCursor;//保存前一帧图像
};

#endif // START_H
