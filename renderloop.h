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
    void stopRender();
    void keyPressEvent(float speed, char type);
    void mouseMoveEvent(float pitch, float yaw);
private:
    void triangleByBc(FragmentData *fragmentData, IShader *shader, int& tm);
    Vector3f barycentric(FragmentData *fragmentData, Vector2i& p);
    bool faceCulling(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3);
    void GPUStage(IShader *shader, VertexData *vertexData);
    void Pass(IShader *shader);

    Model *model;
    int width;
    int height;
    FrameBuffer frameBuffer;
    FrameBuffer shadowMap;
    bool stop;
    clock_t start, finish;
    double deltaFrameTime;
    MatrixTransform* transform;
    Camera* camera;
    PhoneShader* phoneShader;
    Vector3f lightPos;
    TGAImage diffuseImg, normalImg;

signals:
    void frameOut(unsigned char *image, double deltaFrameTime, int fps);

public slots:
    void loop();

};

#endif // RENDERLOOP_H
