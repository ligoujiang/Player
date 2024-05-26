#include "AudioThread.h"
#include "Decode.h"
#include "AudioPlay.h"
#include "Resample.h"

AudioThread::AudioThread() {}

AudioThread::~AudioThread() {}

//打开音频相关对象，不管成功与否都清理para
bool AudioThread::Open(AVCodecParameters *para)
{

    pts = 0;

    if (!decode) decode = new Decode();
    if (!res) res = new Resample();
    if (!ap) ap = AudioPlay::Get();

    //打开重采样对象
    res->Open(para);

    //打开音频播放对象
    ap->sampleRate = para->sample_rate;
    ap->channels = para->ch_layout.nb_channels;
    ap->Open();

    //打开音频解码对象,并内部释放para
    decode->Open(para);
    return true;
}
//工作函数
void AudioThread::run()
{
    unsigned char *pcm = new unsigned char[1024 * 1024 * 10];
    while (!isExit)
    {
        //没有数据
        if (pkts.empty() || !decode || !res || !ap)
        {
            msleep(1);
            continue;
        }

        //判断是否暂停
        if(isPause){
            msleep(100);
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
//设置暂停
void AudioThread::SetPause(bool isPause)
{
    this->isPause=isPause;
}

//清理资源
void AudioThread::Clear()
{
    //DecodeThread::Clear();
    decode->Clear();
    if(ap) ap->Clear();
    pts = 0;
    //清理队列
    while(!pkts.empty()){
        mux.lock();
        AVPacket *pkt=pkts.front();
        av_packet_free(&pkt);
        pkts.pop();
        mux.unlock();
    }
}
