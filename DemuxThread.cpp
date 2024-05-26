#include "DemuxThread.h"

#include "Demux.h"
#include "VideoThread.h"
#include "AudioThread.h"

#include <QDebug>

#include "Decode.h"
#include "OpenGLWidget.h"

DemuxThread::DemuxThread() {}

DemuxThread::~DemuxThread() {}

//创建解封装对象
bool DemuxThread::Open(const char *url,OpenGLWidget *gl)
{
    //创建对象
    if(!demux) demux=new Demux;
    if(!vt) vt=new VideoThread;
    if(!at) at=new AudioThread;

    //打开解封装
    demux->Open(url);

    //进度条
    totalMs=demux->totalMs;
    //qDebug()<<"dt:"<<totalMs;

    //打开视频解码器和处理线程
    vt->Open(demux->CopyVPara());
    vt->gl=gl;  //指向主窗口的gl

    //打开音频解码器和处理线程
    at->Open(demux->CopyAPara());
    return true;
}

//启动（本身）解封装、解码视频、解码音频线程
void DemuxThread::Start()
{
    QThread::start();   //启动当前本身（解封装）线程
    if (vt)vt->start(); //启动视频线程
    if (at)at->start(); //启动音频线程
}

//工作函数
void DemuxThread::run()
{
    while (!isExit)
    {
        // if (!demux)
        // {
        //     msleep(5);
        //     continue;
        // }

        //音视频同步
        if (vt && at)
        {
            pts = at->pts;  //进度条
            vt->synpts = at->pts;   //同步
        }



        if(isPause){
            msleep(100);
            continue;
        }




        AVPacket *pkt = demux->Read();
        if (!pkt)
        {

            msleep(5);
            continue;
        }
        //判断数据是音频
        if (demux->isAudio(pkt))
        {
            //音频
            if(at) at->Push(pkt);
        }
        else
        {
            //视频
            if (vt) vt->Push(pkt);
        }
        //msleep(1);
    }
}

//设置暂停
void DemuxThread::SetPause(bool isPause)
{
    this->isPause=isPause;
    //vt->SetPause(isPause);
    at->SetPause(isPause);
}
//seek
void DemuxThread::Seek(double pos)
{
    Clear();    //清理


    bool status=this->isPause;

    SetPause(true); //暂停

    mux.lock();
    if(demux){
        demux->Seek(pos);
    }
    mux.unlock();

    //实际要显示的位置
    long long seekPts=pos*demux->totalMs;
    while(!isExit){
        AVPacket *pkt=demux->Read();
        if(!pkt) break;
        if(pkt->stream_index==demux->audio_index){
            //是音频丢弃数据
            av_packet_free(&pkt);
            continue;
        }
        bool ret=vt->decode->Send(pkt);
        if(!ret) break;
        AVFrame *frame=vt->decode->Recv();
        if(!frame) continue;
        //到达位置
        if(frame->pts>=seekPts){
            this->pts=frame->pts; //暂停状态下，立刻显示该帧画面
            vt->gl->Init(frame->data[0],frame->data[1],frame->data[2],frame->linesize[0],frame->linesize[1],frame->linesize[2],frame->width,frame->height);
            break;
        }
        av_frame_free(&frame);
    }

    if(!status){
        SetPause(false); //播放
    }
}
//清理资源 (不然seek有问题)
void DemuxThread::Clear()
{
    if (demux) demux->Clear();
    if (vt) vt->Clear();
    if (at) at->Clear();
}
