#include "Resample.h"
#include <QDebug>

Resample::Resample() {}

Resample::~Resample() {}

//打开重采样器
//目前是输出参数和输入参数一致，除了采样格式指定输出为s16
bool Resample::Open(AVCodecParameters *para)
{
    if(!para) return false;

    //Close();

    //mux.lock();
    //获取输入音频的声道数、采样格式、采样率
    AVChannelLayout in_ch_layout=para->ch_layout;  //输入的通道数
    enum AVSampleFormat in_sampleFormat=(AVSampleFormat)para->format;  //输入的采样格式
    int in_sampleRate=para->sample_rate;    //输入的采样率

    //设置输出音频的声道数、采样格式、采样率
    AVChannelLayout out_ch_layout=AV_CHANNEL_LAYOUT_STEREO; //输出的通道数
    out_sampleformat=AV_SAMPLE_FMT_S16; //输出的采样格式
    int out_sampleRate=para->sample_rate;  //输出的采样率

    //配置重采样参数
    swr_alloc_set_opts2(&swr_ctx,&out_ch_layout,out_sampleformat,out_sampleRate,
                        &in_ch_layout,in_sampleFormat,in_sampleRate,0,NULL);

    //用完释放para
    //avcodec_parameters_free(&para);
    //初始化重采样上下文
    int ret=swr_init(swr_ctx);
    mux.unlock();
    if(ret!=0){
        qDebug()<<"swr_init failed!";
        return false;
    }
    return true;
}

//开始重采样
//返回值为重采样大小，不管成功与否都释放indata空间
int Resample::ReSample(AVFrame *indata, unsigned char *outdata)
{
    if(!indata) return 0;
    if(!outdata){
        av_frame_free(&indata);
        return 0;
    }

    //分配空间
    uint8_t *data[2]={0};
    data[0]=outdata;
    //开始重采样 ret值为采样点大小
    int ret=swr_convert(swr_ctx,data,indata->nb_samples,
                          (const uint8_t**)indata->data,indata->nb_samples);
    //返回重采样后的数据大小
    //采样点*声道数*采样格式 比如：1024*2*2=4096
    int outSize=ret*indata->ch_layout.nb_channels*av_get_bytes_per_sample(out_sampleformat);
    //释放
    av_frame_free(&indata);
    if(ret<=0) return ret;
    return outSize;
}
//关闭重采样器
void Resample::Close()
{
    //mux.lock();
    swr_free(&swr_ctx);
    //mux.unlock();
    qDebug()<<"success close Resample！";
}
