#include "VideoThread.h"

#include "Decode.h"
#include "OpenGLWidget.h"
#include <QDebug>

VideoThread::VideoThread() {}

VideoThread::~VideoThread() {
    //delete decode;
}

//打开，不管成功与否都清理para
bool VideoThread::Open(AVCodecParameters *para,OpenGLWidget *gl)
{
    Clear();
    /*
        720P的分辨率为1280x720像素
        1080P的分辨率为1920*1080像素
        2k的分辨率为2560*1440像素
        4k的分辨率为3840*2160像素
        8K的分辨率为7680×4320像素
    */
    if(para->height==720||para->height==1080||para->height==1440||para->height==2160){
        qDebug()<<"无需转换";
    }else{
        sws=new Swscale();
        sws->Open(para);
    }

    //指向opengl组件对象
    if(gl){
        m_gl=gl;
        if(sws){
            m_gl->Init(1280,720);
        }else{
            m_gl->Init(para->width,para->height);
        }
    }

    // //图像格式转换
    // sws_width=1280;
    // sws_heigth=720;
    // //SWS_BICUBIC
    // sws_ctx=sws_getContext(para->width,para->height,AV_PIX_FMT_YUV420P,
    //                          sws_width,sws_heigth,AV_PIX_FMT_YUV420P,SWS_BICUBIC,NULL,NULL,NULL);

    // //new一个转换的空间
    // frameYUV=av_frame_alloc();
    // //av_image_get_buffer_size() 根据参数计算视频图像缓冲区大小
    // //创建缓冲区大小
    // out_buffer=static_cast<uint8_t*>(av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,sws_width,sws_heigth,1)));
    // //对申请的缓冲区进行格式化和划分大小
    // av_image_fill_arrays(frameYUV->data,frameYUV->linesize,out_buffer,AV_PIX_FMT_YUV420P,sws_width,sws_heigth,1);

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
        // if(isPause){
        //     msleep(10);
        //     continue;
        // }

        vmux.lock();
        if (pkts.size() <= maxList)
        {
            pkts.push(pkt);
            vmux.unlock();
            break;
        }
        vmux.unlock();
        msleep(1);
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


        // if(isPause){
        //     msleep(10);
        //     continue;
        // }


        //音视频同步
        if (synpts < decode->pts)
        {
            msleep(1);
            continue;
        }

        //vmux.lock();
        AVPacket *pkt = pkts.front();
        pkts.pop();
        //vmux.unlock();
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
            if (m_gl)
            {
                if(sws){
                    //sws.Sws(frame)转换并在内部释放frame
                    m_gl->Repaint(sws->Sws(frame));
                }else{
                    //无需转换，并且释放frame
                    m_gl->Repaint(frame);
                    av_frame_free(&frame);
                }
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
    synpts = 0;
    if(sws){
        delete sws;
        sws=nullptr;
    }
    //清理队列
    vmux.lock();
    while(!pkts.empty()){
        AVPacket *pkt=pkts.front();
        av_packet_free(&pkt);
        pkts.pop();
        vmux.unlock();
    }
    vmux.unlock();
}


