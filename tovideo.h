#ifndef TOVIDEO_H
#define TOVIDEO_H


#include <QImage>
#include<iostream>
#include<QPixmap>
#include<string>
#include<QDebug>
#include <mutex>
#include<chrono>
#include<thread>
#include <condition_variable>
//#ifdef	__cplusplus
extern "C"
{
//#endif
#include <stdlib.h>
#include<inttypes.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
//#include
//#ifdef __cplusplus

};
//#endif
#include <libyuv.h>
//#include "libyuv/convert.h"
class ToVideo
{
private:
    const wchar_t *filename;
    const char  *codec_name;
    const AVCodec *codec;
    AVCodecContext *c= NULL;
    int i, ret, x, y;
    FILE *f;
    AVFrame *frame;
    AVPacket *pkt;
    uint8_t endcode[4];

public:
    ToVideo();
    ToVideo(std::wstring file_name,std::string codecname,int wid,int hei,int fps,float bitrate);
    void toFrame(QImage image,AVPacket*pkt,AVFrame*frame);
    static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                       FILE *outfile,std::vector<std::vector<std::string>>&ve);
    void setFrames(int fps);
    void flushEncode();
    void sendFrame(const QPixmap &pix,std::vector<std::vector<std::string>>&ve);
    void writeFile(std::shared_ptr<std::vector<std::vector<std::string>>>ve);
    ~ToVideo();
    int gx,gy,gw,gh;
};





#endif // TOVIDEO_H

