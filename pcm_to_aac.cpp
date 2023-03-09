#include"pcm_to_aac.h"
#include<QDebug>


pcm_to_aac::pcm_to_aac(wchar_t* input, std::string output, int samplerate,int channels)
	:inputfile(input),
	outputfile(output.c_str()),
	samplerate_(samplerate),
	channels_(channels)
{
	//av_register_all();
	//avcodec_register_all();

	int ret = 0;
	//const char* inputfile = input;
	//const char* outputfile = output.c_str();
#ifdef TRDEBUG
    qDebug()<<"error on"<<__LINE__<<endl;
#endif
	const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
	if (!codec) {
		std::cout << "avcode_find_encoder failed" << std::endl;
		return;
	}

	//配置编码器上下文
	ac = avcodec_alloc_context3(codec);
	if (!ac) {
		std::cout << "avcodec_alloc_context3 failed" << std::endl;
		return;
	}
	ac->sample_rate = samplerate_;//采样率 //每秒钟的采样数（音频是每秒钟采集了多少电信号）
	ac->channels = 2;//声道数
	ac->sample_fmt = AV_SAMPLE_FMT_FLTP;//采样在内存中存储格式fltp是浮点型占四字节，即采一个样需要四字节存储
	ac->channel_layout = AV_CH_LAYOUT_STEREO;
	ac->bit_rate = 64000;//单位时间内传送比特的数目
	ac->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	//打开音频编解码器
	ret = avcodec_open2(ac, codec, NULL);
	if (ret < 0) {

		std::cout << "avcodec_open2 failed" << std::endl;
		return;
	}

	//创建一个输出的上下文（配置输出格式）
	oc = NULL;
	avformat_alloc_output_context2(&oc, NULL, NULL, outputfile);
	if (!oc) {
		std::cout << "avformat_alloc_output_context3_failed" << std::endl;
		return;
	}
	AVStream* st = avformat_new_stream(oc, NULL);//音频视频字幕都是流的形式，音频下标通常是0
	st->codecpar->codec_tag = 0;
	avcodec_parameters_from_context(st->codecpar, ac);
	av_dump_format(oc, 0, outputfile, 1);//把配置信息打印一下


	ret = avio_open(&oc->pb, outputfile, AVIO_FLAG_WRITE);//已写入的方式打开文件
	if (ret < 0) {
		std::cout << "avio_open failed" << std::endl;
		return;
	}

}

void pcm_to_aac::start() {
	/////以上的配置工作准备完毕
	int ret = 0;
	avformat_write_header(oc, NULL);

	SwrContext* ctx = NULL;//设置重采样对输入音频的参数和输出音频的参数进行导入
	ctx = swr_alloc_set_opts(ctx, ac->channel_layout, ac->sample_fmt, ac->sample_rate, //输出的音频参数
		AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, samplerate_, //输入的音频参数
		0, 0
	);
	if (!ctx) {
		std::cout << "swr_alloc_set_opts failed" << std::endl;
		return;
	}
	ret = swr_init(ctx);
	if (ret < 0) {
		std::cout << "swr_init failed" << std::endl;
		return;
	}

#ifdef TRDEBUG
    qDebug()<<"error on"<<__LINE__<<endl;
#endif
	AVFrame* frame = av_frame_alloc();
	frame->format = AV_SAMPLE_FMT_FLTP;
	frame->channels = 2;
	frame->channel_layout = AV_CH_LAYOUT_STEREO;
	frame->nb_samples = 1024; //一帧音频的样本数量
	ret = av_frame_get_buffer(frame, 0);
	if (ret < 0) {
		std::cout << "av_frame_get_buffer failed" << std::endl;
		return;
	}
#ifdef TRDEBUG
    qDebug()<<"error on"<<__LINE__<<endl;
#endif
	int readsize = frame->nb_samples * 2 * 2;//s16格式两个字节*样本数量*2个通道就是一帧样本占的总内存
	char* pcms = new char[readsize];
	//打开输入的文件
    //std::ifstream infile(inputfile, std::ios::in | std::ios::binary);
    FILE*fi=_wfopen(inputfile,L"rb");
    if (!fi) {
        std::cout << "start error" << std::endl;
	}
	std::cout << __LINE__ << std::endl;
	for (;;) {
        int len = fread(pcms,1, readsize,fi);
		if (len <= 0) {
			std::cout << __LINE__ << len << std::endl;
			break;
		}
        //std::cout << __LINE__ << len << std::endl;
		const uint8_t* data[1];//重采样之前的数据
		data[0] = (uint8_t*)pcms;
		len = swr_convert(ctx, frame->data, frame->nb_samples,//重采样之后的数据
			data, frame->nb_samples /*音频样本数量*/ //重采样之前的数据
		);
        //std::cout << __LINE__ << len << std::endl;
		if (len <= 0) {
			break;
		}
#ifndef TRDEBUG
    qDebug()<<"error on"<<__LINE__<<endl;
#endif
		//将重采样之后的数据发送到编码线程
		ret = avcodec_send_frame(ac, frame);
		if (ret != 0) {
			continue;
		}
		AVPacket pkt;
		av_init_packet(&pkt);
		ret = avcodec_receive_packet(ac, &pkt);//接收编码后的数据
		if (ret != 0) {
			continue;
		}
		pkt.stream_index = 0;
		pkt.dts = 0;
		pkt.pts = 0;
		av_interleaved_write_frame(oc, &pkt);

	}
	std::cout << "已结束" << std::endl;

	delete[] pcms;
	pcms = NULL;
	//写入视频的索引
	av_write_trailer(oc);
}
pcm_to_aac::~pcm_to_aac() {
	//关闭打开文件的io流
	avio_close(oc->pb);
	//关闭编码器
	avcodec_close(ac);
	avcodec_free_context(&ac);//关闭编码器，清理编码器配置参数

}
