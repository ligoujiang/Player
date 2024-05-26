#include "AudioPlay.h"
#include <QDebug>

//实现类
class QtAudioPlay :public AudioPlay{
public:
    //重写父类的Open函数
    virtual bool Open()override{
        QAudioFormat fmt;
        fmt.setSampleRate(sampleRate);
        fmt.setChannelCount(channels);
        fmt.setSampleSize(sampleSize);
        fmt.setCodec("audio/pcm");
        fmt.setByteOrder(QAudioFormat::LittleEndian);
        fmt.setSampleType(QAudioFormat::UnSignedInt);
        Output=new QAudioOutput(fmt);
        io=Output->start();
        if(io){
            return true;
        }
        return false;
    }
    //重写父类的Write函数
    virtual bool Write(const unsigned char *data,int datasize) override{
        int size=io->write((char*)data,datasize);   //size为空间大小 4096 datasize为重采样后的数据大小
        if(datasize!=size){ //当重采样数据大小不等于io写入数据的大小时,返回false
            return false;
        }
        return true;
    }

    virtual int GetFree() override{
        if(!Output){
            return 0;
        }
        int free=Output->bytesFree();
        return free;
    }

    //清理资源
    virtual void Clear()override{
        if(io){
            io->reset();
        }
    };


    //关闭音频播放器
    virtual void Close() override{
        if(io){
            io->close();
            io=NULL;
        }
        if(Output){
            Output->stop();
            delete Output;
            Output=NULL;
        }
        qDebug()<<"success close AudioPlay!";
    };

    //音视频同步 返回缓冲中还没有播放的时间（毫秒）
    virtual long long GetNoPlayMs() override{
        // if (!output)
        // {
        //     return 0;
        // }


        long long pts = 0;

        //还未播放的字节数 总空间-空闲空间=未播放的数据大小
        double size = Output->bufferSize() - Output->bytesFree();
        //一秒音频字节大小 比如：44100*（16/8）*2
        double secSize = sampleRate * (sampleSize / 8) *channels;
        if (secSize <= 0)
        {
            pts = 0;
        }
        else
        {
            pts = (size/secSize) * 1000;    //未播放的数据大小/一秒音频大小 转换为毫秒
        }
        return pts;
    };

    virtual void AudioPause() override{
        Output->suspend();
    }
    //清空音频缓冲区中的数据
    virtual void AudioReset() override{
        io->reset();
    };
};

AudioPlay::AudioPlay() {}

AudioPlay::~AudioPlay() {}

AudioPlay *AudioPlay::Get()
{
    static QtAudioPlay play;
    return &play;
}

