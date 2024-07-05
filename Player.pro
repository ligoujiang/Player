QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AudioPlay.cpp \
    AudioThread.cpp \
    Decode.cpp \
    Demux.cpp \
    DemuxThread.cpp \
    OpenGLWidget.cpp \
    Resample.cpp \
    Swscale.cpp \
    VideoThread.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    AudioPlay.h \
    AudioThread.h \
    Decode.h \
    Demux.h \
    DemuxThread.h \
    OpenGLWidget.h \
    Resample.h \
    Swscale.h \
    VideoThread.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

win32{
INCLUDEPATH +=$$PWD\ffmpeg-master-latest-win64-gpl-shared\include
LIBS +=$$PWD\ffmpeg-master-latest-win64-gpl-shared\lib\avformat.lib    \
       $$PWD\ffmpeg-master-latest-win64-gpl-shared\lib\avcodec.lib    \
       $$PWD\ffmpeg-master-latest-win64-gpl-shared\lib\avdevice.lib    \
       $$PWD\ffmpeg-master-latest-win64-gpl-shared\lib\avfilter.lib    \
       $$PWD\ffmpeg-master-latest-win64-gpl-shared\lib\avutil.lib    \
       $$PWD\ffmpeg-master-latest-win64-gpl-shared\lib\postproc.lib    \
       $$PWD\ffmpeg-master-latest-win64-gpl-shared\lib\swresample.lib    \
       $$PWD\ffmpeg-master-latest-win64-gpl-shared\lib\swscale.lib    \
       "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22621.0\um\x64\Ole32.Lib" \
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
