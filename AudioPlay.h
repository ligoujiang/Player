#ifndef AUDIOPLAY_H
#define AUDIOPLAY_H

#include <QAudioOutput>

//工厂接口类
class AudioPlay
{
public:
    AudioPlay();
    virtual ~AudioPlay();

    //打开音频播放
    virtual bool Open()=0;
    //播放音频
    virtual bool Write(const unsigned char *data,int datasize)=0;

    //通过父类调用子类
    static AudioPlay *Get();

    //判断是否有足够的空间写
    virtual int GetFree()=0;
    //设置暂停
    virtual void SetPause(bool isPuase)=0;
    //调节音频音量
    virtual void setVolume(double volume)=0;

    //清理资源
    virtual void Clear()=0;
    //关闭音频播放器
    virtual void Close()=0;

    //音视频同步 返回缓冲中还没有播放的时间（毫秒）
    virtual long long GetNoPlayMs() = 0;


    //播放的音频格式
    int sampleRate=48000;
    int sampleSize=16;
    int channels=2;
};

#endif // AUDIOPLAY_H
