#ifndef MIX_H
#define MIX_H
#if 0
#include <stdio.h>  
extern "C"{
#include <libavcodec/avcodec.h>  
#include <libavformat/avformat.h>  
#include <libavcodec/bsf.h>
#include <libavutil/mathematics.h>
}
#include<QDebug>

//class trError{
//public:
//    static void logError(const char*str);
//};
//void trError::logError(const char *str){
//#ifdef QDEBUG
//    std::cout<<"error on:"<<__LINE__<<" : "<<str<<std::endl;
//#else
//    qDebug()<<"error on:"<<__LINE__<<" : "<<str<<endl;
//#endif
//}

/* 
 FIX: H.264 in some container format (FLV, MP4, MKV etc.) need 
 "h264_mp4toannexb" bitstream filter (BSF) 
 *Add SPS,PPS in front of IDR frame 
 *Add start code ("0,0,0,1") in front of NALU 
 H.264 in some container (MPEG2TS) don't need this BSF. 
 */  
//'1': Use H.264 Bitstream Filter  
#define USE_H264BSF 1  
  
/* 
 FIX:AAC in some container format (FLV, MP4, MKV etc.) need 
 "aac_adtstoasc" bitstream filter (BSF) 
 */  
//'1': Use AAC Bitstream Filter  
#define USE_AACBSF 1  
  
  
//  
//static int open_input_file(const char *filename)  
//{  
//    int ret;  
//    AVCodec *dec;  
//      
//    if ((ret = avformat_open_input(&pFormatCtx, filename, NULL, NULL)) < 0) {  
//        printf( "Cannot open input file\n");  
//        return ret;  
//    }  
//      
//    if ((ret = avformat_find_stream_info(pFormatCtx, NULL)) < 0) {  
//        printf( "Cannot find stream information\n");  
//        return ret;  
//    }  
//      
//    /* select the video stream */  
//    ret = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);  
//    if (ret < 0) {  
//        printf( "Cannot find a video stream in the input file\n");  
//        return ret;  
//    }  
//    video_stream_index = ret;  
//    pCodecCtx = pFormatCtx->streams[video_stream_index]->codec;  
//      
//    /* init the video decoder */  
//    if ((ret = avcodec_open2(pCodecCtx, dec, NULL)) < 0) {  
//        printf( "Cannot open video decoder\n");  
//        return ret;  
//    }  
//      
//    return 0;  
//}  
  
