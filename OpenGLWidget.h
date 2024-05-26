#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H
#include <QWidget>>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class OpenGLWidget : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWidget(QWidget *parent = nullptr);

    void Init(uint8_t *m_py, uint8_t *m_pu, uint8_t *m_pv, int linesize1,int linesize2,int linesize3,int width, int height);
protected:
    //初始化
    virtual void initializeGL() override;
    //尺寸变化
    virtual void resizeGL(int w, int h) override;
    //刷新显示
    virtual void paintGL() override;

    void initialize_yuv420p();
    void render(uchar* py,uchar* pu,uchar* pv,int linesize1,int linesize2,int linesize3,int width,int height);

private:
    //shader程序
    QOpenGLShaderProgram m_program;

    GLuint m_textureUniformY={0};
    GLuint m_textureUniformU={0};
    GLuint m_textureUniformV={0};

    //opengl的texture地址
    GLuint m_idy={0};
    GLuint m_idu={0};
    GLuint m_idv={0};

    uint8_t* m_py=NULL;
    uint8_t* m_pu=NULL;
    uint8_t* m_pv=NULL;
    int m_width=0;
    int m_height=0;
    int m_linesize1=0;
    int m_linesize2=0;
    int m_linesize3=0;

public slots:
    void recvYuv420pDataShow(uint8_t* m_py, uint8_t *m_pu, uint8_t*m_pv, int linesize1,int linesize2,int linesize3,int width, int height);
};

#endif // OPENGLWIDGET_H
