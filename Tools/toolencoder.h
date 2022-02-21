#ifndef TOOLENCODER_H
#define TOOLENCODER_H


#include <opencv2/core/core.hpp>//OpenCV包含头文件
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <string.h>//字符串操作头文件

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#ifdef __cplusplus
}
#endif
using namespace cv;

class ToolEnCoder
{
public:
    AVFrame *m_pYUVFrame;	//帧对象
    AVCodec *pCodecH264;	//编码器
    AVCodecContext *context;		//编码器数据结构对象
    uint8_t *yuv_buff;      //yuv图像数据区
    uint8_t *rgb_buff;		//rgb图像数据区
    SwsContext *scxt;		//图像格式转换对象
    uint8_t *outbuf;		//编码出来视频数据缓存
    int outbuf_size;        //编码输出数据去大小
    int nDataLen;			//rgb图像数据区长度
    int width;				//输出视频宽度
    int height;				//输出视频高度
    AVPacket pkt;			//数据包对象
    int imgwidthlen;		//图像宽度占用空间
public:
    void ToolEnCoder_Init();//初始化
    void ToolEnCoder_Setpara(AVCodecID mycodeid, int vwidth, int vheight);//设置参数,第一个参数为编码器,第二个参数为压缩出来的视频的宽度，第三个视频则为其高度
    void ToolEnCoder_Encode(FILE *file, uint8_t *data);//编码并写入数据到文件
    void ToolEnCoder_Close();//关闭
};


#endif // TOOLENCODER_H
