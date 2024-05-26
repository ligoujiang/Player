#include "OpenGLWidget.h"

#include <iostream>

#define A_VER 3
#define T_VER 4

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget{parent}{
}


void OpenGLWidget::initializeGL()
{
    initialize_yuv420p();
}

void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLint)w, (GLint)h);      //重置当前的视口
}

void OpenGLWidget::paintGL()
{
    render(m_py, m_pu, m_pv,m_linesize1,m_linesize2,m_linesize3,m_width,m_height); //yuv420p 软解
}

void OpenGLWidget::initialize_yuv420p()
{
    //初始化opengl （QOpenGLFunctions继承）函数
    initializeOpenGLFunctions();
    //顶点shader
    const char *vString =
        "attribute vec4 vertexPosition;\
        attribute vec2 textureCoordinate;\
        varying vec2 texture_Out;\
        void main(void)\
    {\
            gl_Position = vertexPosition;\
            texture_Out = textureCoordinate;\
    }";
        //片元shader
        const char *tString =
        "varying vec2 texture_Out;\
        uniform sampler2D tex_y;\
        uniform sampler2D tex_u;\
        uniform sampler2D tex_v;\
        void main(void)\
    {\
            vec3 YUV;\
            vec3 RGB;\
            YUV.x = texture2D(tex_y, texture_Out).r;\
            YUV.y = texture2D(tex_u, texture_Out).r - 0.5;\
            YUV.z = texture2D(tex_v, texture_Out).r - 0.5;\
            RGB = mat3(1.0, 1.0, 1.0,\
                   0.0, -0.39465, 2.03211,\
                   1.13983, -0.58060, 0.0) * YUV;\
            gl_FragColor = vec4(RGB, 1.0);\
    }";

        glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glClearDepth(1.0);                                  //设置深度缓存
    glEnable(GL_DEPTH_TEST);                            //启用深度测试
    glDepthFunc(GL_LEQUAL);                             //所作深度测试的类型
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  //告诉系统对透视进行修正


    //m_program加载shader（顶点和片元）脚本
    //片元（像素）
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, tString);
    //顶点shader
    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vString);

    //设置顶点位置
    m_program.bindAttributeLocation("vertexPosition",A_VER);
    //设置纹理位置
    m_program.bindAttributeLocation("textureCoordinate",T_VER);

    //编译shader,并检测电脑是否支持OpenGL
    m_program.link();
    m_program.bind();

    //传递顶点和纹理坐标
    //顶点
    static const GLfloat ver[] = {
        -1.0f,-1.0f,
        1.0f,-1.0f,
        -1.0f, 1.0f,
        1.0f,1.0f
    };
    //纹理
    static const GLfloat tex[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
    };

    //设置顶点,纹理数组并启用
    glVertexAttribPointer(A_VER, 2, GL_FLOAT, 0, 0, ver);
    glEnableVertexAttribArray(A_VER);
    glVertexAttribPointer(T_VER, 2, GL_FLOAT, 0, 0, tex);
    glEnableVertexAttribArray(T_VER);

    //从shader获取地址
    m_textureUniformY = m_program.uniformLocation("tex_y");
    m_textureUniformU = m_program.uniformLocation("tex_u");
    m_textureUniformV = m_program.uniformLocation("tex_v");

    //创建纹理
    glGenTextures(1, &m_idy);
    //Y
    glBindTexture(GL_TEXTURE_2D, m_idy);
    //放大过滤，线性插值   GL_NEAREST(效率高，但马赛克严重)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //U
    glGenTextures(1, &m_idu);
    glBindTexture(GL_TEXTURE_2D, m_idu);
    //放大过滤，线性插值
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //V
    glGenTextures(1, &m_idv);
    glBindTexture(GL_TEXTURE_2D, m_idv);
    //放大过滤，线性插值
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWidget::render(uchar *py, uchar *pu, uchar *pv,int linesize1,int linesize2,int linesize3, int width, int height)
{
    if(width == 0 || height == 0 || py==nullptr || pu==nullptr || pv==nullptr){
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        return;
    }

    // if(m_width==linesize1){ //无需对齐
    //     memcpy(py,py,width*height);
    //     memcpy(pu,pu,width*height/4);
    //     memcpy(pv,pv,width*height/4);
    //     std::cout<<"123"<<std::endl;
    // }else{//行对齐
    //     for(int i=0;i<m_height;i++){
    //         memcpy(py+width*i,py+linesize1*i,m_width);
    //     }
    //     for(int i=0;i<m_height/2;i++){
    //         memcpy(pu+width/2*i,pu+linesize2*i,m_width);
    //     }
    //     for(int i=0;i<m_height/2;i++){
    //         memcpy(pv+width/2*i,pv+linesize3*i,m_width);
    //     }
    // }

    //glLoadIdentity();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_idy);
    //修改纹理内容(复制内存内容)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE,py);
    //与shader 关联
    glUniform1i(m_textureUniformY, 0);

    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, m_idu);
    //修改纹理内容(复制内存内容)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width/2, height/2, 0, GL_RED, GL_UNSIGNED_BYTE, pu);
    //与shader 关联
    glUniform1i(m_textureUniformU,1);

    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, m_idv);
        //修改纹理内容(复制内存内容)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width/2, height/2, 0, GL_RED, GL_UNSIGNED_BYTE, pv);
    //与shader 关联
    glUniform1i(m_textureUniformV, 2);

    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

void OpenGLWidget::Init(uint8_t *m_py, uint8_t *m_pu, uint8_t *m_pv, int linesize1,int linesize2,int linesize3,int width, int height)
{
    this->m_py = m_py;
    this->m_pu = m_pu;
    this->m_pv = m_pv;
    this->m_linesize1=linesize1;
    this->m_linesize2=linesize2;
    this->m_linesize3=linesize3;
    m_width  = width;
    m_height = height;
    qDebug()<<"w:"<<m_width<<" h:"<<m_height;
    update(); //自动调用paintGL函数
}

void OpenGLWidget::recvYuv420pDataShow(uint8_t *m_py, uint8_t *m_pu, uint8_t *m_pv, int linesize1,int linesize2,int linesize3,int width, int height)
{
    this->m_py = m_py;
    this->m_pu = m_pu;
    this->m_pv = m_pv;
    this->m_linesize1=linesize1;
    this->m_linesize2=linesize2;
    this->m_linesize3=linesize3;
    m_width  = width;
    m_height = height;
    update(); //自动调用paintGL函数
}
