#include "ShadowShader.h"

ShadowShader::ShadowShader(int& w, int& h):IShader (w, h)
{

}

ShadowShader::~ShadowShader()
{

}

void ShadowShader::vertexShader(IN VertexData &vertexData, OUT FragmentData &fragmentData)
{
    Vector4f homo_pos = projection * view * model * Vector4f(vertexData.vertex_pos[0],vertexData.vertex_pos[1],vertexData.vertex_pos[2], 1.0);
    fragmentData.screen_pos = Vector3f(homo_pos[0]/homo_pos[3], homo_pos[1]/homo_pos[3], homo_pos[2]/homo_pos[3]);
    fragmentData.screen_pos[0] = (fragmentData.screen_pos[0] + 1.f)/2 * SCREEN_WIDTH;
    fragmentData.screen_pos[1] = (fragmentData.screen_pos[1] + 1.f)/2 * SCREEN_HEIGHT;
}

Vector4f ShadowShader::fragmentShader(IN FragmentData &fragmentData)
{
    float z = fragmentData.screen_pos[2];


    return Vector4f(z, z, z, 1);
}
