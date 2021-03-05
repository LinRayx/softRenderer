#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    canvas = nullptr;
    pitch = yaw = 0;
    deltaFrameTime = 0;
    fps = 0;
    loop = new RenderLoop(300, 300);

    loopThread = new QThread(this);
    loop->moveToThread(loopThread);
    connect(loopThread,&QThread::started,loop,&RenderLoop::loop);
    connect(loop,&RenderLoop::frameOut,this,&Widget::receiveFrame);

    loopThread->start();
    QTimer* timer = new QTimer();
    timer = new QTimer();
    connect(timer,&QTimer::timeout,this,&Widget::fpsTimeOut);
    timer->start(1000);
}

Widget::~Widget()
{
    delete ui;
    loop->stopRender();
    loopThread->quit();
    loopThread->wait();
    if(canvas) delete canvas;
    if(loopThread)delete loopThread;
    if(loop) delete loop;
    loop = nullptr;
    canvas = nullptr;
    loopThread = nullptr;
}

void Widget::paintEvent(QPaintEvent*) {
    if(canvas) {
        QPainter painter(this);
        painter.drawImage(0, 0, *canvas);
    }
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    lastX = e->pos().x();
    lastY = e->pos().y();
}

void Widget::mouseMoveEvent(QMouseEvent *e)
{
    float nx = e->pos().x();
    float ny = e->pos().y();

    float xoffset = (nx - lastX) * deltaFrameTime;
    float yoffset = (lastY - ny) * deltaFrameTime;

    lastX = nx;
    lastY = ny;

    float sensitiviry = 0.05;
    xoffset *= sensitiviry;
    yoffset *= sensitiviry;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    loop->mouseMoveEvent(pitch, yaw);

}

void Widget::receiveFrame(unsigned char* image, double deltaFrameTime, int fps) {
    if(canvas)
        delete canvas;

    canvas = new QImage(image, width(), height(), QImage::Format_RGBA8888);
    this->deltaFrameTime = deltaFrameTime;
    this->fps = fps - this->fps;
    update();

}

void Widget::fpsTimeOut()
{
    this->setWindowTitle(QString("SoftRenderer deltaFrame: %1 fps: %2").arg(this->deltaFrameTime));
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    float speed = 0.5f * deltaFrameTime;
    switch (e->key()) {
    case Qt::Key_Escape:
        this->close();
        break;
    case Qt::Key_W:
        loop->keyPressEvent(speed, 'W');
        break;
    case Qt::Key_S:
        loop->keyPressEvent(speed, 'S');
        break;
    case Qt::Key_A:
        loop->keyPressEvent(speed, 'A');
        break;
    case Qt::Key_D:
        loop->keyPressEvent(speed, 'D');
        break;

    }
}
