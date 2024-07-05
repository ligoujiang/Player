#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>
#include <thread>
#include <QThread>


#include "DemuxThread.h"



bool flag=false;



DemuxThread dt;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setStyleSheet(".QWidget{background-color:grey}");
    // std::thread([&]{
    //     while(1){
    //         if(!flag) continue;


    //         AVPacket *pkt = d.Read();


    //         if (!pkt) break;
    //         if(!d.isAudio(pkt)){

    //             //vt.Push(pkt);

    //             vde.Send(pkt);
    //             while(true){
    //                 AVFrame *frame=vde.Recv();
    //                 if(!frame) break;
    //                 op->Repaint(frame);
    //             }

    //             // while(1){
    //             //     AVFrame *frame=vde.Recv();
    //             //     if(!frame) break;
    //             //     qDebug()<<"图像frame："<<frame->pts<<" ";
    //             //     av_frame_free(&frame);
    //             // }
    //         }else{

    //             //at.Push(pkt);

    //             //ade.Send(pkt);
    //             // while(true){
    //             // AVFrame *frame=ade.Recv();
    //             // //分配重采样后的数据空间
    //             // unsigned char *pcm=new unsigned char[1024*1024]; //可根据机器性能，自定义设置大小
    //             //     if(!frame)
    //             //         break;
    //             //     //重采样
    //             //     int size=r.ReSample(frame,pcm);
    //             //     while(true){//必须循环播放，不然会快速播放，原因可能是缓冲区的数据未播放完就被清除掉了，所以得循环
    //             //         if(size<=0) break;
    //             //         //缓冲未播完，等待播放完
    //             //         if(ap->GetFree()<size){ //空闲的数据<重采样的大小时，等待;空闲的数据大>=重采样的大小时，继续写入数据
    //             //             QThread::msleep(1);
    //             //             continue;
    //             //         }
    //             //         //写入数据
    //             //         ap->Write(pcm,size);
    //             //         break;
    //             //     }
    //             //     delete []pcm;
    //             // }

    //             // while(1){
    //             //     AVFrame *frame=ade.Recv();
    //             //     if(!frame) break;
    //             //     qDebug()<<"音频frame："<<frame->pts<<" ";
    //             //     av_frame_free(&frame);
    //             // }
    //         }
    //     }
    // }).detach();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_playBtn_clicked()
{
    //选择文件
    QString name=QFileDialog::getOpenFileName(this,"选择视频文件");
    if(name.isEmpty()) return;
    qDebug()<<name;
    //flag=false;
    //d.Open(name.toUtf8());
    // //d.Open("http://zhibo.hkstv.tv/livestream/mutfysrq/playlist.m3u8");
    //vde.Open(d.CopyVPara());
    // at.Open(d.CopyAPara(),d.sampleRate,d.channels);
    // at.start();

    // vt.Open(d.CopyVPara(),ui->openGLWidget);
    // vt.start();
    // op=ui->openGLWidget;
    // op->Init(d.width,d.height);
    // flag=true;


    dt.Open(name.toUtf8(),ui->openGLWidget,false);
    //dt.Open("https://www.apple.com/105/media/us/iphone-x/2017/01df5b43-28e4-4848-bf20-490c34a926a7/films/feature/iphone-x-feature-tpl-cc-us-20170912_1920x1080h.mp4",ui->openGLWidget,false);
    dt.Start();
    dt.m_flag=true;
}

