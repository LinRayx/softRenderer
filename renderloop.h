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
#include "shader/shadowshader.h"
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
    void triangleByBc(FragmentData *fragmentData, IShader *shader, bool shadow = false);
    void triangleByBcOrtho(FragmentData *fragmentData, IShader *shader);
    void ClearZbuffer();
    bool setZBuffer(int x, int y, float z);
    bool faceCulling(const vec3& v1, const vec3& v2, const vec3& v3);
    void GPUStage(IShader *shader, VertexData *vertexData, bool shadow);
    void do_MSAA(FragmentData *fragmentData);
    void Pass(IShader *shader);
    void renderFloor(IShader* shader, Model* model);
    Model *model,  *floorModel;
    int width;
    int height;
    FrameBuffer frameBuffer;
    std::vector<float> shadowMap;
    bool stop;
    clock_t start, finish;
    double deltaFrameTime;
    MatrixTransform* transform;
    Camera* camera;
    PhoneShader* phoneShader;
    ShadowShader* shadowShader;
    PhoneShader* floorShader;
    vec3 lightPos;
    TGAImage diffuseImg, normalImg;
    float far, near;
    float* zbuffer;
    void shadowPass(IShader *shader, Model *floorModel);
    bool MSAA = true;
    float* MSAA_zbuffer;

//    std::vector<vec4>colorBuffer;
    std::vector< std::vector<bool> > pixelMask;
    vec3 barycentric(const vec2 &pos1, const vec2 &pos2, const vec2 &pos3, const vec2 &p);
signals:
    void frameOut(unsigned char *image, double deltaFrameTime, int fps);

public slots:
    void loop();

};

#endif // RENDERLOOP_H
