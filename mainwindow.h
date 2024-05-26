#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <OpenGLWidget.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    //定时器 进度条显示
    void timerEvent(QTimerEvent *e);

private slots:
    void on_pushButton_clicked();

    void on_OpenFileBtn_clicked();

    void on_Slider_sliderPressed();

    void on_Slider_sliderReleased();

public:
    Ui::MainWindow *ui;
    //进度条点击状态
    bool isSliderPress=false;
signals:
    //void emitMP4Yuv420pData(uint8_t *m_py, uint8_t *m_pu, uint8_t *m_pv,int linesize1,int linesize2,int linesize3, int width, int height);
};
#endif // MAINWINDOW_H
