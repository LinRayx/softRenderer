#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    canvas = nullptr;
    deltaFrameTime = 0;
    loop = new RenderLoop(800, 600);
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

void Widget::receiveFrame(unsigned char* image, double deltaFrameTime) {
    if(canvas)
        delete canvas;

    canvas = new QImage(image, 800, 600, QImage::Format_RGBA8888);
    this->deltaFrameTime = deltaFrameTime;
    update();

}

void Widget::fpsTimeOut()
{
    this->setWindowTitle(QString("SoftRenderer deltaFrame: %1").arg(this->deltaFrameTime));
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Escape:
        this->close();
    }
}
