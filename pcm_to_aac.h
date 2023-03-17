#pragma once
#include<iostream>
#include<string>
#include<fstream>
#define TRDEBUG
extern "C" {
#include<libavcodec/avcodec.h>//������
#include<libavformat/avformat.h>//��װ��
#include<libavutil/avutil.h>//���߿�
#include<libswresample/swresample.h>//�ز�����
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
	//���ñ�����������
	AVCodecContext* ac;
	//����һ������������ģ����������ʽ��
	AVFormatContext* oc = NULL;
    const wchar_t* inputfile;
    const char* outputfile;
    int samplerate_ = 44100;
	int channels_=2;
};
