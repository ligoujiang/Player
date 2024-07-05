#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QThread>
#include <queue>
class Decode;
class AudioPlay;
class Resample;
struct AVCodecParameters;
class AVPacket;

class AudioThread : public QThread
{
public:
    AudioThread();
    virtual ~AudioThread();

    //打开，不管成功与否都清理para
    virtual bool Open(AVCodecParameters *para,int sampleRate,int channels);
    //工作函数
    virtual void run() override;
    //从外部获取pkt,存入pkts队列
    virtual void Push(AVPacket *pkt);
    //设置暂停
    void SetPause(bool isPause);
    //清理资源
    virtual void Clear();

    //音视频同步 当前音频播放的pts
    long long pts = 0;
private:
    //最大队列
    int maxList = 100;
    bool isExit = false; //是否退出
    bool isPause=false; //是否暂停

    std::mutex amux;

    std::queue <AVPacket *> pkts;
    Decode *decode = 0; //音频解码对象
    AudioPlay *ap = 0;  //音频播放对象
    Resample *res = 0;  //音频重采样对象
};

#endif // AUDIOTHREAD_H
