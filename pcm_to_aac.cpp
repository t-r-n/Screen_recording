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

	//���ñ�����������
	ac = avcodec_alloc_context3(codec);
	if (!ac) {
		std::cout << "avcodec_alloc_context3 failed" << std::endl;
		return;
	}
	ac->sample_rate = samplerate_;//������ //ÿ���ӵĲ���������Ƶ��ÿ���Ӳɼ��˶��ٵ��źţ�
	ac->channels = 2;//������
	ac->sample_fmt = AV_SAMPLE_FMT_FLTP;//�������ڴ��д洢��ʽfltp�Ǹ�����ռ���ֽڣ�����һ������Ҫ���ֽڴ洢
	ac->channel_layout = AV_CH_LAYOUT_STEREO;
	ac->bit_rate = 64000;//��λʱ���ڴ��ͱ��ص���Ŀ
	ac->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	//����Ƶ�������
	ret = avcodec_open2(ac, codec, NULL);
	if (ret < 0) {

		std::cout << "avcodec_open2 failed" << std::endl;
		return;
	}

	//����һ������������ģ����������ʽ��
	oc = NULL;
	avformat_alloc_output_context2(&oc, NULL, NULL, outputfile);
	if (!oc) {
		std::cout << "avformat_alloc_output_context3_failed" << std::endl;
		return;
	}
	AVStream* st = avformat_new_stream(oc, NULL);//��Ƶ��Ƶ��Ļ����������ʽ����Ƶ�±�ͨ����0
	st->codecpar->codec_tag = 0;
	avcodec_parameters_from_context(st->codecpar, ac);
	av_dump_format(oc, 0, outputfile, 1);//��������Ϣ��ӡһ��


	ret = avio_open(&oc->pb, outputfile, AVIO_FLAG_WRITE);//��д��ķ�ʽ���ļ�
	if (ret < 0) {
		std::cout << "avio_open failed" << std::endl;
		return;
	}

}

void pcm_to_aac::start() {
	/////���ϵ����ù���׼�����
	int ret = 0;
	avformat_write_header(oc, NULL);

	SwrContext* ctx = NULL;//�����ز�����������Ƶ�Ĳ����������Ƶ�Ĳ������е���
	ctx = swr_alloc_set_opts(ctx, ac->channel_layout, ac->sample_fmt, ac->sample_rate, //�������Ƶ����
		AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, samplerate_, //�������Ƶ����
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
	frame->nb_samples = 1024; //һ֡��Ƶ����������
	ret = av_frame_get_buffer(frame, 0);
	if (ret < 0) {
		std::cout << "av_frame_get_buffer failed" << std::endl;
		return;
	}
#ifdef TRDEBUG
    qDebug()<<"error on"<<__LINE__<<endl;
#endif
	int readsize = frame->nb_samples * 2 * 2;//s16��ʽ�����ֽ�*��������*2��ͨ������һ֡����ռ�����ڴ�
	char* pcms = new char[readsize];
	//��������ļ�
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
		const uint8_t* data[1];//�ز���֮ǰ������
		data[0] = (uint8_t*)pcms;
		len = swr_convert(ctx, frame->data, frame->nb_samples,//�ز���֮�������
			data, frame->nb_samples /*��Ƶ��������*/ //�ز���֮ǰ������
		);
        //std::cout << __LINE__ << len << std::endl;
		if (len <= 0) {
			break;
		}
#ifndef TRDEBUG
    qDebug()<<"error on"<<__LINE__<<endl;
#endif
		//���ز���֮������ݷ��͵������߳�
		ret = avcodec_send_frame(ac, frame);
		if (ret != 0) {
			continue;
		}
		AVPacket pkt;
		av_init_packet(&pkt);
		ret = avcodec_receive_packet(ac, &pkt);//���ձ���������
		if (ret != 0) {
			continue;
		}
		pkt.stream_index = 0;
		pkt.dts = 0;
		pkt.pts = 0;
		av_interleaved_write_frame(oc, &pkt);

	}
	std::cout << "�ѽ���" << std::endl;

	delete[] pcms;
	pcms = NULL;
	//д����Ƶ������
	av_write_trailer(oc);
}
pcm_to_aac::~pcm_to_aac() {
	//�رմ��ļ���io��
	avio_close(oc->pb);
	//�رձ�����
	avcodec_close(ac);
	avcodec_free_context(&ac);//�رձ�������������������ò���

}
