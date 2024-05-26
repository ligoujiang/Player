#include "ReSlider.h"

ReSlider::ReSlider(QWidget *parent)
    :QSlider(parent)
{}

ReSlider::~ReSlider()
{

}

void ReSlider::mousePressEvent(QMouseEvent *event)
{
    double pos=(double)event->pos().x()/(double)width();
    setValue(pos*this->maximum());
    //原有事件处理
    QSlider::mousePressEvent(event);    //这个不能点击到最尾处
    //QSlider::sliderReleased(); //用这个的话不能滑动进度条
}

