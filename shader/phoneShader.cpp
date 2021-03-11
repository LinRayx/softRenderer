#include "PhoneShader.h"

PhoneShader::PhoneShader(int& w, int& h): IShader (w, h)
{
}

PhoneShader::~PhoneShader()
{

}


void PhoneShader::vertexShader(IN VertexData& vertexData, OUT FragmentData& fragmentData)
{
    Vector4f homo_pos = Vector4f(vertexData.vertex_pos[0],vertexData.vertex_pos[1],vertexData.vertex_pos[2], 1.0);
    Vector4f homo_world_pos = model * homo_pos;
    Vector3f world_pos = Vector3f(homo_world_pos[0], homo_world_pos[1], homo_world_pos[2]);
    Vector4f proj_pos = projection * view * model * homo_pos;
    Vector3f screen_pos = Vector3f(proj_pos[0]/proj_pos[3], proj_pos[1]/proj_pos[3], proj_pos[2]/proj_pos[3]);
    Vector4f lightSpacePos = lightSpaceMat * homo_pos;;
    Matrix3f TBN = vertexData.TBN;

    fragmentData.lightSpacePos = Vector3f(lightSpacePos[0]/lightSpacePos[3], lightSpacePos[1]/lightSpacePos[3], lightSpacePos[2]/lightSpacePos[3]);
    screen_pos[0] = (screen_pos[0] + 1.f)/2 * SCREEN_WIDTH;
    screen_pos[1] = (screen_pos[1] + 1.f)/2 * SCREEN_HEIGHT;
    fragmentData.lightSpacePos[0] = (fragmentData.lightSpacePos[0] + 1.f)/2 * SCREEN_WIDTH;
    fragmentData.lightSpacePos[1] = (fragmentData.lightSpacePos[1] + 1.f)/2 * SCREEN_HEIGHT;

//    std::cout << fragmentData.lightSpacePos[0] << " " << fragmentData.lightSpacePos[1] << " " << fragmentData.lightSpacePos[2]  <<std::endl;
//    std::cout << proj_pos[0] << " " << proj_pos[1] << " " << proj_pos[2]  << " " << proj_pos[3] <<std::endl;
    fragmentData.screen_pos = screen_pos;
    // 转换到切线空间
    fragmentData.lightDir = TBN * (vertexData.light_pos - world_pos);
    fragmentData.viewDir = TBN * (vertexData.view_pos - world_pos);
    fragmentData.uv = vertexData.uv;
    fragmentData.world_pos = world_pos;
}

void PhoneShader::vecMulvec(Vector3f& vec1, Vector3f& vec2, Vector3f& res) {
    res[0] = vec1[0] * vec2[0];
    res[1] = vec1[1] * vec2[1];
    res[2] = vec1[2] * vec2[2];
}

Vector4f PhoneShader::fragmentShader(IN FragmentData &fragmentData)
{
    // 在切线空间下计算
    Vector3f ambient = Vector3f(0.05f, 0.05f, 0.05f);
    Vector3f diffTex = texture(fragmentData.uv, diffuseImg);
//    std::cout << diffTex << std::endl;
    Vector3f normal = texture(fragmentData.uv, normalImg);
    std::swap(normal[0], normal[2]);
    Vector3f lightDir = fragmentData.lightDir.normalized();
    Vector3f viewDir = fragmentData.viewDir.normalized();
    normal = (normal * 2.0 - Vector3f(1., 1., 1.)).normalized();

//    std::cout << "normal: " << normal << "lightDir: "<< lightDir<< std::endl;
    Vector3f half = (viewDir + lightDir).normalized();
    float diff = std::max(lightDir.dot(normal), 0.0f);
    Vector3f diffuse;
    vecMulvec(diffTex, fragmentData.lightColor, diffuse);
    diffuse *= diff;
    float spec = std::pow(std::max(normal.dot(half), 0.0f), 32.0f);
    Vector3f specular = Vector3f(0.1f, 0.1f, 0.1f) * spec;

    // shadow
    float bias = std::max(0.5f * (1.0f - normal.dot(lightDir)), 0.1f);
//    std::cout << bias << std::endl;
    float shadow = ShadowCalculation(fragmentData, bias);

    Vector3f color = (diffuse + specular) * (1-shadow) + ambient;
    return Vector4f(color[0], color[1], color[2], 1.0);
}

float PhoneShader::ShadowCalculation(FragmentData &fragmentData, float bias) {
    float x = fragmentData.lightSpacePos[0];
    float y = fragmentData.lightSpacePos[1];

    float shadow = 0;

    if (fragmentData.shadowMap.size() > 0) {
        float clostDepth = fragmentData.shadowMap[size_t(x+y * SCREEN_WIDTH)];
//        std::cout << fragmentData.screen_pos[2] << " " << clostDepth << " " << x <<" " << y  << std::endl;
        if (fragmentData.screen_pos[2]-bias > clostDepth) {
            shadow = 1;
        }
    }
    return shadow;
}

void PhoneShader::setLightSpaceMat(Matrix4f mat)
{
    lightSpaceMat = mat;
}

void PhoneShader::setDiffuseImage(TGAImage& image) {
    diffuseImg = image;
}

void PhoneShader::setNormalImage(TGAImage &image)
{
    normalImg = image;
}
