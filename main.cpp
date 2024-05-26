#include "mainwindow.h"

#include <QApplication>

#include "QFile"
#include <QAudioOutput>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();



    // //播放原始数据 pcm
    // QFile sourcefile;
    // QAudioOutput *audio;
    // sourcefile.setFileName("s16le_44100_2.pcm");
    // sourcefile.open(QIODevice::ReadOnly);
    // QAudioFormat format;
    // format.setSampleRate(44100);
    // format.setChannelCount(2);
    // format.setSampleSize(16);
    // format.setCodec("audio/pcm");
    // format.setByteOrder(QAudioFormat::LittleEndian);
    // format.setSampleType(QAudioFormat::UnSignedInt);
    // audio=new QAudioOutput(format);
    // audio->start(&sourcefile);


    return a.exec();
}
