#ifndef PHONESHADER_H
#define PHONESHADER_H
#include "../renderCore/ishader.h"
#include "../renderCore/fragmentdata.h"
#include "../renderCore/vertexdata.h"
#include "../utils.h"
class phoneShader : public IShader {
public:
    phoneShader();
    ~phoneShader() override;
    void vertexShader(IN VertexData& vertexData, OUT FragmentData& fragmentData) override;
    Vector4f fragmentShader(IN FragmentData& fragmentData) override;
    void setModelMat(Matrix4f&);
    void setViewMat(Matrix4f&);
    void setProjectionMat(Matrix4f&);
    void setUV(Vector2f& uv);
    void setDiffuseImage(TGAImage& image);
    void setNormalImage(TGAImage& image);
    void vecMulvec(Vector3f& vec1, Vector3f& vec2, Vector3f& res);
private:
    Matrix4f model;
    Matrix4f view;
    Matrix4f projection;
    Vector2f uv;
    TGAImage diffuseImg;
    TGAImage normalImg;
};

#endif
