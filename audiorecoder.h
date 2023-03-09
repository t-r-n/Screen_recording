#ifndef AUDIORECODER_H
#define AUDIORECODER_H

#include <QWidget>
#include <QMainWindow>
#include <QMediaRecorder>
#include <QUrl>
#include<QAudioRecorder>
#include<QAudioProbe>
class QAudioRecorder;
class AudioRecoder : public QWidget
{
    Q_OBJECT
public:
    explicit AudioRecoder(QWidget *parent = nullptr);
    void setOutputLocation(QString filename);
    void togglePause();
    void toggleRecord(QString device);
//signals:


//private slots:

//    void updateStatus(QMediaRecorder::Status);
//    void onStateChanged(QMediaRecorder::State);
//    void updateProgress(qint64 pos);
//    void displayErrorMessage();

    //void on_outputButton_clicked();



    //Ui::AudioRecorder *ui = nullptr;

    QAudioRecorder *m_audioRecorder = nullptr;
    QAudioProbe *m_probe = nullptr;
    //QList<AudioLevel*> m_audioLevels;
    bool m_outputLocationSet = false;
};

#endif // AUDIORECODER_H
