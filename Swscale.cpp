#include "Swscale.h"

Swscale::Swscale() {}

Swscale::~Swscale(){
    Clear();
}

//打开颜色空间转换
bool Swscale::Open(AVCodecParameters *para)
{
    //清理
    Clear();

    //图像格式转换
    sws_width=1280;
    sws_heigth=720;
    sws_ctx=sws_getContext(para->width,para->height,AV_PIX_FMT_YUV420P,
                              sws_width,sws_heigth,AV_PIX_FMT_YUV420P,SWS_BICUBIC,NULL,NULL,NULL);

    //new一个转换的空间
    frameYUV=av_frame_alloc();
    //av_image_get_buffer_size() 根据参数计算视频图像缓冲区大小
    //创建缓冲区大小
    out_buffer=static_cast<uint8_t*>(av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,sws_width,sws_heigth,1)));
    //对申请的缓冲区进行格式化和划分大小
    av_image_fill_arrays(frameYUV->data,frameYUV->linesize,out_buffer,AV_PIX_FMT_YUV420P,sws_width,sws_heigth,1);
}

//转换视频帧，并释放frame
AVFrame* Swscale::Sws(AVFrame *frame)
{
    if(!frame) return NULL;
    sws_scale(sws_ctx,(uint8_t* const*)frame->data,frame->linesize,0,
              frame->height,frameYUV->data,frameYUV->linesize);
    av_frame_free(&frame);
    return frameYUV;
}

//清理资源
void Swscale::Clear()
{
    if(out_buffer) av_free(out_buffer);
    if(frameYUV) av_frame_free(&frameYUV);
}
