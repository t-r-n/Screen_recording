#ifndef AUDIOIN_H
#define AUDIOIN_H

#include <QWidget>
#include <QWidget>
#include <QtGui>
#include <QAudioInput>
#include <QAudioOutput>
#include <iostream>

class AudioIn : public QWidget
{
    Q_OBJECT
public:
    explicit AudioIn(QWidget *parent = nullptr);
public:

    void init();

    void read();

private slots:
    void stopRecording();
    void finishedPlaying(QAudio::State state);

private:
    QFile outputFile;   // class member.
    QAudioInput* audio; // class member.
    QAudioOutput *outAudio;

signals:

};

#endif // AUDIOIN_H
