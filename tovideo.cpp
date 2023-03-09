#include "tovideo.h"


ToVideo::ToVideo(std::wstring file_name,std::string codecname,int wid,int hei,int fps,float bitrate)
{
    uint8_t endcode1[] ={ 0, 0, 1, 0xb7 };
    memcpy(endcode,&endcode1,sizeof(endcode1));

    filename = file_name.c_str();
    codec_name = codecname.c_str();
    //avcodec_get_name(codec_name)
    /* find the mpeg1video encoder */
    codec = avcodec_find_encoder_by_name(codec_name);
    if (!codec) {
        fprintf(stderr, "Codec '%s' not found\n", codec_name);
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);

    /* put sample parameters */
    int mbps=1000000;
    c->bit_rate = bitrate*mbps;
    /* resolution must be a multiple of two */
    c->width = wid;
    c->height = hei;
    /* frames per second */
    c->time_base = (AVRational){1, fps};
    c->framerate = (AVRational){fps, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);

    /* open it */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
        exit(1);
    }
    //f = _wfopen(filename, "wb");
    f=_wfopen(filename,L"wb");
    //f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;
    frame->pts=0;
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }
}
void ToVideo::sendFrame(const QImage &image,std::vector<std::vector<std::string>>&ve){
    ret = av_frame_make_writable(frame);
    if (ret < 0)
        exit(1);
#if 0
    if(pix.isNull()){
        qDebug()<<__LINE__<<"QPix错误"<<endl;
    }
    QImage image=pix.toImage();
    if(image.isNull()){
        if(pix.isNull()){
            qDebug()<<"QPix错误"<<endl;
        }
        qDebug()<<"Qimage错误"<<endl;
    }
#endif
    if(image.isNull()){
        qDebug()<<__LINE__<<"QPix错误"<<endl;
    }
    //QImage image=pix.toImage();
//    if(image.isNull()){
//        if(image.isNull()){
//            qDebug()<<"QPix错误"<<endl;
//        }
//        qDebug()<<"Qimage错误"<<endl;
//    }
    int cc=0;
    //image.bits();
#if 1
    for (int h = 0; h < c->height; h++)
    {

        for (int w = 0; w < c->width; w++)
        {
            int r,g,b;
            QRgb rgb;
            if(h<gy||h>=(gy+gh)||w>=(gx+gw)||w<gx){//超过矩形框，黑色填充
                r=0;
                g=0;
                b=0;
            }else{
                rgb = image.pixel(w-gx, h-gy);
                if(((int)rgb)==12345){
                    if(image.isNull()){
                        qDebug()<<"图片为空"<<endl;
                        qDebug()<<image.width()<<image.height()<<endl;
                    }
                    //qDebug()<<"图片宽高："<<image.width()<<" "<<image.height()<<endl;//游戏界面下如果不是原来的桌面分辨率，会出问题
                    //假如桌面分辨率是1920*1080，游戏里是1600*800就会出超出范围的错误，看是重新设置解码分辨率还是怎么搞
                    //qDebug()<<"报出范围的错误"<<endl;
                    r=0;
                    g=0;
                    b=0;
                }else{
                    //qDebug()<<"图片宽高："<<image.width()<<" "<<image.height()<<endl;
                    r = qRed(rgb);
                    g = qGreen(rgb);
                    b = qBlue(rgb);
                }

            }

//            int dy = ((66*r + 129*g + 25*b) >> 8) + 16;
//            int du = ((-38*r + -74*g + 112*b) >> 8) + 128;
//            int dv = ((112*r + -94*g + -18*b) >> 8) + 128;

            int dy= ((r << 6) + (r << 3) + (r << 2) + r + (g << 7) + (g << 4) + (g << 2) + (g << 1) + (b << 4) + (b << 3) + (b << 2) + b) >> 8;
            int du= (-((r << 5) + (r << 3) + (r << 1)+ r) - ((g << 6) + (g << 4) + (g << 2)+g) + (b << 7) + 32768) >> 8;
            int dv= ((r << 7) - ((g << 6) + (g << 5) + (g << 3) + (g << 3) + g) - ((b << 4) + (b << 2) + b) + 32768 )>> 8;



            uchar yy = (uchar)dy;
            uchar uu = (uchar)du;
            uchar vv = (uchar)dv;

            frame->data[0][h * frame->linesize[0] + w] = yy;

            if(h % 2 == 0 && w % 2 == 0)
            {
                frame->data[1][h/2 * (frame->linesize[1]) + w/2] = uu;
                frame->data[2][h/2 * (frame->linesize[2]) + w/2] = vv;
            }
           //cpu跑到%30需要500ms，那这个没法降cpu啊，卧槽在一秒内要把200mb的数据压缩完，需要500ms，否则就要积压内存，明天考虑不要截取这么大的数据
            //std::this_thread::yield();
            //std::this_thread::sleep_for(std::chrono::microseconds(1000));
            //qDebug()<<"1毫秒"<<endl;
//            std::condition_variable cv;
//            std::mutex mtx;
//            std::unique_lock<std::mutex> lck(mtx);
//            cv.wait_for(lck,std::chrono::microseconds(500));
        }
        //std::this_thread::yield();
                    if(++cc==10){//把一帧的压缩控制在一秒左右
                        cc=0;
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }

        //std::this_thread::yield();
    }
#endif
#if 0
    //2 创建视频重采样上下文：指定源和目标图像分辨率、格式
    //qDebug()<<"图片格式"<<image.format()<<endl;
        SwsContext *swsCtx = NULL;
        swsCtx = sws_getCachedContext(swsCtx,
            c->width, c->height, AV_PIX_FMT_RGB32,
            c->width, c->height, AV_PIX_FMT_YUV420P,
            SWS_BICUBIC,
            NULL, NULL, NULL
            );
    auto rgbData=image.bits();
    //5 创建RGB视频帧并绑定RGB缓冲区（avpicture_fill是给rgbFrame初始化一些字段，并且会自动填充data和linesize）
    AVFrame *rgbFrame = av_frame_alloc();
    av_image_fill_arrays(rgbFrame->data,rgbFrame->linesize,rgbData,AV_PIX_FMT_RGB32,c->width, c->height,1);
    //av_image_fill_arrays(rgbFrame,rgbData, AV_PIX_FMT_RGB24, c->width, c->height);
    //6 像素格式转换，转换后的YUV数据存放在yuvFrame
    int outSliceH = sws_scale(swsCtx, rgbFrame->data, rgbFrame->linesize, 0, c->height,
        frame->data, frame->linesize);
    if (outSliceH <= 0){
        qDebug()<<"转换失败"<<endl;
        return;
    }
#endif
    frame->pts++;//设置时间戳保证每一帧图片的顺序
    encode(c, frame, pkt, f,ve);
}
void ToVideo::flushEncode(){
    std::vector<std::vector<std::string>>a;
    encode(c, NULL, pkt, f,a);
}

