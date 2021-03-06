#ifndef RENDERLOOP_H
#define RENDERLOOP_H
#include "utils.h"
#include "MatrixTransform.h"
#include <QObject>
#include "shader/phoneShader.h"
#include "renderCore/fragmentdata.h"
#include "renderCore/FrameBuffer.h"
#include "renderCore/model.h"
#include "renderCore/tgaimage.h"
class RenderLoop : public QObject
{
     Q_OBJECT
public:
    explicit RenderLoop(int _width, int _height, QObject *parent = nullptr);
    ~RenderLoop() override;

    void triangleByBc(FragmentData *fragmentData, float *zBuffer, int& tm);
    Vector3f barycentric(FragmentData *fragmentData, Vector2i& p);
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
    TGAImage diffuseImg, normalImg;

signals:
    void frameOut(unsigned char *image, double deltaFrameTime, int fps);

public slots:
    void loop();

};

#endif // RENDERLOOP_H
