#include "VideoThread.h"

#include "Decode.h"
#include "OpenGLWidget.h"

VideoThread::VideoThread() {
    //connect(this,&VideoThread::emitMP4Yuv420pData,gl,&OpenGLWidget::recvYuv420pDataShow);
}

VideoThread::~VideoThread() {}

//打开，不管成功与否都清理para
bool VideoThread::Open(AVCodecParameters *para)
{

    synpts = 0;

    //打开视频对象
    if(!gl) gl=new OpenGLWidget();

    //打开解码器,内部会释放para
    if (!decode) decode = new Decode();
    decode->Open(para);
    return true;
}

//从外部获取pkt,存入pkts队列
void VideoThread::Push(AVPacket *pkt)
{
    if (!pkt)return;
    while (!isExit)
    {
        if(isPause){
            msleep(10);
            continue;
        }

        if (pkts.size() < maxList)
        {
            mux.lock();
            pkts.push(pkt);
            mux.unlock();
            break;
        }
        msleep(10);
    }
}
//工作函数
void VideoThread::run()
{
    while (!isExit)
    {
        //没有数据
        if (pkts.empty() || !decode)
        {
            msleep(1);
            continue;
        }

        //音视频同步
        if (synpts < decode->pts)
        {
            msleep(1);
            qDebug()<<"同步中";
            continue;
        }

        if(isPause){
            msleep(10);
            continue;
        }

        mux.lock();
        AVPacket *pkt = pkts.front();
        pkts.pop();
        mux.unlock();
        bool re = decode->Send(pkt);
        if (!re)
        {
            msleep(1);
            continue;
        }
        //一次send 多次recv
        while (!isExit)
        {
            AVFrame * frame = decode->Recv();
            if (!frame) break;
            //显示视频
            if (gl)
            {
                gl->Init(frame->data[0],frame->data[1],frame->data[2],frame->linesize[0],frame->linesize[1],frame->linesize[2],frame->width,frame->height);
                av_frame_free(&frame);
                //msleep(25);
                //emit emitMP4Yuv420pData(frame->data[0],frame->data[1],frame->data[2],frame->linesize[0],frame->linesize[1],frame->linesize[2],frame->width,frame->height);
            }

        }
    }
}
//设置暂停
void VideoThread::SetPause(bool isPause)
{
    this->isPause=isPause;
}
//清理资源 (seek)
void VideoThread::Clear()
{
    decode->Clear();
    synpts = 0;
    //清理队列
    while(!pkts.empty()){
        mux.lock();
        AVPacket *pkt=pkts.front();
        av_packet_free(&pkt);
        pkts.pop();
        mux.unlock();
    }
}


