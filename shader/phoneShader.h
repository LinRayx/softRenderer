#include "../renderCore/ishader.h"
#include "../renderCore/fragmentdata.h"
#include "../renderCore/vertexdata.h"
#include "../utils.h"
#ifndef PHONESHADER_H
#define PHONESHADER_H

class PhoneShader : public IShader {
public:
    PhoneShader(int &w, int &h);
    ~PhoneShader() override;
    void vertexShader(IN VertexData& vertexData, OUT FragmentData& fragmentData) override;
    Vector4f fragmentShader(IN FragmentData& fragmentData) override;

    void setDiffuseImage(TGAImage& image);
    void setNormalImage(TGAImage& image);
    void vecMulvec(Vector3f& vec1, Vector3f& vec2, Vector3f& res);
    float ShadowCalculation(FragmentData &fragmentData, float bias);
    void setLightSpaceMat(Matrix4f mat);
private:
    Vector2f uv;
    TGAImage diffuseImg;
    TGAImage normalImg;
    Matrix4f lightSpaceMat;
};

#endif
