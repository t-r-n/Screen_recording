#include "audio.h"

Audio::Audio(QString filename,int deviceid)
{

        format.setSampleRate(44100);
        format.setChannelCount(2);   //设定声道数目，mono(平声道)的声道数目是1；stero(立体声)的声道数目是2
        format.setSampleSize(16);
        format.setCodec("audio/pcm");   //编码器
        format.setByteOrder(QAudioFormat::LittleEndian);   //设定高低位,LittleEndian（低位优先）/LargeEndian(高位优先)
        format.setSampleType(QAudioFormat::SignedInt);
        //获取默认的音频输入设备，判断是否支持指定的格式，如果不支持则使用一个邻近的格式
//    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
//            if (!info.isFormatSupported(format))
//            {
//                format = info.nearestFormat(format);
//                qDebug()<<"Recodec:"<<format.codec()<<endl;

//            }
        device=deviceid;
        file=filename;

        auto al=QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
        //QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
        if (!al.at(device-1).isFormatSupported(format))
        {
            format = al.at(device-1).nearestFormat(format);
            qDebug()<<"Recodec:"<<format.codec()<<endl;
        }
        qDebug()<<"执行到此:"<<al.at(device-1).deviceName()<<" "<<format.sampleRate()<<endl;
        format.sampleRate();
        audioInput = new QAudioInput(al.at(device-1),format, NULL);
        //audio_in = new QAudioInput(info,auido_input_format, this);
        connect(audioInput,SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged_input(QAudio::State)));

}

void Audio::start(){
    if(audioInput->state()==QAudio::StoppedState)
    {
       // qDebug()<<"没有处理任何数据.\n";
        //设置采集的时间
        //QTimer::singleShot(AUDIO_INPUT_TIME,this,SLOT(stopRecording()));
        destinationFile.setFileName(file);
        destinationFile.open( QIODevice::WriteOnly | QIODevice::Truncate);
        audioInput->start(&destinationFile);
    }
}
void Audio::pass(){

    audioInput->suspend();
    //iscontinue.store(false);
}
void Audio::exit(){
    audioInput->stop();
    destinationFile.close();
    delete audioInput;
    //isexit.store(true);
}
void Audio::handleStateChanged_input(QAudio::State newState)
{
 switch (newState) {
     case QAudio::StoppedState:
         if (audioInput->error() != QAudio::NoError) {
             // Error handling
             qDebug()<<"音频录制出错"<<audioInput->error()<<endl;
             //Log_Display(QString("录音出现错误.\n"));
         } else {
             // Finished recording
             //Log_Display(QString("完成录音\n"));
             //将PCM文件转为WAV文件
             //CreateWavFile(SAVE_FILE_PATH,SAVE_WAV_FILE_PATH);
         }
         break;
     case QAudio::ActiveState:
         // Started recording - read from IO device
         //Log_Display(QString("开始从IO设备读取PCM声音数据.\n"));
         break;
     default:
         // ... other cases as appropriate
         break;
 }
}











//录音状态
/*

QAaudio：：ActiveState
0
正在处理音频数据，此状态是在调用 start（） 后设置的，并且音频数据可供处理。
Qaudio：：SuspendedState暂停状态
1
音频流处于挂起状态。在调用 suspend（） 后或当另一个流控制音频设备时输入。在后一种情况下，对恢复的调用会将音频设备的控制权返回到此流。这通常只应在用户请求时完成。
QAaudio：：StoppedState停止状态
2
音频设备已关闭，并且未处理任何音频数据
QAaudio：：IdleState
3
传入的 QIODevice 没有数据，音频系统的缓冲区为空，此状态是在调用 start（） 之后设置的，而没有音频数据可供处理。
Qaudio：：InterruptedState中断状态
4
此流处于挂起状态，因为另一个优先级较高的流当前可以控制音频设备。在较高优先级的流放弃对音频设备的控制之前，播放无法恢复。

*/





//开始播放音频
#if 0
void Audio::on_pushButton_2_clicked()
{
    static bool flag=1;
    if(flag)
    {
        flag=0;
        //QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        //选择声卡输出设备
        QAudioDeviceInfo info=output_list.at(ui->comboBox_output->currentIndex());

        if(!info.isFormatSupported(auido_input_format))
        {
          Log_Display(QString("后端不支持原始音频格式，无法播放音频.\n"));
          return;
        }
        //当前设备支持的编码
        Log_Display(QString("当前设备支持的编码格式:\n"));
        QStringList list=info.supportedCodecs();
        for(int i=0;i<list.size();i++)
        {
            Log_Display(list.at(i)+"\n");
        }
        audio_out = new QAudioOutput(info,auido_input_format,this);
        connect(audio_out,SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged_out(QAudio::State)));
    }
    sourceFile.setFileName(SAVE_FILE_PATH);
    sourceFile.open(QIODevice::ReadOnly);
    audio_out->start(&sourceFile);
}

//播放音频的反馈信息
void Audio::handleStateChanged_out(QAudio::State newState)
{
    switch (newState){
          case QAudio::IdleState:
              // Finished playing (no more data)
              audio_out->stop();
              sourceFile.close();
              Log_Display(QString("音频播放完成.\n"));
              break;

          case QAudio::StoppedState:
              // Stopped for other reasons
              if (audio_out->error() != QAudio::NoError) {
                 Log_Display(QString("播放音频出现错误.\n"));
              }
              break;
          default:
              // ... other cases as appropriate
              break;
      }
}
#endif




