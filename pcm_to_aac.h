#pragma once
#include<iostream>
#include<string>
#include<fstream>
#define TRDEBUG
extern "C" {
#include<libavcodec/avcodec.h>//编解码库
#include<libavformat/avformat.h>//封装库
#include<libavutil/avutil.h>//工具库
#include<libswresample/swresample.h>//重采样库
}
//#pragma warning(disable: 4996)
//#pragma comment(lib,"avformat.lib")
//#pragma comment(lib,"avcodec.lib")
//#pragma comment(lib,"avutil.lib")
//#pragma comment(lib,"swresample.lib")

class pcm_to_aac {
public:
    pcm_to_aac(wchar_t* input, std::string output, int samplerate = 44100, int channels = 2);
	void start();
    static int pcmToAac(char*input,char*output);
	~pcm_to_aac();
private:
	//配置编码器上下文
	AVCodecContext* ac;
	//创建一个输出的上下文（配置输出格式）
	AVFormatContext* oc = NULL;
    const wchar_t* inputfile;
    const char* outputfile;
    int samplerate_ = 44100;
	int channels_=2;
};