inline int muxer_main(char *inputH264FileName,char *inputAacFileName,char *outMP4FileName)
{  
    AVOutputFormat *ofmt =NULL;  //声明输出格式
    //Input AVFormatContext and Output AVFormatContext  
    AVFormatContext *ifmt_ctx_v =NULL, *ifmt_ctx_a =NULL,*ofmt_ctx = NULL;//声明输入和输出的属性
    AVPacket pkt;  //数据压缩后的包
    AVCodec *dec;  //目标解码器
   int ret, i;  
   int videoindex_v=-1,videoindex_out=-1;  
   int audioindex_a=-1,audioindex_out=-1;  
   int frame_index=0;  
    int64_t cur_pts_v=0,cur_pts_a=0;  
       
   const char *in_filename_v =inputH264FileName;  
   const char *in_filename_a =inputAacFileName;  
   const char *out_filename =outMP4FileName;//Output file URL  
      
    printf("==========in h264==filename:%s\n",in_filename_v);  
    printf("==========in aac ===filename:%s\n",in_filename_a);  
    printf("==========outfile:%s\n",out_filename);  

    //Input  
   if ((ret = avformat_open_input(&ifmt_ctx_a, in_filename_a,NULL,NULL)) <0) { //打开输入文件，用avformatcontext保存输入文件的属性
//        printf("=====11========RET:%d\n",ret);  
        printf( "Could not open input file.");  
        qDebug()<<"error on "<<__LINE__<<endl;
        return -1;
    }  
//    printf("=====2========RET:%d\n",ret);  
   if ((ret = avformat_find_stream_info(ifmt_ctx_a,0)) <0) {   //找找有没有这个输入文件属性的流
        printf( "Failed to retrieve input stream information");  
        qDebug()<<"error on "<<__LINE__<<endl;
        //goto end;
         return -1;
    }  
      
      
   if ((ret = avformat_open_input(&ifmt_ctx_v, in_filename_v,NULL,NULL)) <0) {  //打开输入文件，用avformatcontext保存输入文件的属性
        printf( "Could not open input file:%d\n",ret);  
        qDebug()<<"error on "<<__LINE__<<endl;
        //goto end;
         return -1;
    }  
//    printf("=====0========RET:%d\n",ret);  
   if ((ret = avformat_find_stream_info(ifmt_ctx_v,0)) <0) {  //找找有没有这个输入文件属性的流
        printf( "Failed to retrieve input stream information");  
        qDebug()<<"error on "<<__LINE__<<endl;
        //goto end;
         return -1;
    }  
      
//    /* init the video decoder */  
//    if ((ret = avcodec_open2(ifmt_ctx_a->, dec, NULL)) < 0) {  
//        printf( "Cannot open video decoder\n");  
//        return ret;  
//    }  
//      
  
     
    printf("===========Input Information==========\n");  
    av_dump_format(ifmt_ctx_v,0, in_filename_v,0);  
    av_dump_format(ifmt_ctx_a,0, in_filename_a,0);    //打印输入文件信息
    printf("======================================\n");  
    //Output  
    avformat_alloc_output_context2(&ofmt_ctx,NULL,NULL, out_filename);  //给输出文件的属性记录先分配内存
   if (!ofmt_ctx) {  
        printf( "Could not create output context\n");  
        ret = AVERROR_UNKNOWN;
        qDebug()<<"error on "<<__LINE__<<endl;
        //goto end;
         return -1;
    }  
    ofmt = (AVOutputFormat *)ofmt_ctx->oformat;   //记录输出文件的格式
      
   for (i =0; i < ifmt_ctx_v->nb_streams; i++) {  //处理输入文件的每一个流
        //Create output AVStream according to input AVStream  
       if (ifmt_ctx_v->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {//如果是视频流
            AVStream *in_stream = ifmt_ctx_v->streams[i];  //取得该流的地址
            AVStream* out_stream = avformat_new_stream(ofmt_ctx, NULL);//不知道更改后，这里是不是填null //给输出添加一个流
            videoindex_v=i;  //流的索引号
           if (!out_stream) {  
                printf( "Failed allocating output stream\n");
                qDebug()<<"error on "<<__LINE__<<endl;
                ret = AVERROR_UNKNOWN;  
                 return -1;
            }  
            videoindex_out=out_stream->index;  
            //Copy the settings of AVCodecContext  
            ret =av_dict_set(&out_stream->metadata,"rotate","90",0);//设置旋转角度，这tm是干什么
           if(ret>=0)  
            {  
                printf("=========yes=====set rotate success!===\n");  
            }  
           const AVCodec* vc = avcodec_find_decoder(in_stream->codecpar->codec_id); // 软解  给该流找解码器，之后编码用
    // vc = avcodec_find_decoder_by_name("h264_mediacodec"); // 硬解
           if (!vc) {
               //LOGE("avcodec_find_decoder[videoStream] failure");
               //goto end;
               qDebug()<<"error on "<<__LINE__<<endl;
                return -1;
           }
           AVCodecContext* codec_ctx = avcodec_alloc_context3(vc);//给编码器分配记录内容，属性的空间
           ret = avcodec_parameters_to_context(codec_ctx, in_stream->codecpar);//复制流的编码器
           if (ret < 0) {
               printf("Failed to copy in_stream codecpar to codec context\n");
               //goto end;
               qDebug()<<"error on "<<__LINE__<<endl;
                return -1;
           }

           codec_ctx->codec_tag = 0;
           if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
               codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

           ret = avcodec_parameters_from_context(out_stream->codecpar, codec_ctx);//从编码器给流编码器
           if (ret < 0) {
               printf("Failed to copy codec context to out_stream codecpar context\n");
               //goto end;
               qDebug()<<"error on "<<__LINE__<<endl;
                return -1;
           }
           break;  //这一个流做完就不用去下一个流了
        }  
    }  
      
   for (i =0; i < ifmt_ctx_a->nb_streams; i++) {  
        //Create output AVStream according to input AVStream  
       if (ifmt_ctx_v->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
               //if(ifmt_ctx_v->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){  
           AVStream* in_stream = ifmt_ctx_a->streams[i];
               //AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);  
           AVStream* out_stream = avformat_new_stream(ofmt_ctx, NULL);
            audioindex_a=i;  
           if (!out_stream) {  
                printf( "Failed allocating output stream\n");  
                ret = AVERROR_UNKNOWN;  
                qDebug()<<"error on "<<__LINE__<<endl;
                //goto end;
                 return -1;
            }  
            audioindex_out=out_stream->index;  
           // //Copy the settings of AVCodecContext  
           //if (avcodec_copy_context(out_stream->codec, in_stream->codec) <0) {  
           //     printf( "Failed to copy context from input to output stream codec context\n");  
           //    goto end;  
           // }  
           // out_stream->codec->codec_tag =0;  
           //if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)  
           //     out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;  


           const AVCodec* vc = avcodec_find_decoder(in_stream->codecpar->codec_id); // 软解
       // vc = avcodec_find_decoder_by_name("h264_mediacodec"); // 硬解
           if (!vc) {
               //LOGE("avcodec_find_decoder[videoStream] failure");
               //goto end;
               qDebug()<<"error on "<<__LINE__<<endl;
                return -1;
           }
           AVCodecContext* codec_ctx = avcodec_alloc_context3(vc);
           ret = avcodec_parameters_to_context(codec_ctx, in_stream->codecpar);
           if (ret < 0) {
               printf("Failed to copy in_stream codecpar to codec context\n");
               //goto end;
               qDebug()<<"error on "<<__LINE__<<endl;
                return -1;
           }

           codec_ctx->codec_tag = 0;
           if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)//好像是在写全局的头
               codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

           ret = avcodec_parameters_from_context(out_stream->codecpar, codec_ctx);
           if (ret < 0) {
               printf("Failed to copy codec context to out_stream codecpar context\n");
               //goto end;
               qDebug()<<"error on "<<__LINE__<<endl;
                return -1;
           }
           break;  
        }  
    }  
      
    printf("==========Output Information==========\n");  
    av_dump_format(ofmt_ctx,0, out_filename,1);  
    printf("======================================\n");  
    //Open output file  
   if (!(ofmt->flags & AVFMT_NOFILE)) {  //打开输出流
       if (avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE) <0) {  
            printf("Could not open output file '%s'", out_filename);  
            //goto end;
            qDebug()<<"error on "<<__LINE__<<endl;
             return -1;
        }  
    }  
    //Write file header  
   if (avformat_write_header(ofmt_ctx,NULL) < 0) {  
        printf( "Error occurred when opening output file\n");  
        //goto end;
        qDebug()<<"error on "<<__LINE__<<endl;
         return -1;
    }  
      
      
    //FIX  
   /*
   //声明：	
AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb"); 
//使用
av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
//释放：
av_bitstream_filter_close(h264bsfc);  

  
  //声明
AVBSFContext *bsf_ctx = nullptr;
const AVBitStreamFilter *pfilter = av_bsf_get_by_name("h264_mp4toannexb");
av_bsf_alloc(pfilter, &bsf_ctx);
//使用：
av_bsf_send_packet(bsf_ctx, &packet);
av_bsf_receive_packet(bsf_ctx, &packet);
//释放：
av_bsf_free(&bsf_ctx);

   
   */
