#ifndef DEMUXTHREAD_H
#define DEMUXTHREAD_H
#include <QThread>
class Demux;
//class VideoThread;
//class AudioThread;
class OpenGLWidget;

#include "VideoThread.h"
#include "AudioThread.h"

//创建解封装对象，并且启动解封装线程（本身）、解码视频线程、解码音频线程
class DemuxThread : public QThread
{
public:
    DemuxThread();
    virtual ~DemuxThread();

    //创建解封装对象、解码视频、解码音频线程，并设置设置显示视频窗口
    virtual bool Open(const char *url,OpenGLWidget *gl,bool flag);
    //启动（本身）解封装、解码视频、解码音频线程
    virtual void Start();
    //工作函数 本线程解封装、并把解封装数据包插入各线程队列
    virtual void run();
    //清理资源
    void Clear();
    //设置暂停
    void SetPause(bool isPause);

    bool m_flag=true;

private:
    bool isVideo=false;
    bool isAudio=false;

    bool isExit=false;  //是否退出
    bool isPause=false; //是否暂停
    std::mutex mux;

    Demux *demux=nullptr; //解封装
    AudioThread *at=nullptr;  //音频线程
    VideoThread *vt=nullptr;  //视频线程

};

#endif // DEMUXTHREAD_H
