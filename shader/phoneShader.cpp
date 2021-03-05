#include "phoneShader.h"

phoneShader::phoneShader()
{

}

phoneShader::~phoneShader()
{

}


Vector3f phoneShader::vertexShader(Vector3f pos)
{

    Vector4f proj_pos = projection * view * model * Vector4f(pos[0], pos[1], pos[2], 1.0);

    Vector3f screen_pos = Vector3f(proj_pos[0]/proj_pos[3], proj_pos[1]/proj_pos[3], proj_pos[2]/proj_pos[3]);
    screen_pos[0] = (screen_pos[0] + 1.)/2 * SCREEN_WIDTH;
    screen_pos[1] = (screen_pos[1] + 1.)/2 * SCREEN_HEIGHT;
    return screen_pos;
}

Vector4f phoneShader::fragmentShader(FragmentData &fragmentData)
{
    Vector3f diffTex = texture(fragmentData.uv, diffuseImg);
    Vector3f normal = texture(fragmentData.uv, normalImg).normalized();
    Vector3f lightDir = (fragmentData.tangentLightPos - fragmentData.tangentWorldPos).normalized();
//    Vector3f viewDir = (fragmentData.tangentViewPos - fragmentData.tangentWorldPos).normalized();
//    Vector3f half = (viewDir + lightDir).normalized();
    float diff = std::max(lightDir.dot(normal), 0.0f);
    Vector3f diffuse = diff * diffTex * fragmentData.lightColor;
//    TGAColor color = diffuseImg.get(uv[0] * diffuseImg.get_width(), uv[1] * diffuseImg.get_height());
//    std::cout << color <<" " << screen_pos<< std::endl;
//    return Vector4i(color.raw[0], color.raw[1], color.raw[2], 255);
//    return Vector4i(color[0], color[1], color[2], 255);
    return Vector4f(diffuse.x(), diffuse.y(), diffuse.z(), 1.0);
}

void phoneShader::setModelMat(Matrix4f &m)
{
    model = m;
}

void phoneShader::setViewMat(Matrix4f &m)
{
    view = m;
}

void phoneShader::setProjectionMat(Matrix4f &m)
{
    projection = m;
}

void phoneShader::setUV(Vector2f& uv) {
    this->uv = uv;
}

void phoneShader::setDiffuseImage(TGAImage& image) {
    diffuseImg = image;
}

void phoneShader::setNormalImage(TGAImage &image)
{
    normalImg = image;
}
