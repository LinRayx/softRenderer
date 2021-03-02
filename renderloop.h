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
    ~RenderLoop() override;

    void triangleByBc(Vector3f *screen_point, Vector2f *uv, float *zBuffer, TGAImage &image, float near, float far);
    Vector3i texture(Vector2f& uv, TGAImage &image);
    Vector3f barycentric(Vector3f *pts, Vector2i& p);
    Vector3f cross(const Vector3f& a, const Vector3f& b);
    void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
    void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    void keyPressEvent(float speed, char type);
    void mouseMoveEvent(float pitch, float yaw);
    bool faceCulling(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3);
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
    void frameOut(unsigned char *image, double deltaFrameTime, int fps);

public slots:
    void loop();

};

#endif // RENDERLOOP_H
