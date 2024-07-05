#ifndef RESAMPLE_H
#define RESAMPLE_H
extern "C"{
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
}
#include <mutex>

class Resample
{
public:
    Resample();
    virtual ~Resample();

    //打开重采样器
    //目前是输出参数和输入参数一致，除了采样格式指定输出为s16
    virtual bool Open(AVCodecParameters* para); //success return true
    //开始重采样
    //返回重采样大小，不管成功与否都释放indata空间
    virtual int ReSample(AVFrame *indata,unsigned char* outdata);
    //关闭重采样器
    virtual void Close();
private:
    std::mutex mux;
    //重采样器上下文
    SwrContext *swr_ctx=nullptr;
    //输出的采样格式
    enum AVSampleFormat out_sampleformat;
};

#endif // RESAMPLE_H
