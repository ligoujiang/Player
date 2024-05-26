#ifndef RESLIDER_H
#define RESLIDER_H

#include <QObject>
#include <QMouseEvent>
#include <QSlider>

class ReSlider:public QSlider
{
    Q_OBJECT
public:
    ReSlider(QWidget *parent=NULL);
    ~ReSlider();
    void mousePressEvent(QMouseEvent *event);
};

#endif // RESLIDER_H
