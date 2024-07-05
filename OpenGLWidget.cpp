#include "OpenGLWidget.h"

//自动加双引号
#define GET_STR(x) #x
#define A_VER 3
#define T_VER 4
//顶点shader
const char *vString=GET_STR(
    attribute vec4 vertexIn;    //顶点坐标
    attribute vec2 textureIn;   //片元坐标
    varying vec2 textureOut;    //共享坐标
    void main(void){
        gl_Position=vertexIn;
        textureOut=textureIn;
    }
    );
//片元shader
const char *tString=GET_STR(
    varying vec2 textureOut;    //共享坐标
    uniform sampler2D tex_y;
    uniform sampler2D tex_u;
    uniform sampler2D tex_v;
    void main(void){
        //yuv转换成rgb
        vec3 yuv;
        vec3 rgb;
        yuv.x=texture2D(tex_y,textureOut).r;
        yuv.y=texture2D(tex_u,textureOut).r-0.5;
        yuv.z=texture2D(tex_v,textureOut).r-0.5;
        rgb=mat3(1.0,1.0,1.0,
                   0.0,-0.39465,2.03211,
                   1.13983,-0.58060,0.0)*yuv;
        gl_FragColor=vec4(rgb,1.0);
    }
    );

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{}

OpenGLWidget::~OpenGLWidget() {
}

void OpenGLWidget::Init(int width, int height)
{
    //initializeOpenGLFunctions();
    this->width=width;
    this->height=height;
    qDebug()<<"w:"<<this->width<<"h:"<<this->height;
    delete datas[0];
    delete datas[1];
    delete datas[2];

    //分配材质内存空间
    datas[0]=new unsigned char[width*height];   //Y
    datas[1]=new unsigned char[width*height/4]; //U
    datas[2]=new unsigned char[width*height/4]; //V

    if(texs[0]){
        glDeleteTextures(3,texs);
    }
    //创建材质
    glGenTextures(3,texs);
    //Y
    glBindTexture(GL_TEXTURE_2D,texs[0]);
    //放大过滤，线性插值 GL_NEAREST(效率高，但马赛克严重)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //创建材质（显卡）空间
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,width,height,0,GL_RED,GL_UNSIGNED_BYTE,0);
    //U
    glBindTexture(GL_TEXTURE_2D,texs[1]);
    //放大过滤，线性插值
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //创建材质（显卡）空间
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,width/2,height/2,0,GL_RED,GL_UNSIGNED_BYTE,0);
    //V
    glBindTexture(GL_TEXTURE_2D,texs[2]);
    //放大过滤，线性插值
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //创建材质（显卡）空间
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,width/2,height/2,0,GL_RED,GL_UNSIGNED_BYTE,0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWidget::Repaint(AVFrame *frame)
{
    //if (!frame)return;

    // //容错，保证尺寸正确
    // if (!datas[0] || width*height == 0 || frame->width != this->width || frame->height != this->height)
    // {
    //     av_frame_free(&frame);
    //     return;
    // }

    //mux.lock();




    memcpy(datas[0],frame->data[0],width*height);
    memcpy(datas[1],frame->data[1],width*height/4);
    memcpy(datas[2],frame->data[2],width*height/4);



    // if(width==frame->linesize[0]){ //无需对齐
    //     memcpy(datas[0],frame->data[0],width*height);
    //     memcpy(datas[1],frame->data[1],width*height/4);
    //     memcpy(datas[2],frame->data[2],width*height/4);
    // }else{//行对齐
    //     qDebug()<<"行对齐";
    //     for(int i=0;i<height;i++){
    //         memcpy(datas[0]+width*i,frame->data[0]+frame->linesize[0]*i,width);
    //     }
    //     for(int i=0;i<height/2;i++){
    //         memcpy(datas[1]+width/2*i,frame->data[1]+frame->linesize[1]*i,width);
    //     }
    //     for(int i=0;i<height/2;i++){
    //         memcpy(datas[2]+width/2*i,frame->data[2]+frame->linesize[2]*i,width);
    //     }
    // }


    //mux.unlock();
    //av_frame_free(&frame);
    update();
}

void OpenGLWidget::initializeGL()
{
    //先初始化OpenGL函数
    initializeOpenGLFunctions();
    //program加载shader(顶点和片元)脚本
    //片元（像素）
    qDebug()<<program.addShaderFromSourceCode(QOpenGLShader::Fragment,tString);
    //顶点shader
    qDebug()<<program.addShaderFromSourceCode(QOpenGLShader::Vertex,vString);
    //设置顶点坐标的变量
    program.bindAttributeLocation("vertexIn",A_VER);
    //设置材质坐标
    program.bindAttributeLocation("textureIn",T_VER);
    //编译shader
    program.link();
    program.bind();
    //qDebug()<<"program.link()="<<program.link();
    //qDebug()<<"program.bind()="<<program.bind();
    //传递顶点和材质坐标
    //顶点
    static const GLfloat ver[]={
        -1.0f,-1.0f,
        1.0f,-1.0f,
        -1.0f,1.0f,
        1.0f,1.0f
    };
    //材质
    static const GLfloat tex[]={
        0.0f,1.0f,
        1.0f,1.0f,
        0.0f,0.0f,
        1.0f,0.0f
    };
    //顶点
    glVertexAttribPointer(A_VER,2,GL_FLOAT,0,0,ver);
    glEnableVertexAttribArray(A_VER);
    //材质
    glVertexAttribPointer(T_VER,2,GL_FLOAT,0,0,tex);
    glEnableVertexAttribArray(T_VER);

    //从shader获取材质
    unis[0]=program.uniformLocation("tex_y");
    unis[1]=program.uniformLocation("tex_u");
    unis[2]=program.uniformLocation("tex_v");

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
}

void OpenGLWidget::resizeGL(int w, int h)
{
}


void OpenGLWidget::paintGL()
{
    if(width == 0 || height == 0 || datas[0]==nullptr ){
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        return;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texs[0]);   //0层绑定到Y材质
    //修改材质内容（复制内存内容）
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width,height,GL_RED,GL_UNSIGNED_BYTE,datas[0]);
    //与shader uni遍历关联
    glUniform1i(unis[0],0);

    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D,texs[1]);   //1层绑定到U材质
    //修改材质内容（复制内存内容）
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width/2,height/2,GL_RED,GL_UNSIGNED_BYTE,datas[1]);
    //与shader uni遍历关联
    glUniform1i(unis[1],1);

    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D,texs[2]);   //2层绑定到V材质
    //修改材质内容（复制内存内容）
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width/2,height/2,GL_RED,GL_UNSIGNED_BYTE,datas[2]);
    //与shader uni遍历关联
    glUniform1i(unis[2],2);

    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}
