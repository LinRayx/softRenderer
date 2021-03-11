#include "../renderCore/ishader.h"
#include "../utils.h"
#ifndef SHADOWSHADER_H
#define SHADOWSHADER_H


class ShadowShader : public IShader
{
public:
    ShadowShader(int& w, int& h);
    ~ShadowShader() override;

    // IShader interface
    void vertexShader(IN VertexData &vertexData, OUT FragmentData &fragmentData) override;
    vec4 fragmentShader(IN FragmentData &fragmentData) override;
    float near, far;
};

#endif // SHADOWSHADER_H