#if USE_H264BSF  //混合流
    //AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");  
   AVBSFContext* h264bsfc;
   h264bsfc = NULL;
    const AVBitStreamFilter* pfilter = av_bsf_get_by_name("h264_mp4toannexb");
    av_bsf_alloc(pfilter, &h264bsfc);
#endif  
#if USE_AACBSF  
    AVBSFContext* aacbsfc = NULL;
    const AVBitStreamFilter* pfilter1 = av_bsf_get_by_name("aac_adtstoasc");
    av_bsf_alloc(pfilter1, &aacbsfc);
        //av_bitstream_filter_init("aac_adtstoasc");
#endif  
      
   while (1) {  
        AVFormatContext *ifmt_ctx;  
       int stream_index=0;  
        AVStream *in_stream, *out_stream;  
          
        //Get an AVPacket  
       if(av_compare_ts(cur_pts_v,ifmt_ctx_v->streams[videoindex_v]->time_base,cur_pts_a,ifmt_ctx_a->streams[audioindex_a]->time_base) <=0){  
            ifmt_ctx=ifmt_ctx_v;  
            stream_index=videoindex_out;  
              
           if(av_read_frame(ifmt_ctx, &pkt) >=0){  
               do{  
                    in_stream  = ifmt_ctx->streams[pkt.stream_index];  
                    out_stream = ofmt_ctx->streams[stream_index];  
                      
                   if(pkt.stream_index==videoindex_v){  
                        //FIX￡∫No PTS (Example: Raw H.264)  
                       //Simple Write PTS  
                       if(pkt.pts==AV_NOPTS_VALUE){  
                           //Write PTS  
                            AVRational time_base1=in_stream->time_base;  
                            //Duration between 2 frames (us)  
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);  
                           //Parameters  
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);  
                            pkt.dts=pkt.pts;  
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);  
                            frame_index++;  
                        }  
                          
                        cur_pts_v=pkt.pts;  
                       break;  
                    }  
                }while(av_read_frame(ifmt_ctx, &pkt) >=0);  
            }else{  
               break;  
            }  
        }else{  
            ifmt_ctx=ifmt_ctx_a;  
            stream_index=audioindex_out;  
           if(av_read_frame(ifmt_ctx, &pkt) >=0){  
               do{  
                    in_stream  = ifmt_ctx->streams[pkt.stream_index];  
                    out_stream = ofmt_ctx->streams[stream_index];  
                      
                   if(pkt.stream_index==audioindex_a){  
                          
                       //FIX￡∫No PTS  
                       //Simple Write PTS  
                       if(pkt.pts==AV_NOPTS_VALUE){  
                           //Write PTS  
                            AVRational time_base1=in_stream->time_base;  
                            //Duration between 2 frames (us)  
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);  
                           //Parameters  
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);  
                            pkt.dts=pkt.pts;  
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);  
                            frame_index++;  
                        }  
                        cur_pts_a=pkt.pts;  
                          
                       break;  
                    }  
                }while(av_read_frame(ifmt_ctx, &pkt) >=0);  
            }else{  
               break;  
            }  
              
        }  
          
        //FIX:Bitstream Filter  
