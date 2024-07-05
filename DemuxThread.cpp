#include "DemuxThread.h"

#include "Demux.h"
#include "VideoThread.h"
#include "AudioThread.h"

DemuxThread::DemuxThread() {}

DemuxThread::~DemuxThread() {
    delete vt;
    delete at;
    delete demux;
}

//创建解封装对象
bool DemuxThread::Open(const char *url,OpenGLWidget *gl,bool flag)
{
    Clear();
    m_flag=flag;

    //创建对象
    if(!demux) demux=new Demux;

    //打开解封装
    demux->Open(url);
    //判断是否有音频或者视频
    demux->StreamIndex(isVideo,isAudio);
    qDebug()<<isVideo<<isAudio;

    if(isVideo){
        //创建对象并打开视频解码器和处理线程
        if(!vt) vt=new VideoThread;
        vt->Open(demux->CopyVPara(),gl);
        //vt->synpts=0;
    }else{
        if(vt) vt->Clear();
    }

    if(isAudio){
        //创建对象并打开音频解码器和处理线程
        if(!at) at=new AudioThread;
        at->Open(demux->CopyAPara(),demux->sampleRate,demux->channels);
    }else{
        //立马停止播放音频
        if(at){
            at->Clear();
        }
    }
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
        if(!m_flag) continue;


        if (!demux)
        {
            msleep(5);
            continue;
        }

        //音视频同步
        if (vt && at && isVideo)
        {
            vt->synpts = at->pts;
        }



        // if(isPause){
        //     msleep(100);
        //     continue;
        // }




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
            if(vt) vt->Push(pkt);
        }

    }
}
//清理资源
void DemuxThread::Clear()
{
    isVideo=false;
    isAudio=false;
}

//设置暂停
void DemuxThread::SetPause(bool isPause)
{
    this->isPause=isPause;
    at->SetPause(isPause);
}
