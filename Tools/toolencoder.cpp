#include "toolencoder.h"
void ToolEnCoder::ToolEnCoder_Init()
{

    av_register_all();
    avcodec_register_all();
    m_pYUVFrame = new AVFrame[1];//YUV帧数据赋值
    context = nullptr;//解码器指针对象赋初值
}

void ToolEnCoder::ToolEnCoder_Setpara(AVCodecID mycodeid, int vwidth, int vheight)
{
    pCodecH264 = avcodec_find_encoder(mycodeid);//查找h264编码器
    if (!pCodecH264)
    {
        fprintf(stderr, "h264 codec not found\n");
        exit(1);
    }
    width = vwidth;
    height = vheight;

    context = avcodec_alloc_context3(pCodecH264);//函数用于分配一个AVCodecContext并设置默认值，如果失败返回NULL，并可用av_free()进行释放
    context->bit_rate = 1024*1024; //码率，越高视频质量越好
    context->width = vwidth;//设置编码视频宽度
    context->height = vheight; //设置编码视频高度
    context->time_base.den = 25;//设置帧率,num为分子和den为分母，如果是1/25则表示25帧/s
    context->time_base.num = 1;
    context->gop_size = 10; //设置GOP大小,该值表示每10帧会插入一个I帧
    context->max_b_frames = 1;//设置B帧最大数,该值表示在两个非B帧之间，所允许插入的B帧的最大帧数
    context->pix_fmt = AV_PIX_FMT_YUV420P;//设置像素格式

    imgwidthlen = context->width * 3;
    av_opt_set(context->priv_data, "tune", "zerolatency", 0);//设置编码器的延时，解决前面的几十帧不出数据的情况

    if (avcodec_open2(context, pCodecH264, NULL) < 0)//打开编码器
        return;

    nDataLen = vwidth*vheight * 3;//计算图像rgb数据区长度

    yuv_buff = new uint8_t[nDataLen/2];//初始化数据区，为yuv图像帧准备填充缓存
    rgb_buff = new uint8_t[nDataLen];//初始化数据区，为rgb图像帧准备填充缓存
    outbuf_size = 100000;////初始化编码输出数据区
    outbuf = new uint8_t[outbuf_size];
    m_pYUVFrame->width = vwidth;
    m_pYUVFrame->height = vheight;
    scxt = sws_getContext(context->width, context->height, AV_PIX_FMT_BGR24, context->width, context->height, AV_PIX_FMT_YUV420P, SWS_POINT, NULL, NULL, NULL);//初始化格式转换函数
    av_image_fill_arrays(m_pYUVFrame->data, m_pYUVFrame->linesize, yuv_buff, AV_PIX_FMT_YUV420P, width, height, 1);
}

void ToolEnCoder::ToolEnCoder_Encode(FILE *file, uint8_t *data)
{
    memset(&pkt, 0, sizeof(AVPacket));
    av_init_packet(&pkt);

    memcpy(rgb_buff, data, nDataLen);//拷贝图像数据到rgb图像帧缓存中准备处理
    // 将RGB转化为YUV
    sws_scale(scxt, &rgb_buff, &imgwidthlen, 0, context->height, m_pYUVFrame->data, m_pYUVFrame->linesize);

    int myoutputlen = 0;
    int returnvalue = avcodec_encode_video2(context, &pkt, m_pYUVFrame, &myoutputlen);
    if (returnvalue == 0)
    {
        fwrite(pkt.data, 1, pkt.size, file);
    }


}

void ToolEnCoder::ToolEnCoder_Close()
{
    delete[]m_pYUVFrame;
    delete[]rgb_buff;
    delete[]yuv_buff;
    delete[]outbuf;
    sws_freeContext(scxt);
    avcodec_close(context);//关闭编码器
    av_free(context);
}