#if USE_H264BSF  
        //av_bitstream_filter_filter(h264bsfc, in_stream->codec,NULL, &pkt.data, &pkt.size, pkt.data, pkt.size,0);  
        av_bsf_send_packet(h264bsfc, &pkt);
        av_bsf_receive_packet(h264bsfc, &pkt);//用哪个
#endif  
#if USE_AACBSF  
        //av_bitstream_filter_filter(aacbsfc, out_stream->codec,NULL, &pkt.data, &pkt.size, pkt.data, pkt.size,0); 
        av_bsf_send_packet(aacbsfc, &pkt);
        av_bsf_receive_packet(h264bsfc, &pkt);
#endif  
          
          
        //Convert PTS/DTS  
//        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));


//        //pkt.pts=av_rescale_q_rnd(pkt.pts,in_stream->time_base,out_stream->time_base,(enum AVRounding)8197);
//        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
          
        //暂时修改方法
        pkt.pts += 4500*frame_index;
        pkt.dts += 4500*frame_index;

//        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);

        av_packet_rescale_ts(&pkt, in_stream->time_base, out_stream->time_base);
        //av_packet_rescale_ts(&pkt,in_stream->time_base,out_stream->time_base)
        pkt.pos = -1;  
        pkt.stream_index=stream_index;  
          
        printf("Write 1 Packet. size:%5d\tpts:%lld\n",pkt.size,pkt.pts);  
       //Write  
        int qRet=0;
       if ((qRet=av_interleaved_write_frame(ofmt_ctx, &pkt)) <0) {
            printf( "Error muxing packet\n");
            qDebug()<<"error on "<<__LINE__<<"error is:"<<qRet<<endl;
           break;  
        }  
        //av_free_packet(&pkt);
          av_packet_unref(&pkt);
    }  
    //Write file trailer  
    av_write_trailer(ofmt_ctx);  
      
#if USE_H264BSF  
    //av_bitstream_filter_close(h264bsfc);
    av_bsf_free(&h264bsfc);
    //av_bitstream_filter_close(h264bsfc);
#endif  
#if USE_AACBSF  
    //av_bitstream_filter_close(aacbsfc);
    av_bsf_free(&aacbsfc);
#endif
      
end:
    avformat_close_input(&ifmt_ctx_v);  
    avformat_close_input(&ifmt_ctx_a);  
    /* close output */  
   if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))  
        avio_close(ofmt_ctx->pb);  
    avformat_free_context(ofmt_ctx);  
   if (ret <0 && ret != AVERROR_EOF) {  
        printf( "Error occurred.\n");
        qDebug()<<"error on "<<__LINE__<<endl;
       return -1;  
    }  
      
    printf("======muxer mp4 success =====!\n");  
   return 0;  
}  
#endif

