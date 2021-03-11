#include "PhoneShader.h"

PhoneShader::PhoneShader(int& w, int& h): IShader (w, h)
{
}

PhoneShader::~PhoneShader()
{

}


void PhoneShader::vertexShader(IN VertexData& vertexData, OUT FragmentData& fragmentData)
{
    vec4 homo_pos = vec4(vertexData.vertex_pos, 1.0);
    vec3 world_pos = vec3(model * homo_pos);
    vec4 proj_pos = projection * view * model * homo_pos;
    vec4 screen_pos = vec4(vec3(proj_pos)/proj_pos.w, proj_pos.w);
    screen_pos[0] = (screen_pos[0] + 1.f)/2 * SCREEN_WIDTH;
    screen_pos[1] = (screen_pos[1] + 1.f)/2 * SCREEN_HEIGHT;
    screen_pos[2] = (screen_pos[2] + 1.f)/2;
    fragmentData.screen_pos = screen_pos;



    vec4 lightSpacePos = lightSpaceMat * model * homo_pos;
    fragmentData.lightSpacePos[0] = (lightSpacePos[0] + 1.f)/2 * SCREEN_WIDTH;
    fragmentData.lightSpacePos[1] = (lightSpacePos[1] + 1.f)/2 * SCREEN_HEIGHT;
    fragmentData.lightSpacePos[2] = (lightSpacePos[2] + 1.f)/2;


    // 转换到切线空间
    mat3 TBN = vertexData.TBN;
    fragmentData.lightDir = TBN * (vertexData.light_pos - world_pos);
    fragmentData.viewDir = TBN * (vertexData.view_pos - world_pos);
    fragmentData.uv = vertexData.uv;
    fragmentData.world_pos = world_pos;
}



vec4 PhoneShader::fragmentShader(IN FragmentData &fragmentData)
{

    // 在切线空间下计算
    vec3 ambient = vec3(0.15f, 0.15f, 0.15f);
    vec3 diffTex = texture(fragmentData.uv, diffuseImg);
    vec3 normal = texture(fragmentData.uv, normalImg);
    std::swap(normal[0], normal[2]);
    vec3 lightDir = normalize(fragmentData.lightDir);
    vec3 viewDir = normalize(fragmentData.viewDir);
    normal = normalize((normal * 2.0f - vec3(1.f)));

//    Util::printVec3(normal);
//    Util::printVec3(lightDir);
//    Util::printVec3(viewDir);
    vec3 half = normalize(viewDir + lightDir);
    float diff = std::max(dot(lightDir,normal), 0.0f);
//    std::cout << diff << std::endl;
    vec3 diffuse = diffTex * diff;

    float spec = std::pow(std::max(dot(normal, half), 0.0f), 32.0f);
    vec3 specular = vec3(0.1f) * spec;

    // shadow
    float bias = std::max(0.5f * (1.0f - dot(normal, lightDir)), 0.1f);
    float shadow = ShadowCalculation(fragmentData, bias);

    vec3 color = (diffuse + specular) * (1-shadow) + ambient;
//    Util::printVec3(color);
    return vec4(color, 1.0);
}

float PhoneShader::ShadowCalculation(FragmentData &fragmentData, float bias) {
    float x = fragmentData.lightSpacePos[0];
    float y = fragmentData.lightSpacePos[1];

    float shadow = 0;

    if (shadowMap.size() > 0) {
        float clostDepth = shadowMap[size_t(x+y * SCREEN_WIDTH)];
        std::cout << fragmentData.lightSpacePos[2] << " " << clostDepth << " " << x+y * SCREEN_WIDTH <<" " << SCREEN_WIDTH  << std::endl;
        if (fragmentData.lightSpacePos[2]-bias > clostDepth) {
            shadow = 1;
        }
    }
    return shadow;
}

void PhoneShader::setLightSpaceMat(mat4 mat)
{
    lightSpaceMat = mat;
}

void PhoneShader::setShadowMap(std::vector<float> shadowMap)
{
    this->shadowMap = shadowMap;
}

void PhoneShader::setDiffuseImage(TGAImage& image) {
    diffuseImg = image;
}

void PhoneShader::setNormalImage(TGAImage &image)
{
    normalImg = image;
}
