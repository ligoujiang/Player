#ifndef SWSCALE_H
#define SWSCALE_H

extern "C"{
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "libavcodec/codec_par.h"
}

class Swscale
{
public:
    Swscale();
    virtual ~Swscale();

    //打开并配置颜色空间转换
    virtual bool Open(AVCodecParameters *para);
    //转换视频帧，并释放frame
    virtual AVFrame* Sws(AVFrame *frame);
    //清理
    virtual void Clear();
private:
    SwsContext *sws_ctx;

    AVFrame *frameYUV=nullptr;
    uint8_t *out_buffer=NULL;
    //需要转换的视频尺寸
    int sws_width=0;
    int sws_heigth=0;
};

#endif // SWSCALE_H