#if 0
extern "C"
{
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libavcodec/bsf.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libavutil/log.h"
#include "libavutil/imgutils.h"
#include "libavutil/audio_fifo.h"
#include <libswresample/swresample.h>
#include "libavutil/avstring.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libswscale/swscale.h"
}
/*
FIX: H.264 in some container format (FLV, MP4, MKV etc.) need
"h264_mp4toannexb" bitstream filter (BSF)
  *Add SPS,PPS in front of IDR frame
  *Add start code ("0,0,0,1") in front of NALU
H.264 in some container (MPEG2TS) don't need this BSF.
*/
//'1': Use H.264 Bitstream Filter
//'0': do not Use H.264 Bitstream Filter
#define USE_H264BSF 0

/*
FIX:AAC in some container format (FLV, MP4, MKV etc.) need
"aac_adtstoasc" bitstream filter (BSF)
*/
//'1': Use AAC Bitstream Filter
//'0': do not Use AAC Bitstream Filter
#define USE_AACBSF 0

inline int muxer_main(char *inputH264FileName,char *inputAacFileName,char *outMP4FileName)
{
    AVOutputFormat *ofmt = nullptr;
    //Input AVFormatContext and Output AVFormatContext
    AVFormatContext *ifmt_ctx_v = nullptr, *ifmt_ctx_a = nullptr,*ofmt_ctx = nullptr;
    AVPacket pkt;
    int ret, i;
    int videoindex_v=-1,videoindex_out=-1;
    int audioindex_a=-1,audioindex_out=-1;
    int frame_index=0;
    int64_t cur_pts_v=0,cur_pts_a=0;

    const char *in_filename_a = inputAacFileName;
    const char *in_filename_v = inputH264FileName;
    const char *out_filename = outMP4FileName;//Output file URL
    //Input
    if ((ret = avformat_open_input(&ifmt_ctx_v, in_filename_v, nullptr, nullptr)) < 0) {
        printf( "Could not open input file.");
        return -1;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx_v, nullptr)) < 0) {
        printf( "Failed to retrieve input stream information");
        return -1;
    }
//#if 1
//    AVInputFormat* aac_format = av_find_input_format("aac");
//    if (aac_format == NULL) {
//        printf("AAC format not found\n");
//        return -1;
//    }

//    AVFormatContext* format_ctx = avformat_alloc_context();
//    if (format_ctx == NULL) {
//        printf("Could not allocate format context\n");
//        return -1;
//    }

////    if (avformat_open_input(&format_ctx, "your_aac_file.aac", aac_format, NULL) != 0) {
////        printf("Could not open AAC file\n");
////        return -1;
////    }
//#endif
    if ((ret = avformat_open_input(&ifmt_ctx_a, in_filename_a, nullptr, nullptr)) < 0) {
        printf( "Could not open input file.");
        return -1;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx_a, 0)) < 0) {
        printf( "Failed to retrieve input stream information");
        return -1;
    }
    printf("===========Input Information==========\n");
    av_dump_format(ifmt_ctx_v, 0, in_filename_v, 0);
    av_dump_format(ifmt_ctx_a, 0, in_filename_a, 0);
    printf("======================================\n");
    //Output
    avformat_alloc_output_context2(&ofmt_ctx, nullptr, nullptr, out_filename);
    if (!ofmt_ctx) {
        printf( "Could not create output context\n");
        return -1;
    }
    //ofmt = ofmt_ctx->oformat;
    ofmt = (AVOutputFormat *)ofmt_ctx->oformat;
    for (i = 0; i < ifmt_ctx_v->nb_streams; i++) {
            //Create output AVStream according to input AVStream
            if(ifmt_ctx_v->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO){
            AVStream *in_stream = ifmt_ctx_v->streams[i];
            const AVCodec * pCodec_v = avcodec_find_decoder(in_stream->codecpar->codec_id);
            AVStream *out_stream = avformat_new_stream(ofmt_ctx, pCodec_v);
            videoindex_v=i;
            if (!out_stream) {
                printf( "Failed allocating output stream\n");
                return -1;
            }
            videoindex_out=out_stream->index;
            AVCodecContext * pCodecCtx = avcodec_alloc_context3(pCodec_v);
            //Copy the settings of AVCodecContext
            if (avcodec_parameters_to_context(pCodecCtx,in_stream->codecpar)< 0) {
                printf( "Failed to copy context from input to output stream codec context\n");
                return -1;
            }
            out_stream->codecpar->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            {
                pCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            }
            break;
        }
    }


    for (i = 0; i < ifmt_ctx_a->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        if(ifmt_ctx_a->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
            AVStream *in_stream = ifmt_ctx_a->streams[i];
            const AVCodec * pCodec_a = avcodec_find_decoder(in_stream->codecpar->codec_id);
            AVStream *out_stream = avformat_new_stream(ofmt_ctx, pCodec_a);
            audioindex_a=i;
            if (!out_stream) {
                printf( "Failed allocating output stream\n");
                return -1;
            }
            audioindex_out=out_stream->index;
            AVCodecContext * pCodecCtx = avcodec_alloc_context3(pCodec_a);
            if (avcodec_parameters_to_context(pCodecCtx,in_stream->codecpar)< 0) {
                printf( "Failed to copy context from input to output stream codec context\n");
                return -1;
            }
            out_stream->codecpar->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            {
                pCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            }
            break;
        }
    }

    printf("==========Output Information==========\n");
    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    printf("======================================\n");
    //Open output file
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        if (avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE) < 0) {
            printf( "Could not open output file '%s'", out_filename);
            return -1;
        }
    }
    //Write file header
    if (avformat_write_header(ofmt_ctx, NULL) < 0) {
        printf( "Error occurred when opening output file\n");
        return -1;
    }


    //FIX
