#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QThread>
#include <queue>

struct AVCodecParameters;
class Decode;
class OpenGLWidget;
class AVPacket;


class VideoThread : public QThread
{
    Q_OBJECT
public:
    VideoThread();
    virtual ~VideoThread();

    //打开，不管成功与否都清理para
    virtual bool Open(AVCodecParameters *para);
    //从外部获取pkt,存入pkts队列
    virtual void Push(AVPacket *pkt);
    //工作函数
    virtual void run();
    //设置暂停
    void SetPause(bool isPause);
    //清理资源
    virtual void Clear();

    OpenGLWidget *gl=0; //gl

    //音视频同步时间，由外部传入
    long long synpts = 0;

    Decode *decode = 0;
private:
    //最大队列
    int maxList = 100;
    bool isExit = false;    //是否退出
    bool isPause=false; //是否暂停

    std::queue <AVPacket *> pkts;
    std::mutex mux;
signals:
    //void emitMP4Yuv420pData(uint8_t *m_py, uint8_t *m_pu, uint8_t *m_pv,int linesize1,int linesize2,int linesize3, int width, int height);
};

#endif // VIDEOTHREAD_H
