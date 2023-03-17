#pragma once
#ifndef PCM2AAC_H
#define PCM2AAC_H




#include <iostream>
#include <cstring>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

int pcmToAac(char*input,char*output) {
//    if (argc != 3) {
//        std::cerr << "Usage: " << argv[0] << " <input file> <output file>" << std::endl;
//        return 1;
//    }

    const char* input_filename = input;
    const char* output_filename = output;

    AVFormatContext* input_format_context = nullptr;
    int ret = avformat_open_input(&input_format_context, input_filename, nullptr, nullptr);
    if (ret < 0) {
        std::cerr << "Failed to open input file: " << av_err2str(ret) << std::endl;
        return 1;
    }

    ret = avformat_find_stream_info(input_format_context, nullptr);
    if (ret < 0) {
        std::cerr << "Failed to retrieve input stream information: " << av_err2str(ret) << std::endl;
        avformat_close_input(&input_format_context);
        return 1;
    }

    AVCodec* codec = nullptr;
    AVCodecParameters* codec_parameters = nullptr;
    int stream_index = av_find_best_stream(input_format_context, AVMEDIA_TYPE_AUDIO, -1, -1,(const AVCodec**) &codec, 0);
    if (stream_index < 0) {
        std::cerr << "Failed to find audio stream in input file" << std::endl;
        avformat_close_input(&input_format_context);
        return 1;
    }
    codec_parameters = input_format_context->streams[stream_index]->codecpar;

    AVCodecContext* codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        std::cerr << "Failed to allocate codec context" << std::endl;
        avformat_close_input(&input_format_context);
        return 1;
    }
    ret = avcodec_parameters_to_context(codec_context, codec_parameters);
    if (ret < 0) {
        std::cerr << "Failed to copy codec parameters to codec context: " << av_err2str(ret) << std::endl;
        avformat_close_input(&input_format_context);
        avcodec_free_context(&codec_context);
        return 1;
    }
    ret = avcodec_open2(codec_context, codec, nullptr);
    if (ret < 0) {
        std::cerr << "Failed to open codec: " << av_err2str(ret) << std::endl;
        avformat_close_input(&input_format_context);
        avcodec_free_context(&codec_context);
        return 1;
    }

    AVFormatContext* output_format_context = nullptr;
    ret = avformat_alloc_output_context2(&output_format_context, nullptr, nullptr, output_filename);
    if (ret < 0) {
        std::cerr << "Failed to allocate output format context: " << av_err2str(ret) << std::endl;
        avcodec_close(codec_context);
        avcodec_free_context(&codec_context);
        avformat_close_input(&input_format_context);
        return 1;
    }

    AVStream* output_stream = avformat_new_stream(output_format_context, nullptr);
    if (!output_stream) {
        std::cerr << "Failed to create new audio stream in output file" << std::endl;
        avcodec_close(codec_context);
        avcodec_free_context(&codec_context);
        avformat_close_input(&input_format_context);
        avformat_free_context(output_format_context);
        return 1;
    }
    ret = avcodec_parameters_copy(output_stream->codecpar, codec_parameters);
    if (ret < 0) {
        std::cerr << "Failed to copy codec parameters to output stream: " << av_err2str(ret) << std::endl;
        avcodec_close(codec_context);
        avcodec_free_context(&codec_context);
        avformat_close_input(&input_format_context);
        avformat_free_context(output_format_context);
        return 1;
    }

    ret = avio_open(&output_format_context->pb, output_filename, AVIO_FLAG_WRITE);
    if (ret < 0) {
        std::cerr << "Failed to open output file: " << av_err2str(ret) << std::endl;
        avcodec_close(codec_context);
        avcodec_free_context(&codec_context);
        avformat_close_input(&input_format_context);
        avformat_free_context(output_format_context);
        return 1;
    }

    ret = avformat_write_header(output_format_context, nullptr);
    if (ret < 0) {
        std::cerr << "Failed to write output file header: " << av_err2str(ret) << std::endl;
        avio_closep(&output_format_context->pb);
        avcodec_close(codec_context);
        avcodec_free_context(&codec_context);
        avformat_close_input(&input_format_context);
        avformat_free_context(output_format_context);
        return 1;
        }
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    while (true) {
        AVFrame* frame = av_frame_alloc();
        if (!frame) {
            std::cerr << "Failed to allocate audio frame" << std::endl;
            break;
        }

        ret = av_read_frame(input_format_context, &pkt);
        if (ret < 0) {
            av_frame_free(&frame);
            break;
        }

        if (pkt.stream_index != stream_index) {
            av_packet_unref(&pkt);
            continue;
        }

        ret = avcodec_send_packet(codec_context, &pkt);
        if (ret < 0) {
            std::cerr << "Error sending packet to codec context: " << av_err2str(ret) << std::endl;
            av_packet_unref(&pkt);
            av_frame_free(&frame);
            break;
        }

        while (ret >= 0) {
            ret = avcodec_receive_frame(codec_context, frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            } else if (ret < 0) {
                std::cerr << "Error receiving frame from codec context: " << av_err2str(ret) << std::endl;
                av_packet_unref(&pkt);
                av_frame_free(&frame);
                break;
            }

            frame->pts = av_rescale_q(frame->pts, input_format_context->streams[stream_index]->time_base, codec_context->time_base);
            frame->pkt_dts = av_rescale_q(frame->pkt_dts, input_format_context->streams[stream_index]->time_base, codec_context->time_base);
            frame->pkt_duration = av_rescale_q(frame->pkt_duration, input_format_context->streams[stream_index]->time_base, codec_context->time_base);

            ret = av_interleaved_write_frame(output_format_context, &pkt);
            if (ret < 0) {
                std::cerr << "Error writing audio frame to file: " << av_err2str(ret) << std::endl;
                av_packet_unref(&pkt);
                av_frame_free(&frame);
                break;
            }

            av_frame_unref(frame);
        }

        av_packet_unref(&pkt);
        av_frame_free(&frame);
    }

    av_write_trailer(output_format_context);

    avformat_close_input(&input_format_context);
    avcodec_close(codec_context);
    avcodec_free_context(&codec_context);
    avformat_free_context(output_format_context);

    return 0;

}




#endif // PCM2AAC_H
