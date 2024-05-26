#include "Decode.h"
#include <QDebug>

Decode::Decode() {}

Decode::~Decode() {}

//打开解码器（视频和音频通用）,并释放para空间(是从Demux获取到的para)
bool Decode::Open(AVCodecParameters *para)
{
    //每次打开先关闭
    Close();
    pts = 0;

    //查找解码器
    const AVCodec *codec=avcodec_find_decoder(para->codec_id); //根据id查找解码器
    if(!codec){
        avcodec_parameters_free(&para);
        qDebug()<<"find vcodec id failed!";
        return false;
    }
    //分配解码器上下文空间
    code_ctx=avcodec_alloc_context3(codec);
    //配置解码器上下文
    avcodec_parameters_to_context(code_ctx,para);
    //用完并释放para空间
    avcodec_parameters_free(&para);
    //打开解码器上下文
    int ret=avcodec_open2(code_ctx,codec,0);
    if(ret!=0){
        avcodec_free_context(&code_ctx);
        qDebug()<<"avcodec_open2 failed!";
        return false;
    }
    return true;
}

//发送待解码数据，并释放pkt空间
bool Decode::Send(AVPacket *pkt)
{
    int ret=avcodec_send_packet(code_ctx,pkt);  //解码成功，保存到解码器里
    av_packet_free(&pkt); //解码完释放pkt空间
    if(ret!=0) return false;
    return true;
}

//获取解码数据，一次Send可能需要多次Recv，获取缓冲中的数据
//每次复制一份frame，由外部释放 av_frame_free
AVFrame *Decode::Recv()
{
    AVFrame *frame=av_frame_alloc();
    int ret=avcodec_receive_frame(code_ctx,frame);
    if(ret!=0){ //释放frame并返回NULL
        av_frame_free(&frame);
        return NULL;
    }
    qDebug()<<"framePts:"<<frame->pts;
    //音视频同步 获取当前解码frame的pts
    pts = frame->pts;
    return frame; //成功返回frame，需要外部释放
}
//清理解码器缓冲
void Decode::Clear()
{   if(code_ctx) avcodec_flush_buffers(code_ctx);
}
//关闭解码器
void Decode::Close()
{
    if (code_ctx)
    {
        avcodec_close(code_ctx);
        avcodec_free_context(&code_ctx);
    }
    pts = 0;
    qDebug()<<"success close Decode！";
}