#if USE_H264BSF
    AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
#endif
#if USE_AACBSF
    AVBitStreamFilterContext* aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");
#endif

    while (1) {
        AVFormatContext *ifmt_ctx;
        int stream_index=0;
        AVStream *in_stream, *out_stream;

        //Get an AVPacket
        if(av_compare_ts(cur_pts_v,ifmt_ctx_v->streams[videoindex_v]->time_base,cur_pts_a,ifmt_ctx_a->streams[audioindex_a]->time_base) <= 0){
            ifmt_ctx=ifmt_ctx_v;
            stream_index=videoindex_out;

            if(av_read_frame(ifmt_ctx, &pkt) >= 0){

                    in_stream  = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if(pkt.stream_index==videoindex_v){
                        //FIX：No PTS
                        //Simple Write PTS
                        if(pkt.pts==AV_NOPTS_VALUE){
                            //Write PTS
                            AVRational time_base1=in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts=pkt.pts;
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_v=pkt.pts;
                        printf("VIDEO  ");
                    }
            }else
            {
                break;
            }
        }else{
            ifmt_ctx=ifmt_ctx_a;
            stream_index=audioindex_out;
            if(av_read_frame(ifmt_ctx, &pkt) >= 0){

                    in_stream  = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if(pkt.stream_index==audioindex_a){
                        //FIX：No PTS
                        //Simple Write PTS
                        if(pkt.pts==AV_NOPTS_VALUE){
                            //Write PTS
                            AVRational time_base1=in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts=pkt.pts;
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_a=pkt.pts;

                        printf("AUDIO  ");
                    }
            }else
            {
                break;
            }

        }

        //FIX:Bitstream Filter
#if USE_H264BSF
        av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
#if USE_AACBSF
        av_bitstream_filter_filter(aacbsfc, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        pkt.stream_index=stream_index;
        printf("Write 1 Packet. size:%5d\tpts:%lld\n",pkt.size,pkt.pts);
        //Write
        if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {
            printf( "Error muxing packet\n");
            break;
        }
        av_packet_unref(&pkt);

    }
    //Write file trailer
    av_write_trailer(ofmt_ctx);

#if USE_H264BSF
    av_bitstream_filter_close(h264bsfc);
#endif
#if USE_AACBSF
    av_bitstream_filter_close(aacbsfc);
#endif


    avformat_close_input(&ifmt_ctx_v);
    avformat_close_input(&ifmt_ctx_a);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF) {
        printf( "Error occurred.\n");
        return -1;
    }
    return 0;
}



#endif


#if 1
extern "C"
{
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libavcodec/bsf.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libavutil/log.h"
#include "libavutil/imgutils.h"
#include "libavutil/audio_fifo.h"
#include <libswresample/swresample.h>
#include "libavutil/avstring.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libswscale/swscale.h"
}
/*
FIX: H.264 in some container format (FLV, MP4, MKV etc.) need
"h264_mp4toannexb" bitstream filter (BSF)
  *Add SPS,PPS in front of IDR frame
  *Add start code ("0,0,0,1") in front of NALU
H.264 in some container (MPEG2TS) don't need this BSF.
*/
//'1': Use H.264 Bitstream Filter
//'0': do not Use H.264 Bitstream Filter
#define USE_H264BSF 0

/*
FIX:AAC in some container format (FLV, MP4, MKV etc.) need
"aac_adtstoasc" bitstream filter (BSF)
*/
//'1': Use AAC Bitstream Filter
//'0': do not Use AAC Bitstream Filter
#define USE_AACBSF 0

inline int muxer_main(char *inputH264FileName,char *inputAacFileName,char *outMP4FileName)
{
    AVOutputFormat *ofmt = nullptr;
    //Input AVFormatContext and Output AVFormatContext
    AVFormatContext *ifmt_ctx_v = nullptr, *ifmt_ctx_a = nullptr,*ofmt_ctx = nullptr;
    AVPacket pkt;
    int ret, i;
    int videoindex_v=-1,videoindex_out=-1;
    int audioindex_a=-1,audioindex_out=-1;
    int frame_index=0;
    int64_t cur_pts_v=0,cur_pts_a=0;

    const char *in_filename_a = inputAacFileName;
    const char *in_filename_v = inputH264FileName;
    const char *out_filename = outMP4FileName;//Output file URL
    //av_register_all();

    //Input
    if ((ret = avformat_open_input(&ifmt_ctx_v, in_filename_v, nullptr, nullptr)) < 0) {
        printf( "Could not open input file h264.");
        return -1;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx_v, nullptr)) < 0) {
        printf( "Failed to retrieve input stream information");
        return -1;
    }
