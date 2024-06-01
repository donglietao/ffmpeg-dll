#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <signal.h>
#include <stdlib.h>

volatile sig_atomic_t flag = 0;
void handler(int sig)
{
    flag = 1;
}
// 打开输入设备
int open_input_device(AVFormatContext **input_fmt_ctx2,
                      AVStream **in_stream2,
                      AVCodecContext **in_codec_ctx2)
{
    // 定义临时指针，在初始化后赋值给传入的指针
    AVFormatContext *input_fmt_ctx = NULL;
    AVStream *in_stream = NULL;
    AVCodecContext *in_codec_ctx = NULL;
    //----------------------------------输入设备-------------------------------------------//
    // 创建输入设备上下文，这里是桌面采集

    AVInputFormat *input_fmt = av_find_input_format("gdigrab");
    // 设置采集参数
    AVDictionary *options = NULL;
    av_dict_set(&options, "framerate", "8", 0);
    // 打开桌面采集设备
    if (avformat_open_input(&input_fmt_ctx, "desktop", input_fmt, &options) != 0)
    {
        printf("Could not open input device\n");
        return -1;
    }
    av_dict_free(&options);
    // 获取流信息
    if (avformat_find_stream_info(input_fmt_ctx, NULL) < 0)
    {
        printf("Could not find stream information\n");
        return -1;
    }
    // 打印输入设备信息
    av_dump_format(input_fmt_ctx, 0, "desktop", 0);

    // 找到第一个输入视频流
    // AVStream *in_stream = NULL;
    for (int i = 0; i < input_fmt_ctx->nb_streams; i++)
    {
        if (input_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            in_stream = input_fmt_ctx->streams[i];
            break;
        }
    }
    // 判断输入流是否存在
    if (in_stream == NULL)
    {
        printf("No video stream found\n");
        return -1;
    }
    // 输入流的编解码器
    AVCodec *in_codec = avcodec_find_decoder(in_stream->codecpar->codec_id);
    if (!in_codec)
    {
        printf("in_codec not found\n");
        return -1;
    }

    // 输入设备解码器上下文
    // AVCodecContext *in_codec_ctx = avcodec_alloc_context3(in_codec);
    in_codec_ctx = avcodec_alloc_context3(in_codec);
    if (!in_codec_ctx)
    {
        printf("in_codec_ctx Could not allocate video codec context\n");
        return -1;
    }

    // 初始化输入设备解码器上下文
    if (avcodec_parameters_to_context(in_codec_ctx, in_stream->codecpar) < 0)
    {
        printf("in_codec_ctx Could not copy codec parameters\n");
        return -1;
    }
    // 打开输入设备解码器
    if (avcodec_open2(in_codec_ctx, in_codec, NULL) < 0)
    {
        printf("in_codec_ctx Could not open codec\n");
        return -1;
    }

    // 打印输入设备解码器参数
    printf("----------------------------------in_codec param-------------------------------------------\n");
    printf("in_codec width: %d\n", in_codec_ctx->width);
    printf("in_codec height: %d\n", in_codec_ctx->height);
    printf("in_codec codec_id: %s\n", avcodec_get_name(in_codec_ctx->codec_id));
    printf("in_codec pix_fmt: %s\n", av_get_pix_fmt_name(in_codec_ctx->pix_fmt));
    printf("in_codec framerate.num: %d\n", in_codec_ctx->framerate.num);
    printf("in_codec framerate.den: %d\n", in_codec_ctx->framerate.den);
    printf("in_codec time_base.num: %d\n", in_codec_ctx->time_base.num);
    printf("in_codec time_base.den: %d\n", in_codec_ctx->time_base.den);
    printf("in_codec bit_rate.num: %d\n", in_codec_ctx->bit_rate);
    printf("----------------------------------in_stream param-------------------------------------------\n");
    printf("in_stream r_frame_rate.num: %d\n", in_stream->r_frame_rate.num);
    printf("in_stream r_frame_rate.den: %d\n", in_stream->r_frame_rate.den);
    printf("in_stream avg_frame_rate.num: %d\n", in_stream->avg_frame_rate.num);
    printf("in_stream avg_frame_rate.den: %d\n", in_stream->avg_frame_rate.den);
    printf("in_stream time_base.num: %d\n", in_stream->time_base.num);
    printf("in_stream time_base.den: %d\n", in_stream->time_base.den);
    printf("in_stream start_time: %d\n", in_stream->start_time);

    // 赋值给传入的指针,仅赋值地址
    *input_fmt_ctx2 = input_fmt_ctx;
    *in_stream2 = in_stream;
    *in_codec_ctx2 = in_codec_ctx;

    return 0;
}
// 打开输出设备

