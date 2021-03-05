#ifndef RENDERLOOP_H
#define RENDERLOOP_H
#include "utils.h"
#include "MatrixTransform.h"
#include <QObject>
#include "shader/phoneShader.h"
#include "renderCore/fragmentdata.h"
class RenderLoop : public QObject
{
     Q_OBJECT
public:
    explicit RenderLoop(int _width, int _height, QObject *parent = nullptr);
    ~RenderLoop() override;

    void triangleByBc(Vector3f *screen_point, Vector2f *uv, Vector3f *world_pos, Vector3f& normal, TGAImage &normalImg, float *zBuffer, TGAImage &image, float near, float far, int& tm);
    Vector3i texture(Vector2f& uv, TGAImage &image);
    Vector3f barycentric(Vector3f *pts, Vector2i& p);
    Vector3f cross(const Vector3f& a, const Vector3f& b);
    void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
    void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    void keyPressEvent(float speed, char type);
    void mouseMoveEvent(float pitch, float yaw);
    bool faceCulling(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3);
    Model *model;
    int width;
    int height;
    FrameBuffer frameBuffer;
    void stopRender();
    bool stop;
    clock_t start, finish;
    double deltaFrameTime;
    MatrixTransform* transform;
    Camera* camera;
    phoneShader* shader;
    Vector3f lightPos;
signals:
    void frameOut(unsigned char *image, double deltaFrameTime, int fps);

public slots:
    void loop();

};

#endif // RENDERLOOP_H
