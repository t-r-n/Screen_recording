#include "audioin.h"
#include <QDebug>
AudioIn::AudioIn(QWidget *parent) : QWidget(parent)
{
    init();
    //read();
}




void AudioIn::init()
{
    outputFile.setFileName("./test.raw");
    outputFile.open( QIODevice::WriteOnly | QIODevice::Truncate );
    QAudioFormat format;

    format.setSampleRate(48000);
    format.setChannelCount(2);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format))
    {
       qWarning()<<"default format not supported try to use nearest";
       format = info.nearestFormat(format);
    }
    audio = new QAudioInput(info, format, this);
    QTimer::singleShot(10000, this, SLOT(stopRecording()));
    audio->start(&outputFile);

     qDebug() <<"record begin!";
}


void AudioIn::read()
{
    outputFile.setFileName("./test.raw");
    outputFile.open(QIODevice::ReadOnly);
    QAudioFormat format;

    format.setSampleRate(48000);
    format.setChannelCount(2);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format))
    {
     qWarning()<<"raw audio format not supported by backend, cannot play audio.";
     return;
    }

    outAudio = new QAudioOutput(format, this);
    connect(outAudio,SIGNAL(stateChanged(QAudio::State)),SLOT(finishedPlaying(QAudio::State)));
    outAudio->start(&outputFile);
    qDebug() <<"play begin!";
}

void AudioIn::stopRecording()
{
    audio->stop();
    outputFile.close();
    delete audio;
    qDebug() <<"record end!";
}

void AudioIn::finishedPlaying(QAudio::State state)
{
    if(state == QAudio::IdleState)
    {
         outAudio->stop();
         outputFile.close();
         delete outAudio;
    }
    qDebug() << "play end!";
}
