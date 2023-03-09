#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include<QPainter>
#include<QGuiApplication>
#include<QScreen>
#include<QImage>
#include<QDateTime>
#include<QString>
#include<QMenuBar>
#include<QMenu>
#include<QAction>
#include"start.h"
#include"dxgimanager.h"
#include<QFileDialog>
#include<stdlib.h>
#include<QAudioDeviceInfo>
#include<QAudio>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    friend class Dialog;
    bool iscatchsound=false;
    int  zhenshu;    //设置录屏帧数
    int fenbianlv;  //设置分辨率
    bool isglobalscreen=false;
    void changesound(bool is){iscatchsound=is;}
    Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QScreen *screen = QGuiApplication::primaryScreen();

    void paintEvent(QPaintEvent *event1);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();



    void on_pushButton_4_clicked();



    void on_pushButton_5_clicked();

private:

    start *st=0;
    bool ishide=true;
    QString filename;
    QMenuBar*bar;
    QMenu* fileMenu;
    QMenu* editMenu;

    Ui::Widget *ui;
    int rect_left_x;
    int rect_left_y;
    int rect_right_x;
    int rect_right_y;
};
#endif // WIDGET_H