#if 1
    const AVInputFormat* aac_format = av_find_input_format("aac");
    if (aac_format == NULL) {
        printf("AAC format not found\n");
        return -1;
    }

    AVFormatContext* format_ctx = avformat_alloc_context();
    if (format_ctx == NULL) {
        printf("Could not allocate format context\n");
        return -1;
    }

//    if (avformat_open_input(&format_ctx, "your_aac_file.aac", aac_format, NULL) != 0) {
//        printf("Could not open AAC file\n");
//        return -1;
//    }
#endif
    if ((ret = avformat_open_input(&ifmt_ctx_a, in_filename_a, aac_format, nullptr)) < 0) {
        printf( "Could not open input file aac.");
        return -1;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx_a, 0)) < 0) {
        printf( "Failed to retrieve input stream information");
        return -1;
    }
    printf("===========Input Information==========\n");
    av_dump_format(ifmt_ctx_v, 0, in_filename_v, 0);
    av_dump_format(ifmt_ctx_a, 0, in_filename_a, 0);
    printf("======================================\n");
    //Output
    avformat_alloc_output_context2(&ofmt_ctx, nullptr, nullptr, out_filename);
    if (!ofmt_ctx) {
        printf( "Could not create output context\n");
        return -1;
    }
    //ofmt = ofmt_ctx->oformat;
    ofmt = (AVOutputFormat *)ofmt_ctx->oformat;
    for (i = 0; i < ifmt_ctx_v->nb_streams; i++) {
            //Create output AVStream according to input AVStream
            if(ifmt_ctx_v->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO){
            AVStream *in_stream = ifmt_ctx_v->streams[i];
            const AVCodec * pCodec_v = avcodec_find_decoder(in_stream->codecpar->codec_id);
            AVStream *out_stream = avformat_new_stream(ofmt_ctx, pCodec_v);
            videoindex_v=i;
            if (!out_stream) {
                printf( "Failed allocating output stream\n");
                return -1;
            }
            videoindex_out=out_stream->index;
            AVCodecContext * pCodecCtx = avcodec_alloc_context3(pCodec_v);
            //Copy the settings of AVCodecContext
            if (avcodec_parameters_to_context(pCodecCtx,in_stream->codecpar)< 0) {
                printf( "Failed to copy context from input to output stream codec context\n");
                return -1;
            }
        //writeheader出问题加了这一段-------
            if (avcodec_parameters_from_context(out_stream->codecpar,pCodecCtx)) {
                printf( "Failed to copy context from input to output stream codec context\n");
                return -1;
            }
            out_stream->codecpar->codec_tag = 0;
            out_stream->codecpar->codec_type=AVMEDIA_TYPE_VIDEO;
            printf( "codecpar width:%d codecpar height:%d",in_stream->codecpar->width,in_stream->codecpar->height);

            out_stream->codecpar->width=in_stream->codecpar->width;
            out_stream->codecpar->height=in_stream->codecpar->height;

            printf( "out codecpar width:%d codecpar height:%d",out_stream->codecpar->width,out_stream->codecpar->height);

        //-----------------一直到这
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            {
                pCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            }
            break;
        }
    }


    for (i = 0; i < ifmt_ctx_a->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        if(ifmt_ctx_a->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
            AVStream *in_stream = ifmt_ctx_a->streams[i];
            const AVCodec * pCodec_a = avcodec_find_decoder(in_stream->codecpar->codec_id);
            AVStream *out_stream = avformat_new_stream(ofmt_ctx, pCodec_a);
            audioindex_a=i;
            if (!out_stream) {
                printf( "Failed allocating output stream\n");
                return -1;
            }
            audioindex_out=out_stream->index;
            AVCodecContext * pCodecCtx = avcodec_alloc_context3(pCodec_a);
            if (avcodec_parameters_to_context(pCodecCtx,in_stream->codecpar)< 0) {//从流给编码器赋值编码参数
                printf( "Failed to copy context from input to output stream codec context\n");
                return -1;
            }
  //writeheader出问题加了这一段-------
            //avcodec_parameters_from_context         从编码器给流赋值编码参数
            if (avcodec_parameters_from_context(out_stream->codecpar,pCodecCtx)) {
                printf( "Failed to copy context from input to output stream codec context\n");
                return -1;
            }
            //in_stream->codecpar->width
            out_stream->codecpar->codec_type=AVMEDIA_TYPE_AUDIO;
            out_stream->codecpar->codec_tag = 0;
            out_stream->codecpar->sample_rate=in_stream->codecpar->sample_rate;
   //_____________________到这
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            {
                pCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            }
            break;
        }
    }

    printf("==========Output Information==========\n");
    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    printf("======================================\n");
    //Open output file
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        if (avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE) < 0) {
            printf( "Could not open output file '%s'", out_filename);
            return -1;
        }
    }
    //Write file header
    //ofmt_ctx->oformat->flags |= AVFMT_NODIMENSIONS;
    if (avformat_write_header(ofmt_ctx, NULL) < 0) {
        printf( "Error occurred when opening output file\n");
        return -1;
    }


    //FIX
