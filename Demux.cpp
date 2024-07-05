#include "Demux.h"
#include <QDebug>
#include <QThread>

//pts转为毫秒静态函数
static double r2d(AVRational r){
    return r.den == 0 ? 0 : (double)r.num/(double)r.den;
}

Demux::Demux() {
    //初始化网络库（可以打开rtsp rtmp http 协议的流媒体视频）
    avformat_network_init();
}

Demux::~Demux() {}

bool Demux::Open(const char *url)
{

    //每次打开必须先关闭，否则打开另一个文件的话程序会崩溃
    Close();

    //网络相关参数设置
    AVDictionary *opts=NULL;
    //设置rtsp流以tcp协议打开
    av_dict_set(&opts,"rtsp_transport","tcp",0);
    //网路延迟时间
    av_dict_set(&opts,"max_delay","1000",0);

    int ret=avformat_open_input(&fmt_ctx,url,0,&opts);
    //int ret=avformat_open_input(&fmt_ctx,url,0,0);
    if(ret!=0){
        char buf[1024]={0};
        av_strerror(ret,buf,sizeof(buf)-1);
        qDebug()<<"open"<<url<<"failed! :"<<buf;
        return false;
    }
    //获取流信息
    ret=avformat_find_stream_info(fmt_ctx,0);
    //打印视频流详细信息
    av_dump_format(fmt_ctx,0,url,0);

    // //获取视频流索引
    video_index=av_find_best_stream(fmt_ctx,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);
    if(video_index>=0){
        st=fmt_ctx->streams[video_index];
        //获取视频相关信息
        width=st->codecpar->width;
        height=st->codecpar->height;
        qDebug()<<"width:"<<width<<" height:"<<height;
    }

    //获取音频流索引
        audio_index=av_find_best_stream(fmt_ctx,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);
    if(audio_index>=0){
        st=fmt_ctx->streams[audio_index];
        //获取音频相关信息
        sampleRate=st->codecpar->sample_rate;
        channels=st->codecpar->ch_layout.nb_channels;
        qDebug()<<"sampleRate:"<<sampleRate<<" channels:"<<channels;
    }
    //总时长 毫秒
    this->totalMs=fmt_ctx->duration/(AV_TIME_BASE/1000);
    qDebug()<<"totalMs:"<<totalMs;
    return true;
}

//读pkt 空间需要外部释放，释放AVPacket对象空间和数据空间 av_packet_free
//该函数需要调用多次读取每一帧，所以返回值为AVPacket
AVPacket *Demux::Read()
{
    mux.lock();
    if(!fmt_ctx){
        mux.unlock();
        return NULL;
    }
    AVPacket *pkt=av_packet_alloc();
    //读取一帧，并分配空间
    int ret=av_read_frame(fmt_ctx,pkt);

    if(ret!=0){ //当ret!=0时，说明文件读取完毕，退出并返回NULL值的pkt
        // //循环播放
        // qDebug()<<"end";
        // int ms=3000; //三秒位置 根据时间基数（分数）转换
        // long long pos=(double)ms/(double)1000*r2d(fmt_ctx->streams[pkt->stream_index]->time_base);
        // av_seek_frame(fmt_ctx,audio_index,pos,AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_FRAME);
        av_packet_free(&pkt);
        mux.unlock();
        return NULL;
    }

    //当前帧的播放时间=pts*time_base;
    //pts转换为毫秒 为后续方便做同步
    pkt->pts=pkt->pts*(r2d(fmt_ctx->streams[pkt->stream_index]->time_base)*1000);
    pkt->dts=pkt->dts*(1000*(r2d(fmt_ctx->streams[pkt->stream_index]->time_base)));
    //qDebug()<<"转换为毫秒的pts:"<<pkt->pts<<" ";

    //ret==0时，根据音视频索引分别读取数据
    // if(pkt->stream_index==video_index){
    //     qDebug()<<"图像："<<pkt->pts<<" ";
    // }else{
    //     qDebug()<<"音频："<<pkt->pts<<" ";
    // }
    mux.unlock();
    return pkt;
}

//获取视频参数 空间需要外部释放 avcodec_parameters_free
AVCodecParameters *Demux::CopyVPara()
{
    mux.lock();
    if(!fmt_ctx){
        mux.unlock();
        return NULL;
    }
    AVCodecParameters *para=avcodec_parameters_alloc();
    avcodec_parameters_copy(para,fmt_ctx->streams[video_index]->codecpar);
    mux.unlock();
    return para;
}
//获取音频参数 空间需要外部释放 avcodec_parameters_free
AVCodecParameters *Demux::CopyAPara()
{
    mux.lock();
    if(!fmt_ctx){
        mux.unlock();
        return NULL;
    }
    AVCodecParameters *para=avcodec_parameters_alloc();
    avcodec_parameters_copy(para,fmt_ctx->streams[audio_index]->codecpar);
    mux.unlock();
    return para;
}

//判断是否是音频
bool Demux::isAudio(AVPacket *pkt)
{
    if(!pkt) return false;
    if(pkt->stream_index==video_index){
        return false;
    }
    return true;
}

//获取视频和音频索引，用来判断是否有音频或者视频
void Demux::StreamIndex(bool &isVideo, bool &isAudio)
{
    if(video_index>=0){
        isVideo=true;
    }
    if(audio_index>=0){
        isAudio=true;
    }
}
//seek pos 0.0~1.0
bool Demux::Seek(double pos)
{
    mux.lock();
    if(!fmt_ctx){
        mux.unlock();
        return false;
    }
    //清理读取缓冲
    avformat_flush(fmt_ctx);

    long long seekPos=0;
    seekPos=fmt_ctx->streams[video_index]->duration*pos;
    int ret=av_seek_frame(fmt_ctx,video_index,seekPos,AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_FRAME);
    mux.unlock();

    if(ret<0) return false;
    return true;
}
//清空解复用器
void Demux::Clear()
{
    mux.lock();
    if(!fmt_ctx){
        mux.unlock();
        return ;
    }
    //清理读取缓冲
    avformat_flush(fmt_ctx);
    mux.unlock();
}
//关闭解复用
void Demux::Close()
{
    if(!fmt_ctx) return;
    avformat_close_input(&fmt_ctx);
    totalMs=0;
    video_index=-1;
    audio_index=-1;
    qDebug()<<"success close Demux！";
}

//获取总时长
int Demux::getTotal()
{
    return totalMs;
}

