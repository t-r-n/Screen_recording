#include "audiorecoder.h"

AudioRecoder::AudioRecoder(QWidget *parent) : QWidget(parent)
{



//    for (auto &device: m_audioRecorder->audioInputs()) {
//        //ui->audioDeviceBox->addItem(device, QVariant(device));
//    }
    m_audioRecorder = new QAudioRecorder(this);
    m_probe = new QAudioProbe(this);
//    connect(m_probe, &QAudioProbe::audioBufferProbed,
//            this, &AudioRecorder::processBuffer);
//    m_probe->setSource(m_audioRecorder);
    qDebug()<<"执行到此："<<__LINE__<<endl;
#if 0
    connect(m_audioRecorder, &QAudioRecorder::durationChanged, this, &AudioRecoder::updateProgress);
    connect(m_audioRecorder, &QAudioRecorder::statusChanged, this, &AudioRecoder::updateStatus);//状态发生改变
    connect(m_audioRecorder, &QAudioRecorder::stateChanged, this, &AudioRecoder::onStateChanged);
    connect(m_audioRecorder, QOverload<QMediaRecorder::Error>::of(&QAudioRecorder::error), this,
            &AudioRecoder::displayErrorMessage);
#endif

}
#if 0
void AudioRecoder::updateProgress(qint64 duration)
{
    if (m_audioRecorder->error() != QMediaRecorder::NoError || duration < 2000)
        return;

    //ui->statusbar->showMessage(tr("Recorded %1 sec").arg(duration / 1000));
}

void AudioRecoder::updateStatus(QMediaRecorder::Status status)
{
    QString statusMessage;

    switch (status) {
    case QMediaRecorder::RecordingStatus:
        //statusMessage = tr("Recording to %1").arg(m_audioRecorder->actualLocation().toString());
        break;
    case QMediaRecorder::PausedStatus: //暂停状态
        //clearAudioLevels();             //清楚ui界面水平条
        //statusMessage = tr("Paused");
        break;
    case QMediaRecorder::UnloadedStatus:
    case QMediaRecorder::LoadedStatus:  //如果是正在读取状态 框上就是stopped
        //clearAudioLevels();
        //statusMessage = tr("Stopped");
    default:
        break;
    }

//    if (m_audioRecorder->error() == QMediaRecorder::NoError)
//        ui->statusbar->showMessage(statusMessage);
}

void AudioRecoder::onStateChanged(QMediaRecorder::State state)
{
    switch (state) {
    case QMediaRecorder::RecordingState:
        //ui->recordButton->setText(tr("Stop")); //就是设置不同的button格式
        //ui->pauseButton->setText(tr("Pause"));
        break;
    case QMediaRecorder::PausedState:
        //ui->recordButton->setText(tr("Stop"));
        //ui->pauseButton->setText(tr("Resume"));
        break;
    case QMediaRecorder::StoppedState:
        //ui->recordButton->setText(tr("Record"));
        //ui->pauseButton->setText(tr("Pause"));
        break;
    }

    //ui->pauseButton->setEnabled(m_audioRecorder->state() != QMediaRecorder::StoppedState);
}

//static QVariant boxValue(const QComboBox *box)
//{
//    int idx = box->currentIndex();
//    if (idx == -1)
//        return QVariant();

//    return box->itemData(idx);
//}


#endif
void AudioRecoder::toggleRecord(QString device)//开始记录
{
    if (m_audioRecorder->state() == QMediaRecorder::StoppedState) {
        //m_audioRecorder->setAudioInput(boxValue(ui->audioDeviceBox).toString());//选择录音设备

        m_audioRecorder->setAudioInput(device);
        QAudioEncoderSettings settings;//设置各种参数
        settings.setCodec("audio/pcm");
        settings.setSampleRate(48000);
        settings.setBitRate(176400);
        settings.setChannelCount(2);
        //settings.setQuality(QMultimedia::EncodingQuality(ui->qualitySlider->value()));
        settings.setEncodingMode(QMultimedia::ConstantBitRateEncoding);

        //QString container = boxValue(ui->containerBox).toString();

        m_audioRecorder->setEncodingSettings(settings, QVideoEncoderSettings(),"audio/x-pcm");
        m_audioRecorder->record();//开始记录
        qDebug()<<"执行到此："<<__LINE__<<endl;
        qDebug()<<"使用的设备："<<device<<endl;
    }
    else {
        qDebug()<<"执行到此："<<__LINE__<<endl;
        m_audioRecorder->stop();//不是停止态说明应该停止，保存文件
    }
}

void AudioRecoder::togglePause()
{
    if (m_audioRecorder->state() != QMediaRecorder::PausedState){
        qDebug()<<"执行到此："<<__LINE__<<endl;
        m_audioRecorder->pause();
    }
    else{
        qDebug()<<"执行到此："<<__LINE__<<endl;
        m_audioRecorder->record();//继续记录
    }
}

void AudioRecoder::setOutputLocation(QString filename)
{
//#ifdef Q_OS_WINRT
//    // UWP does not allow to store outside the sandbox
//    const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
//    if (!QDir().mkpath(cacheDir)) {
//        qWarning() << "Failed to create cache directory";
//        return;
//    }
//    QString fileName = cacheDir + QLatin1String("/output.wav");
//#else
//    //QString fileName = QFileDialog::getSaveFileName();
//#endif
      qDebug()<<"执行到此："<<__LINE__<<endl;
    m_audioRecorder->setOutputLocation(QUrl::fromLocalFile(filename));

      qDebug()<<"执行到此："<<__LINE__<<QUrl::fromLocalFile(filename)<<endl;
    m_outputLocationSet = true;
}

//void AudioRecoder::displayErrorMessage()
//{
//    //ui->statusbar->showMessage(m_audioRecorder->errorString());
//}

//void AudioRecoder::clearAudioLevels()
//{
////    for (int i = 0; i < m_audioLevels.size(); ++i)
////        m_audioLevels.at(i)->setLevel(0);
//}
