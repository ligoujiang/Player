#ifndef DEMUX_H
#define DEMUX_H
extern "C"{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
}

#include <mutex>

class Demux
{
public:
    Demux();
    virtual ~Demux();
    //打开文件和解复用
    virtual bool Open(const char *url);
    //读pkt 空间需要外部释放（Decode里的send释放），释放AVPacket对象空间和数据空间 av_packet_free
    //该函数需要调用多次读取每一帧，所以返回值为AVPacket
    virtual AVPacket* Read();
    //获取视频参数 空间需要外部释放 avcodec_parameters_free
    virtual AVCodecParameters *CopyVPara();
    //获取音频参数 空间需要外部清理 avcodec_parameters_free
    virtual AVCodecParameters *CopyAPara();
    //判断是否是音频
    virtual bool isAudio(AVPacket *pkt);

    //判断是否有音频或者视频
    virtual void StreamIndex(bool &isVideo,bool &isAudio);

    //seek pos 0.0~1.0
    virtual bool Seek(double pos);

    //清空解复用
    virtual void Clear();
    //关闭解复用
    virtual void Close();

    //开放接口
    int getTotal(); //获取总时长
    //总时长(毫秒)
    int totalMs=0;

    //音视频流索引
    int video_index=-1;
    int audio_index=-1;

    //视频参数
    int width=0;
    int height=0;

    //音频参数
    int sampleRate=0;
    int channels=0;

private:
    //流信息
    AVStream *st=nullptr;
    //解封装上下文
    AVFormatContext *fmt_ctx=NULL;

    std::mutex mux;
};

#endif // DEMUX_H