int open_output_device(AVFormatContext **output_fmt_ctx2,
                       AVStream **out_stream2,
                       AVCodecContext **out_codec_ctx2, AVDictionary *out_options)
{

    // 定义临时指针，在初始化后赋值给传入的指针
    AVFormatContext *output_fmt_ctx = NULL;
    AVStream *out_stream = NULL;
    AVCodecContext *out_codec_ctx = NULL;
    //----------------------------------输出设备-------------------------------------------//
    // 输出文件格式和文件名
    // const char *output_fmt_name = "mp4";
    // const char *output_file = "d:/1.mp4";
    // const char *output_fmt_name = "flv";
    // const char *output_file = "rtmp://139.9.186.204:31935/live/557254322";
    // const char *output_file = "d:/1.flv";
    const char *output_fmt_name = av_dict_get(out_options, "output_fmt_name", NULL, 0)->value;

    // 输出设备上下文
    // AVFormatContext *output_fmt_ctx = NULL;
    AVOutputFormat *output_fmt = av_guess_format(output_fmt_name, NULL, NULL);

    // 打开输出设备
    if (avformat_alloc_output_context2(&output_fmt_ctx, output_fmt, NULL, NULL) < 0)
    {
        printf("Could not open output device\n");
        return -1;
    }

    // 设置输出设备格式
    output_fmt_ctx->oformat = output_fmt;
    // 添加视频流
    // AVStream *out_stream = avformat_new_stream(output_fmt_ctx, NULL);
    out_stream = avformat_new_stream(output_fmt_ctx, NULL);
    out_stream->codecpar->codec_id = AV_CODEC_ID_H264;
    out_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    out_stream->codecpar->format = AV_PIX_FMT_YUV444P;
    out_stream->codecpar->width = atoi(av_dict_get(out_options, "width", NULL, 0)->value);
    out_stream->codecpar->height = atoi(av_dict_get(out_options, "height", NULL, 0)->value);
    out_stream->codecpar->framerate = (AVRational){8, 1};
    // 设置时间基准，一般为帧率的2倍
    out_stream->time_base = (AVRational){1, 8};

    if (!out_stream)
    {
        printf("Could not create output stream\n");
        return -1;
    }
    // 输出流的编解码器参数
    AVCodec *out_codec = avcodec_find_encoder(out_stream->codecpar->codec_id);
    if (!out_codec)
    {
        printf("Output codec not found\n");
        return -1;
    }
    // 输出流编码器上下文
    // AVCodecContext *out_codec_ctx = avcodec_alloc_context3(out_codec);
    out_codec_ctx = avcodec_alloc_context3(out_codec);
    if (!out_codec_ctx)
    {
        printf("Could not allocate output video codec context\n");
        return -1;
    }
    // 初始化输出流编码器上下文
    if (avcodec_parameters_to_context(out_codec_ctx, out_stream->codecpar) < 0)
    {
        printf("Could not copy output codec parameters\n");
        return -1;
    }
    // 设置输出流的编解码器参数
    out_codec_ctx->time_base = out_stream->time_base;
    out_codec_ctx->gop_size = 50;
    out_codec_ctx->keyint_min = 25;
    out_codec_ctx->max_b_frames = 3;
    out_codec_ctx->refs = 3;
    out_codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    // out_codec_ctx->bit_rate = 4000000;
    // out_codec_ctx->rc_min_rate = 4000000;
    // out_codec_ctx->rc_max_rate = 4000000;
    // out_codec_ctx->rc_buffer_size = 4000000;
    // 额外参数
    AVDictionary *out_codec_opts = NULL;
    av_dict_set(&out_codec_opts, "flvflags", "no_sequence_end", 0); // 设置FLV特定的标志
    av_dict_set(&out_codec_opts, "flv_metadata", "1", 0);           // 启用FLV元数据写入
    // 打开编码器
    if (avcodec_open2(out_codec_ctx, out_codec, &out_codec_opts) < 0)
    {
        printf("Could not open output codec\n");
        return -1;
    }
    av_dict_free(&out_codec_opts);
    // 打印输出设备编码码器参数
    printf("----------------------------------out_codec param-------------------------------------------\n");
    printf("out_codec width: %d\n", out_codec_ctx->width);
    printf("out_codec height: %d\n", out_codec_ctx->height);
    printf("out_codec codec_id: %s\n", avcodec_get_name(out_codec_ctx->codec_id));
    printf("out_codec pix_fmt: %s\n", av_get_pix_fmt_name(out_codec_ctx->pix_fmt));
    printf("out_codec framerate.num: %d\n", out_codec_ctx->framerate.num);
    printf("out_codec framerate.den: %d\n", out_codec_ctx->framerate.den);
    printf("out_codec time_base.num: %d\n", out_codec_ctx->time_base.num);
    printf("out_codec time_base.den: %d\n", out_codec_ctx->time_base.den);
    printf("out_codec bit_rate: %d\n", out_codec_ctx->bit_rate);
    printf("out_codec flags: %d\n", out_codec_ctx->flags);
    printf("out_codec profile: %d\n", out_codec_ctx->profile);

    // 赋值给传入的指针
    *output_fmt_ctx2 = output_fmt_ctx;
    *out_stream2 = out_stream;
    *out_codec_ctx2 = out_codec_ctx;

    return 0;
}
// 转码
int transcode(AVFormatContext *input_fmt_ctx,
              AVStream *in_stream,
              AVCodecContext *in_codec_ctx,
              AVFormatContext *output_fmt_ctx,
              AVStream *out_stream,
              AVCodecContext *out_codec_ctx, AVDictionary *ts_options)
{
    const char *output_file = av_dict_get(ts_options, "output_file", NULL, 0)->value;
    // 打印输出设备信息
    av_dump_format(output_fmt_ctx, 0, output_file, 1);
    // 打开输出设备
    if (avio_open(&output_fmt_ctx->pb, output_file, AVIO_FLAG_WRITE) < 0)
    {
        printf("Could not open output device\n");
        return -1;
    }
    out_stream->codecpar->extradata = out_codec_ctx->extradata;
    out_stream->codecpar->extradata_size = out_codec_ctx->extradata_size;
    // 写入头部信息
    if (avformat_write_header(output_fmt_ctx, NULL) < 0)
    {
        printf("Could not write header\n");
        return -1;
    }
    //----------------------------------解码与转换-------------------------------------------//
    // 原始数据包
    AVPacket packet;
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;
    // 原始帧
    AVFrame *frame = av_frame_alloc();
    if (!frame)
    {
        printf("Could not allocate frame\n");
        return -1;
    }
    // 编码后的数据包
    AVPacket enc_packet;
    av_init_packet(&enc_packet);
    enc_packet.data = NULL;
    enc_packet.size = 0;

    // 编码后的帧
    AVFrame *enc_frame = av_frame_alloc();
    if (!enc_frame)
    {
        printf("Could not allocate frame\n");
        return -1;
    }
    // 设置帧信息
    enc_frame->width = out_codec_ctx->width;
    enc_frame->height = out_codec_ctx->height;
    enc_frame->format = out_codec_ctx->pix_fmt;
    av_frame_get_buffer(enc_frame, 0);
    // 格式转换器
    struct SwsContext *sws_ctx = sws_getContext(in_stream->codecpar->width, in_stream->codecpar->height, in_stream->codecpar->format,
                                                out_stream->codecpar->width, out_stream->codecpar->height, out_stream->codecpar->format,
                                                SWS_BILINEAR, NULL, NULL, NULL);
    printf("----------------------------------transcoding-------------------------------------------\n");
    // 初始化一个变量来保存当前的 PTS
    int64_t frame_count = 0;
    int64_t start_pts = in_stream->start_time;
    int64_t start_time = av_gettime();
    while (1)
    {
        if (flag)
        {
            printf("Ctrl+C pressed. Exiting...\n");
            break;
        }
        printf("read frame:%d\n", frame_count);
        int ret = 0;
        // 从输入设备读取数据包
        ret = av_read_frame(input_fmt_ctx, &packet);
        if (ret < 0)
        {
            printf("1:av_read_frame error\n");
            break;
        }
        // 解码数据包为帧
        ret = avcodec_send_packet(in_codec_ctx, &packet);
        if (ret < 0)
        {
            printf("2:in_codec_ctx Error while sending a packet to the decoder: %s\n", av_err2str(ret));
            break;
        }
        // 从解码器中获取帧
        ret = avcodec_receive_frame(in_codec_ctx, frame);
        if (ret < 0)
        {
            printf("3:in_codec_ctx Error while receiving a frame from the decoder: %s\n", av_err2str(ret));
            break;
        }
        // 格式转换
        ret = sws_scale(sws_ctx, (const uint8_t *const *)frame->data, frame->linesize,
                        0, frame->height,
                        enc_frame->data, enc_frame->linesize);
        if (ret < 0)
        {
            printf("4:sws_scale error\n");
            break;
        }
        // 帧统计
        frame_count++;
        // 当前时间
        int64_t current_time = av_gettime();
        // 转换为秒
        double total_time = (current_time - start_time) / 1000000.0;
        double actual_fps = frame_count / total_time;
        // 打印实际帧率
        printf("Actual framerate: %.2f fps\n", actual_fps);

        // 设置帧的时间戳
        enc_frame->pts = av_rescale_q_rnd((frame->pts - start_pts), in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        enc_frame->pkt_dts = enc_frame->pts;

        //  将帧发送到编码器
        ret = avcodec_send_frame(out_codec_ctx, enc_frame);
        if (ret < 0)
        {
            printf("5:out_codec_ctx Error while sending a frame to the encoder: %s\n", av_err2str(ret));
            break;
        }
        // 从编码器中获取数据包
        while (avcodec_receive_packet(out_codec_ctx, &enc_packet) == 0)
        {
            ret = av_interleaved_write_frame(output_fmt_ctx, &enc_packet);
            if (ret < 0)
            {
                printf("7:Error while writing output packet:%s\n", av_err2str(ret));
                break;
            }
        }
        // 释放编码数据包
        av_packet_unref(&enc_packet);
        // 释放解码数据包
        av_packet_unref(&packet);
    }
    // 写入文件尾
    av_write_trailer(output_fmt_ctx);
    // 关闭输出文件
    avio_close(output_fmt_ctx->pb);
    // 释放数据包
    av_packet_free(&packet);
    av_packet_free(&enc_packet);
    // 释放帧
    av_frame_free(&frame);
    av_frame_free(&enc_frame);
    // 释放格式转换器
    sws_freeContext(sws_ctx);
}

__declspec(dllexport) int __cdecl main2(int argc, char const *argv[])
{
    signal(SIGINT, handler);
    // 设置日志级别
    // av_log_set_level(AV_LOG_DEBUG);
    // 注册输入和输出设备
    avdevice_register_all();
    // 初始化网络库
    avformat_network_init();

    int ret = 0;
    // 打开输入设备
    AVFormatContext *input_fmt_ctx = NULL;
    AVCodecContext *in_codec_ctx = NULL;
    AVStream *in_stream = NULL;
    ret = open_input_device(&input_fmt_ctx, &in_stream, &in_codec_ctx);
    if (ret < 0)
    {
        printf("open_input_device error\n");
        goto exit;
        return 0;
    }

    // 打开输出设备
    AVFormatContext *output_fmt_ctx = NULL;
    AVCodecContext *out_codec_ctx = NULL;
    AVStream *out_stream = NULL;
    // 创建一个字典，用于设置输出设备的参数
    AVDictionary *out_options = NULL;
    av_dict_set_int(&out_options, "width", in_stream->codecpar->width, 0);
    av_dict_set_int(&out_options, "height", in_stream->codecpar->height, 0);
    av_dict_set(&out_options, "output_fmt_name", "flv", 0);
    ret = open_output_device(&output_fmt_ctx, &out_stream, &out_codec_ctx, out_options);
    if (ret < 0)
    {
        printf("open_output_device error\n");
        goto exit;
        return 0;
    }

    // 开始转码
    av_dict_set(&out_options, "output_file", "rtmp://139.9.186.204:31935/live/557254322", 0);
    ret = transcode(input_fmt_ctx, in_stream, in_codec_ctx, output_fmt_ctx, out_stream, out_codec_ctx, out_options);
    if (ret < 0)
    {
        printf("transcode error\n");
        goto exit;
        return 0;
    }

exit:
    // 释放编码器上下文
    avcodec_free_context(&in_codec_ctx);
    avcodec_free_context(&out_codec_ctx);
    // 释放输入和输出设备
    avformat_close_input(&input_fmt_ctx);
    avformat_free_context(input_fmt_ctx);
    // 释放输出设备
    avformat_close_input(&output_fmt_ctx);
    avformat_free_context(output_fmt_ctx);
    // 释放网络库
    avformat_network_deinit();
    return 0;
}

__declspec(dllexport) int __cdecl ffmain(int argc, char const *argv[]){
    return main(argc,argv);
}
