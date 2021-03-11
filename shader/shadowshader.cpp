#include "ShadowShader.h"

ShadowShader::ShadowShader(int& w, int& h):IShader (w, h)
{

}

ShadowShader::~ShadowShader()
{

}

void ShadowShader::vertexShader(IN VertexData &vertexData, OUT FragmentData &fragmentData)
{
    vec4 homo_pos = projection * view * model * vec4(vertexData.vertex_pos, 1.0);
    fragmentData.screen_pos = vec4(vec3(homo_pos)/homo_pos.w, homo_pos.w);
    fragmentData.screen_pos[0] = (fragmentData.screen_pos[0] + 1.f)/2 * SCREEN_WIDTH;
    fragmentData.screen_pos[1] = (fragmentData.screen_pos[1] + 1.f)/2 * SCREEN_HEIGHT;
    fragmentData.screen_pos[2] = (fragmentData.screen_pos[2] + 1.f)/2;
//    std::cout << homo_pos.w << " " << fragmentData.screen_pos[0] << " " << fragmentData.screen_pos[1] << " " << fragmentData.screen_pos[2] << std::endl;
}

vec4 ShadowShader::fragmentShader(IN FragmentData &fragmentData)
{
    float z = fragmentData.screen_pos[2];

//    std::cout << "shadowPass: " << z << std::endl;
//    std::cout << z << std::endl;
    return vec4(vec3(z), 1);
}
