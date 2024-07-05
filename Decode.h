#ifndef DECODE_H
#define DECODE_H
extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include <mutex>

class Decode
{
public:
    Decode();
    virtual ~Decode();

    //打开解码器（视频和音频通用）,并释放para空间(是从Demux获取到的para)
    virtual bool Open(AVCodecParameters *para); //success return ture
    //发送待解码数据，并释放pkt空间
    virtual bool Send(AVPacket *pkt);
    //获取解码数据，一次Send可能需要多次Recv，获取缓冲中的数据
    //每次复制一份frame，由外部释放 av_frame_free
    virtual AVFrame* Recv();
    //清理解码器缓冲
    virtual void Clear();
    //关闭解码器
    virtual void Close();

    //音视频同步 当前解码到的pts
    long long pts = 0;
private:
    std::mutex mux;
    //解码器上下文
    AVCodecContext *code_ctx=NULL;
};

#endif // DECODE_H
