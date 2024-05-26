#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Demux.h"
#include "Decode.h"
#include "Resample.h"
#include "AudioPlay.h"
#include <QDebug>
#include <QThread>
#include <thread>

#include <QFileDialog>

extern "C"{
#include "libswscale/swscale.h"

}

#include "DemuxThread.h"

DemuxThread dt;

// #include <AudioThread.h>
// #include <VideoThread.h>


// AudioPlay *ap=0;
// Demux d;
// Decode de;
// Decode dev;
static bool flag=true;

// AudioThread at;
// VideoThread vt;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    startTimer(40);

    //connect(this,&MainWindow::emitMP4Yuv420pData,ui->openGLWidget,&OpenGLWidget::recvYuv420pDataShow);




    //vt.gl=ui->openGLWidget;

    // std::thread([&]{
    //     while(true){
    //         if(start){
    //             QThread::msleep(500);
    //             continue;
    //         }else{
    //             break;
    //         }
    //     }
    // while(true){
    //     // if(!flag) {
    //     //     QThread::msleep(50);
    //     //     continue;
    //     // }

    //     AVPacket *pkt=d.Read();
    //     if(!d.isAudio(pkt)){
    //        vt.Push(pkt);
    //        //av_packet_free(&pkt);
    //     }else{
    //         //qDebug()<<"音频："<<pkt->pts<<" ";
    //         at.Push(pkt);
    //         //av_packet_free(&pkt);
    //         }
    //     }
    // qDebug()<<"success close！";
    // }).detach();



    // std::thread([&]{
    //     while(true){
    //         if(start){
    //             QThread::msleep(500);
    //             continue;
    //         }else{
    //             break;
    //         }
    //     }
    //     Resample r;
    //     r.Open(d.CopyAPara());
    //     ap=AudioPlay::Get();
    //     ap->Open();
    // //分配重采样后的数据空间
    // unsigned char *pcm=new unsigned char[1024*1024]; //可根据机器性能，自定义设置大小

    // while(true){
    //     if(!flag) {
    //         QThread::msleep(50);
    //         continue;
    //     }

    //     AVPacket *pkt=d.Read();
    //     if(!d.isAudio(pkt)){
    //         //qDebug()<<"1图像："<<pkt->pts<<" ";
    //         dev.Send(pkt);
    //         while(true){
    //             AVFrame *frame=dev.Recv();
    //             if(!frame) break;
    //             emit emitMP4Yuv420pData(frame->data[0],frame->data[1],frame->data[2],frame->linesize[0],frame->linesize[1],frame->linesize[2],frame->width,frame->height);
    //             //QThread::msleep(24);
    //             av_frame_free(&frame); //只处理音频的话，这里必须释放pkt，否则会内存泄漏
    //        }
    //     }else{
    //         qDebug()<<"音频："<<pkt->pts<<" ";
    //         //av_packet_free(&pkt);

    //         de.Send(pkt);
    //         while(true){
    //             AVFrame *frame=de.Recv();
    //             if(!frame)
    //                 //qDebug()<<"success close！";
    //                 break;
    //             //重采样
    //             int size=r.ReSample(frame,pcm);
    //             while(true){//必须循环播放，不然会快速播放，原因可能是缓冲区的数据未播放完就被清除掉了，所以得循环
    //                 if(size<=0) break;
    //                 //缓冲未播完，等待播放完
    //                 if(ap->GetFree()<size){ //空闲的数据<重采样的大小时，等待;空闲的数据大>=重采样的大小时，继续写入数据
    //                     QThread::msleep(1);
    //                     continue;
    //                 }
    //                 qDebug()<<"free:"<<ap->GetFree();
    //                 //写入数据
    //                 ap->Write(pcm,size);
    //                 break;
    //             }
    //         }
    //     }
    // }
    // qDebug()<<"success close！";
    // d.Close();
    // de.Close();
    // r.Close();
    // ap->Close();
    // delete []pcm;
    // }).detach();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //ap->AudioPause();
    flag=!flag;
    //at.SetPause(!flag);
    //vt.SetPause(!flag);

    dt.SetPause(!flag);
}

//打开文件
void MainWindow::on_OpenFileBtn_clicked()
{
    //选择文件
    QString name=QFileDialog::getOpenFileName(this,"选择视频文件");
    if(name.isEmpty()) return;
    qDebug()<<name;

    dt.Open(name.toLocal8Bit(),ui->openGLWidget);
    dt.Start();

    // d.Open(name.toLocal8Bit());
    // start=false;
    // dev.Open(d.CopyVPara());
    // de.Open(d.CopyAPara());

    // at.Open(d.CopyAPara());
    // at.start();
    // vt.Open(d.CopyVPara());
    // //vt.gl=ui->openGLWidget;
    // vt.start();

    // //视频总长度转换为时分秒
    // int hours = d.getTotal() / 3600000;
    // int minutes = (d.getTotal() % 3600000) / 60000;
    // int seconds = (d.getTotal() % 60000) / 1000;
    // QTime time(hours,minutes,seconds);
    // ui->timeEdit->setTime(time);
}


//定时器 进度条显示
void MainWindow::timerEvent(QTimerEvent *e)
{
    //如果是按住
    if(isSliderPress) return;


    long long total = dt.totalMs;
    if (total > 0)
    {
        double pos = (double)dt.pts /(double) total;
        int v = ui->Slider->maximum() * pos;
        ui->Slider->setValue(v);
    }
}


//进度条seek
void MainWindow::on_Slider_sliderPressed()
{
    isSliderPress=true;
}

//进度条seek
void MainWindow::on_Slider_sliderReleased()
{
    isSliderPress=false;

    //松开做seek
    double pos=0.0;
    pos=(double)ui->Slider->value()/(double)ui->Slider->maximum();
    dt.Seek(pos);
}

