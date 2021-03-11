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
    virtual vec4 fragmentShader(IN FragmentData& fragmentData) = 0;
    float* GetZBuffer();
    void ClearZBuffer();
    bool SetZBuffer(int x, int y, float z);
    vec3 texture(vec2 uv, TGAImage &image);
    void setScreenSize(int w, int h);
    void setModelMat(mat4&);
    void setViewMat(mat4&);
    void setProjectionMat(mat4&);
    void setZBuffer(float* zb);
protected:
    mat4 model;
    mat4 view;
    mat4 projection;
    float* zBuffer;
    int SCREEN_HEIGHT, SCREEN_WIDTH;
};

#endif // ISHADER_H
