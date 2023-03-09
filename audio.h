#ifndef AUDIO_H
#define AUDIO_H

#include<QAudioInput>
#include<QAudioFormat>
#include<QAudioDeviceInfo>
#include<QDebug>
#include<QThread>
#include<array>
#include<QIODevice>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<thread>
#include<stdlib.h>
#include<stdio.h>
#include<QWidget>
#include<QTimerEvent>
#include <QAudio>     //这五个是QT处理音频的库
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include <QFile>
#include <QTimer>
#include <QDir>
class Audio:public QWidget
{//
    Q_OBJECT
public:

    Audio(QString filename,int deviceid);
    void start();
    void pass();
    void exit();
    //void timerEvent(QTimerEvent *e);
    QAudioInput* audioInput;
    QIODevice * out_io;
    QAudioFormat format;
    int device=1;
    QString file;

public:
    QFile sourceFile;   // class member.
    QFile destinationFile;   // Class member
    //QAudioFormat auido_input_format;
    //QAudioInput *audio_in;
    //QAudioOutput *audio_out;
    //qint64 CreateWavFile(QString catheFileName , QString wavFileName);
    QList<QAudioDeviceInfo> input_list;
    QList<QAudioDeviceInfo> output_list;
private slots:
    void handleStateChanged_input(QAudio::State newState);

};

#endif // AUDIO_H


