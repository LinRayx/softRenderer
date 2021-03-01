#ifndef RENDERLOOP_H
#define RENDERLOOP_H
#include "utils.h"
#include "MatrixTransform.h"
#include <QObject>

class RenderLoop : public QObject
{
     Q_OBJECT
public:
    explicit RenderLoop(int _width, int _height, QObject *parent = nullptr);

    void triangleByBc(Vec3f *screen_point, Vec2f* uv, float *zBuffer, TGAImage &image, Vec3f *triangleColor, float near, float far);
    Vec3f texture(Vec2f uv, TGAImage &image);
    Vec3f barycentric(Vec3f *pts, Vec2i p);
    Vec3f cross(const Vec3f& a, const Vec3f& b);
    void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
    void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    void keyPressEvent(float speed, char type);
    void mouseMoveEvent(float pitch, float yaw);
    Model *model;
    int width  = 800;
    int height = 600;
    FrameBuffer frameBuffer;
    void stopRender();
    bool stop;
    clock_t start, finish;
    double deltaFrameTime;
    MatrixTransform* transform;
    Camera* camera;
signals:
    void frameOut(unsigned char *image, double deltaFrameTime);

public slots:
    void loop();

};

#endif // RENDERLOOP_H