void ToVideo::encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,FILE *outfile,std::vector<std::vector<std::string>>&ve1)
{
    int ret;

    /* send the frame to the encoder */
    if (frame)
        printf("Send frame %3" PRId64"\n", frame->pts);

    ret = avcodec_send_frame(enc_ctx, frame);//把帧送到编码器编码
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }
//在这个地方加锁，多个线程先压缩，在写文件的地方加锁
    std::vector<std::string>ve(3,std::string());
    ve.clear();
    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break; //从这边退出循环把每一帧数据到数组里然后放到新线程池，让当前线程赶紧返回，等待顺序写入文件在新线程池中顺序进行
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }
//while循环到此为止，把pkt压到数组中，用表示同步顺序线程卡在这个位置就行，按顺序唤醒
        //printf("Write packet %3" PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        //fwrite(pkt->data, 1, pkt->size, outfile);
        std::string ss((char*)pkt->data,pkt->size);
        //qDebug()<<"ss size:"<<ss.size()<<"pkt size:"<<pkt->size<<endl;
        ve.emplace_back(std::move(ss));
        av_packet_unref(pkt);
    }
    ve1.emplace_back(std::move(ve));
}
void ToVideo::writeFile(std::shared_ptr<std::vector<std::vector<std::string>>>ve){
    //qDebug()<<"写入的帧数"<<ve->size()<<"line:"<<__LINE__<<endl;
    for(std::size_t i=0;i<ve->size();++i){
        //qDebug()<<"第"<<i<<"帧的包数:"<<(*ve)[i].size()<<endl;//每一帧就一个包
        for(std::size_t j=0;j<(*ve)[i].size();++j){
            fwrite((unsigned char*)(*ve)[i][j].c_str(), 1, (*ve)[i][j].size(), f);
        }
    }
}
void ToVideo::setFrames(int fps){
    c->time_base = (AVRational){1, fps};
    c->framerate = (AVRational){fps, 1};
}

ToVideo::~ToVideo(){
    std::vector<std::vector<std::string>>a;
    encode(c, NULL, pkt, f,a);

    /* Add sequence end code to have a real MPEG file.
       It makes only sense because this tiny examples writes packets
       directly. This is called "elementary stream" and only works for some
       codecs. To create a valid file, you usually need to write packets
       into a proper file format or protocol; see muxing.c.
     */
    if (codec->id == AV_CODEC_ID_MPEG1VIDEO || codec->id == AV_CODEC_ID_MPEG2VIDEO)
        fwrite(endcode, 1, sizeof(endcode), f);
    fclose(f);

    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    qDebug()<<"析构函数调用"<<endl;
}
