#include "../utils.h"
#include "../renderCore/fragmentdata.h"
#include "../renderCore/vertexdata.h"
#include "../renderCore/tgaimage.h"

#ifndef ISHADER_H
#define ISHADER_H
#define OUT
#define IN
class IShader
{
public:
    IShader(int& w, int& h);
    virtual ~IShader();
    virtual void vertexShader(IN VertexData& vertexData, OUT FragmentData& fragmentData) = 0;
    virtual Vector4f fragmentShader(IN FragmentData& fragmentData) = 0;
    float* GetZBuffer();
    void ClearZBuffer();
    bool SetZBuffer(int x, int y, float z);
    Vector3f texture(Vector2f uv, TGAImage &image);
    void setScreenSize(int w, int h);
    void setModelMat(Matrix4f&);
    void setViewMat(Matrix4f&);
    void setProjectionMat(Matrix4f&);
protected:
    Matrix4f model;
    Matrix4f view;
    Matrix4f projection;
    float* zBuffer;
    int SCREEN_HEIGHT, SCREEN_WIDTH;
};

#endif // ISHADER_H
