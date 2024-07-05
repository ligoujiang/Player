#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <mutex>
#include <QOpenGLShaderProgram>

extern "C"{
#include "libavutil/frame.h"
}

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:

    virtual void Init(int width,int height);
    virtual void Repaint(AVFrame* frame);

    OpenGLWidget(QWidget *parent);
    ~OpenGLWidget();
protected:
    //刷新显示
    void paintGL();

    //初始化gl
    void initializeGL();

    // 窗口尺寸变化
    void resizeGL(int width, int height);
private:
    int width=960;
    int height=540;
    std::mutex mux;
    //shader程序
    QOpenGLShaderProgram program;
    //shader中的yuv变量地址
    GLuint unis[3]={0};
    //opengl的texture地址
    GLuint texs[3]={0};
    //材质的内存空间
    unsigned char *datas[3]={0};

};

#endif // OPENGLWIDGET_H

