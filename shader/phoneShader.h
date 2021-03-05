#ifndef PHONESHADER_H
#define PHONESHADER_H
#include "../renderCore/ishader.h"
#include "../renderCore/fragmentdata.h"
class phoneShader : public IShader {
public:
    phoneShader();
    ~phoneShader() override;
    Vector3f vertexShader(Vector3f pos) override;
    Vector4f fragmentShader(FragmentData& fragmentData) override;
    void setModelMat(Matrix4f&);
    void setViewMat(Matrix4f&);
    void setProjectionMat(Matrix4f&);
    void setUV(Vector2f& uv);
    void setDiffuseImage(TGAImage& image);
    void setNormalImage(TGAImage& image);
private:
    Matrix4f model;
    Matrix4f view;
    Matrix4f projection;
    Vector2f uv;
    TGAImage diffuseImg;
    TGAImage normalImg;
    float intensity;
};

#endif
