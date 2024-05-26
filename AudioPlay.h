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
    //清理资源
    virtual void Clear()=0;
    //关闭音频播放器
    virtual void Close()=0;

    //音视频同步 返回缓冲中还没有播放的时间（毫秒）
    virtual long long GetNoPlayMs() = 0;

    //暂停播放音频
    virtual void AudioPause()=0;
    //清空音频缓冲区中的数据
    virtual void AudioReset()=0;



    //播放的音频格式
    int sampleRate=44100;
    int sampleSize=16;
    int channels=2;
protected:

    QAudioOutput *Output=NULL;
    QIODevice *io=NULL;
};

#endif // AUDIOPLAY_H
