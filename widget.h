#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include "renderloop.h"
#include <QThread>
#include <QTimer>
#include <QKeyEvent>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

    void receiveFrame(unsigned char* image, double deltaFrameTime);
    void fpsTimeOut();
protected:
    void keyPressEvent(QKeyEvent *) override;
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent* ) override;
    void mouseMoveEvent(QMouseEvent* ) override;
private:

    Ui::Widget *ui;
    QImage *canvas;
    RenderLoop* loop;
    QThread* loopThread;
    double deltaFrameTime;
    float lastX, lastY;
    float yaw, pitch;
};

#endif // WIDGET_H
