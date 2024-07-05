#include "AudioThread.h"
#include "Decode.h"
#include "AudioPlay.h"
#include "Resample.h"

AudioThread::AudioThread() {}

AudioThread::~AudioThread() {}

//打开音频相关对象，不管成功与否都清理para
bool AudioThread::Open(AVCodecParameters *para, int sampleRate, int channels)
{

    Clear();

    if (!decode) decode = new Decode();
    if (!res) res = new Resample();
    if (!ap) ap = AudioPlay::Get();

    //打开重采样对象
    res->Open(para);

    //打开音频播放对象
    ap->sampleRate = sampleRate;
    ap->channels = channels;
    ap->Open();

    //打开音频解码对象,并内部释放para
    decode->Open(para);
    return true;
}
//工作函数
void AudioThread::run()
{
    unsigned char *pcm = new unsigned char[1024 * 1024 *10];
    while (!isExit)
    {
        //没有数据
        if (pkts.empty() || !decode || !res || !ap)
        {
            msleep(1);
            continue;
        }

        //mux.lock();
        AVPacket *pkt = pkts.front();
        pkts.pop();
        //mux.unlock();
        bool ret = decode->Send(pkt);
        if (!ret)
        {
            msleep(1);
            continue;
        }
        //一次send 多次recv
        while (!isExit)
        {
            AVFrame * frame = decode->Recv();
            if (!frame) break;

            //音视频同步 减去缓冲中未播放的时间
            pts = decode->pts - ap->GetNoPlayMs();

            //重采样
            int size = res->ReSample(frame,pcm);
            //播放音频
            while (!isExit)
            {
                if (size <= 0)break;
                //缓冲未播完，空间不够
                if (ap->GetFree() < size) //||isPause
                {
                    msleep(1);
                    continue;
                }
                ap->Write(pcm, size);
                break;
            }
        }
    }
    delete []pcm;
}

//pkt入队
void AudioThread::Push(AVPacket *pkt)
{
    if (!pkt)return;
    while (!isExit)
    {
        if(isPause){
            msleep(10);
            continue;
        }

        amux.lock();
        if (pkts.size() <= maxList)
        {
            pkts.push(pkt);
            amux.unlock();
            break;
        }
        amux.unlock();
        msleep(1);
    }
}
//设置暂停
void AudioThread::SetPause(bool isPause)
{
    this->isPause=isPause;
}

//清理资源
void AudioThread::Clear()
{
    pts = 0;
    if(ap) ap->Clear();
    //清理队列
    amux.lock();
    while(!pkts.empty()){
        AVPacket *pkt=pkts.front();
        av_packet_free(&pkt);
        pkts.pop();
        amux.unlock();
    }
    amux.unlock();
}
