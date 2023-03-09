#ifndef AUDIO_PLUS_H
#define AUDIO_PLUS_H

#include <QWidget>
#include <QWidget>
#include <QtGui>
#include <QAudioInput>
#include <QAudioOutput>

class audio_plus
{
    Q_OBJECT
public:
    void init();
    void read();

    audio_plus();
private slots:
    void stopRecording();
    void finishedPlaying(QAudio::State state);

private:
    QFile outputFile;   // class member.
    QAudioInput* audio; // class member.
    QAudioOutput *outAudio;

};

#endif // AUDIO_PLUS_H
