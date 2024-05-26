#ifndef DEMUXTHREAD_H
#define DEMUXTHREAD_H
#include <QThread>
class Demux;
class VideoThread;
class AudioThread;
class OpenGLWidget;

//创建解封装对象，并且启动解封装线程（本身）、解码视频线程、解码音频线程
class DemuxThread : public QThread
{
public:
    DemuxThread();
    virtual ~DemuxThread();

    //创建解封装对象、解码视频、解码音频线程，并设置设置显示视频窗口
    virtual bool Open(const char *url,OpenGLWidget *gl);
    //启动（本身）解封装、解码视频、解码音频线程
    virtual void Start();
    //工作函数 本线程解封装、并把解封装数据包插入各线程队列
    virtual void run();
    //设置暂停
    void SetPause(bool isPause);

    //seek
    virtual void Seek(double pos);

    //清理资源
    virtual void Clear();



    //进度条显示相关
    long long pts=0;
    long long totalMs=0;
private:
    bool isExit=false;  //是否退出
    bool isPause=false; //是否暂停
    std::mutex mux;
    Demux *demux=0; //解封装

    AudioThread *at=0;  //音频线程
    VideoThread *vt=0;  //视频线程
};

#endif // DEMUXTHREAD_H
