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
    vec4 fragmentShader(IN FragmentData& fragmentData) override;

    void setDiffuseImage(TGAImage& image);
    void setNormalImage(TGAImage& image);

//    void vecMulvec(vec3& vec1, vec3& vec2, vec2& res);
    float ShadowCalculation(IN FragmentData &fragmentData, float bias);
    void setLightSpaceMat(mat4 mat);
    void setShadowMap(std::vector<float> shadowMap);
private:
    vec2 uv;
    TGAImage diffuseImg;
    TGAImage normalImg;
    mat4 lightSpaceMat;
    std::vector<float> shadowMap;
};

#endif