#if USE_H264BSF
    AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
#endif
#if USE_AACBSF
    AVBitStreamFilterContext* aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");
#endif

    while (1) {
        AVFormatContext *ifmt_ctx;
        int stream_index=0;
        AVStream *in_stream, *out_stream;

        //Get an AVPacket
        if(av_compare_ts(cur_pts_v,ifmt_ctx_v->streams[videoindex_v]->time_base,cur_pts_a,ifmt_ctx_a->streams[audioindex_a]->time_base) <= 0){
            ifmt_ctx=ifmt_ctx_v;
            stream_index=videoindex_out;

            if(av_read_frame(ifmt_ctx, &pkt) >= 0){

                    in_stream  = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if(pkt.stream_index==videoindex_v){
                        //FIX：No PTS
                        //Simple Write PTS
                        if(pkt.pts==AV_NOPTS_VALUE){
                            //Write PTS
                            AVRational time_base1=in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts=pkt.pts;
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_v=pkt.pts;
                        printf("VIDEO  ");
                    }
            }else
            {
                break;
            }
        }else{
            ifmt_ctx=ifmt_ctx_a;
            stream_index=audioindex_out;
            if(av_read_frame(ifmt_ctx, &pkt) >= 0){

                    in_stream  = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if(pkt.stream_index==audioindex_a){
                        //FIX：No PTS
                        //Simple Write PTS
                        if(pkt.pts==AV_NOPTS_VALUE){
                            //Write PTS
                            AVRational time_base1=in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts=pkt.pts;
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_a=pkt.pts;

                        printf("AUDIO  ");
                    }
            }else
            {
                break;
            }

        }

        //FIX:Bitstream Filter
#if USE_H264BSF
        av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
#if USE_AACBSF
        av_bitstream_filter_filter(aacbsfc, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        pkt.stream_index=stream_index;
        printf("Write 1 Packet. size:%5d\tpts:%lld\n",pkt.size,pkt.pts);
        //Write
        if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {
            printf( "Error muxing packet\n");
            break;
        }
        av_packet_unref(&pkt);

    }
    //Write file trailer
    av_write_trailer(ofmt_ctx);

#if USE_H264BSF
    av_bitstream_filter_close(h264bsfc);
#endif
#if USE_AACBSF
    av_bitstream_filter_close(aacbsfc);
#endif


    avformat_close_input(&ifmt_ctx_v);
    avformat_close_input(&ifmt_ctx_a);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF) {
        printf( "Error occurred.\n");
        return -1;
    }
    return 0;
}



#endif

#endif //MIX_H
