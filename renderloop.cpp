#include "renderloop.h"
RenderLoop::RenderLoop(int _width, int _height, QObject *parent) : QObject(parent), width(_width), height(_height)
{
    stop = false;
    camera = new Camera;
    transform = new MatrixTransform;
    phoneShader = new PhoneShader(_width, _height);
    shadowShader = new ShadowShader(_width, _height);
    floorShader = new PhoneShader(_width, _height);
    zbuffer = new float[width * height];
    far = 50.f;
    near = 0.1f;
}

RenderLoop::~RenderLoop()
{

    if(model) delete model;

    if(camera) delete camera;

    if(transform) delete transform;

    if(phoneShader) delete phoneShader;

    if (shadowShader) delete shadowShader;

    if (zbuffer) delete[] zbuffer;

}

void RenderLoop::loop()
{
    frameBuffer.Resize(width, height);
    shadowMap.resize(size_t(width * height));
    TGAImage diffuseImg, normalImg, floorDiffuseImg, floorNormalImg;
    std::string diffuseImgPath = APP_PATH + "image" + OS_FILE_INTERVEL + "african_head_diffuse.tga";
    std::string normalImgPath = APP_PATH + "image" + OS_FILE_INTERVEL + "african_head_nm_tangent.tga";
    if (diffuseImg.read_tga_file(diffuseImgPath.c_str()) == false) return;
    diffuseImg.flip_vertically();
    if (normalImg.read_tga_file(normalImgPath.c_str()) == false) return;
    normalImg.flip_vertically();
    std::string objPath = APP_PATH + "obj" + OS_FILE_INTERVEL + "african_head.obj";
    model = new Model(objPath.c_str());

    std::string floorDiffuseImgPath = APP_PATH + "image" + OS_FILE_INTERVEL + "floor_diffuse.tga";
    std::string floorNormalImgPath = APP_PATH + "image" + OS_FILE_INTERVEL + "floor_nm_tangent.tga";
    if (floorDiffuseImg.read_tga_file(floorDiffuseImgPath.c_str()) == false) return;
    floorDiffuseImg.flip_vertically();
    if (floorNormalImg.read_tga_file(floorNormalImgPath.c_str()) == false) return;
    floorNormalImg.flip_vertically();
    std::string floorObjPath = APP_PATH + "obj" + OS_FILE_INTERVEL + "floor.obj";

    Model* floorModel = new Model(floorObjPath.c_str());

    camera->SetPosition(vec3(3, 0, 3));
    int fps = 0;
    lightPos = vec3(3, 0, 3);
    phoneShader->setDiffuseImage(diffuseImg);
    phoneShader->setNormalImage(normalImg);

    shadowShader->far = far;
    shadowShader->near = near;

    floorShader->setDiffuseImage(floorDiffuseImg);
    floorShader->setNormalImage(floorNormalImg);
//    while(!stop) {
    ClearZbuffer();
        frameBuffer.ClearColorBuffer(Vec4c(0, 0, 0, 0));
        start = clock();
//        shadowPass(shadowShader, floorModel);
//        phoneShader->setShadowMap(shadowMap);
        Pass(phoneShader);
        renderFloor(floorShader, floorModel);
        finish = clock();
        deltaFrameTime = static_cast<double>(finish-start)/CLOCKS_PER_SEC;
        emit frameOut(frameBuffer.data(), deltaFrameTime, fps);
//    }
    qDebug() << "render quit";
}

void RenderLoop::shadowPass(IShader* shader, Model* floorModel) {
    vec3 viewPos = lightPos;
    vec3 center = vec3(0, 0, 0);
    vec3 up = vec3(0, 1, 0);
    float rot = 0;
    mat4 rotate = transform->GetRotationMatrix4x4(vec3(0, rot, 0));
    mat4 view = transform->GetLookAtMatrix4x4(viewPos, center, up);
//    Matrix4f projection = transform->GetPersepectiveMatrix4x4(transform->Radians(45), 1.f*width/height, near, far);
    mat4 projection = transform->GetOrthoMatrix4x4(10.f, 10.f, 1.f, 10.f);
    phoneShader->setLightSpaceMat(projection * view);
    VertexData vertexData[3];
    shader->setModelMat(rotate);
    shader->setViewMat(view);
    shader->setProjectionMat(projection);
    for (size_t i = 0; i < model->size(); ++i) {
        for (size_t j = 0; j < 3; ++j) {
        // 初始化vertex shader数据
            model->getTriangleVertex(vertexData[j].vertex_pos, i, j);
        }
        GPUStage(shader, vertexData, true);
    }
    mat4 modelMat = transform->GetModelMatrix4x4(vec3(0, 0, 0), vec3(0, 0, 0), vec3(2, 2, 2));
    shader->setModelMat(modelMat);
    for (size_t i = 0; i < floorModel->size(); ++i) {
        for (size_t j = 0; j < 3; ++j) {
        // 初始化vertex shader数据
            floorModel->getTriangleVertex(vertexData[j].vertex_pos, i, j);
        }
        GPUStage(shader, vertexData, true);
    }
}

// 这部分代码还是太冗余了
void RenderLoop::Pass(IShader* shader)
{
    vec3 center = vec3(0, 0, 0);
    vec3 up = vec3(0, 1, 0);
    float rot = 0;
    if (rot > 360) rot = 0;
//        rot = rot + 300 * speed;
    rot = 0;

    vec3 viewPos = camera->GetPosition();
    mat4 modelMat = transform->GetRotationMatrix4x4(vec3(0, rot, 0));
    mat4 view = transform->GetLookAtMatrix4x4(viewPos, center, up);
    mat4 projection = transform->GetPersepectiveMatrix4x4(transform->Radians(45), 1.f*width/height, near, far);

    VertexData vertexData[3];
    shader->setModelMat(modelMat);
    shader->setViewMat(view);
    shader->setProjectionMat(projection);
    // 几何阶段
    for (size_t i = 0; i < model->size(); ++i) {
        for (size_t j = 0; j < 3; ++j) {
        // 初始化vertex shader数据
            model->getTriangleVertex(vertexData[j].vertex_pos, i, j);
            model->getTriangleUv(vertexData[j].uv, i, j);
            model->getTriangleNormal(vertexData[j].normal, i, j);
            vertexData[j].light_pos = lightPos;
            vertexData[j].view_pos = viewPos;
        }
        // 计算每个点的切线空间矩阵，应该放在顶点着色器中，传出切线空间的信息
        vec3 E1 = vertexData[1].vertex_pos - vertexData[0].vertex_pos;
        vec3 E2 = vertexData[2].vertex_pos - vertexData[0].vertex_pos;
        vec2 deltaUV1 = vertexData[1].uv - vertexData[0].uv;
        vec2 deltaUV2 = vertexData[2].uv - vertexData[0].uv;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        mat3 TBN[3];

        vec3 tangentTri = normalize(vec3(f * (deltaUV2.y * E1.x - deltaUV1.y * E2.x),
                                         f * (deltaUV2.y * E1.y - deltaUV1.y * E2.y),
                                         f * (deltaUV2.y * E1.z - deltaUV1.y * E2.z)));
//            Vector3f bitangent = Vector3f(-f * (deltaUV2.y() * E1.x() + deltaUV1.y() * E2.x()), -f * (deltaUV2.y() * E1.y() + deltaUV1.y() * E2.y()), -f * (deltaUV2.y() * E1.z() + deltaUV1.y() * E2.z())).normalized();
        // 每个点都要算切线空间

        for (int j = 0; j < 3; ++j) {
            vec3 tangent = modelMat * vec4(normalize(tangentTri - dot(tangentTri, vertexData[j].normal) * vertexData[j].normal),0.f);
            vec3 bitangent = modelMat * vec4(normalize(cross(tangent,vertexData[j].normal)), 0.f);
            vec3 normal = modelMat * vec4(vertexData[j].normal, 0.f);
            TBN[j] = mat3(-tangent, bitangent, normal); // x轴取反
            vertexData[j].TBN = TBN[j];
            // 开始vertex shader
        }
        GPUStage(shader, vertexData, false);
    }
}

void RenderLoop::renderFloor(IShader *shader, Model *model)
{
    vec3 center = vec3(0, 0, 0);
    vec3 up = vec3(0, 1, 0);
    float rot = 0;
    if (rot > 360) rot = 0;
//        rot = rot + 300 * speed;
    rot = 0;
    vec3 viewPos = camera->GetPosition();
//    mat4 modelMat = transform->GetTranslationMatrix4x4(vec3(0, 0, 0));
    mat4 modelMat = transform->GetModelMatrix4x4(vec3(0, 0, 0), vec3(0, 0, 0), vec3(2, 2, 2));
    mat4 view = transform->GetLookAtMatrix4x4(viewPos, center, up);
    mat4 projection = transform->GetPersepectiveMatrix4x4(transform->Radians(45), 1.f*width/height, near, far);

    VertexData vertexData[3];
    shader->setModelMat(modelMat);
    shader->setViewMat(view);
    shader->setProjectionMat(projection);
    // 几何阶段
    for (size_t i = 0; i < model->size(); ++i) {
        for (size_t j = 0; j < 3; ++j) {
        // 初始化vertex shader数据
            model->getTriangleVertex(vertexData[j].vertex_pos, i, j);
            model->getTriangleUv(vertexData[j].uv, i, j);
            model->getTriangleNormal(vertexData[j].normal, i, j);
            vertexData[j].light_pos = lightPos;
            vertexData[j].view_pos = viewPos;
        }
        // 计算每个点的切线空间矩阵，应该放在顶点着色器中，传出切线空间的信息
        vec3 E1 = vertexData[2].vertex_pos - vertexData[0].vertex_pos;
        vec3 E2 = vertexData[1].vertex_pos - vertexData[0].vertex_pos;
        vec2 deltaUV1 = vertexData[2].uv - vertexData[0].uv;
        vec2 deltaUV2 = vertexData[1].uv - vertexData[0].uv;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        mat3 TBN[3];

        vec3 tangentTri = normalize(vec3(f * (deltaUV2.y * E1.x - deltaUV1.y * E2.x), f * (deltaUV2.y * E1.y - deltaUV1.y * E2.y), f * (deltaUV2.y * E1.z - deltaUV1.y * E2.z)));
//            Vector3f bitangent = Vector3f(-f * (deltaUV2.y() * E1.x() + deltaUV1.y() * E2.x()), -f * (deltaUV2.y() * E1.y() + deltaUV1.y() * E2.y()), -f * (deltaUV2.y() * E1.z() + deltaUV1.y() * E2.z())).normalized();
        // 每个点都要算切线空间

        for (int j = 0; j < 3; ++j) {
            vec3 tangent = modelMat * vec4(normalize(tangentTri - dot(tangentTri, vertexData[j].normal) * vertexData[j].normal),0.f);
            vec3 bitangent = modelMat * vec4(normalize(cross(tangent,vertexData[j].normal)), 0.f);
            vec3 normal = modelMat * vec4(vertexData[j].normal, 0.f);
            TBN[j] = mat3(tangent, bitangent, normal);
            vertexData[j].TBN = TBN[j];
            // 开始vertex shader
        }
        GPUStage(shader, vertexData, false);
    }
}

void RenderLoop::GPUStage(IShader *shader, VertexData* vertexData, bool shadow)
{
    FragmentData fragmentData[3];
    for (int i = 0; i < 3; ++i) {
        shader->vertexShader(vertexData[i], fragmentData[i]);
    }

    if (faceCulling(fragmentData[0].screen_pos, fragmentData[1].screen_pos, fragmentData[2].screen_pos)) return;
    if (shadow) {
        triangleByBcOrtho(fragmentData,shader);
    } else {
        triangleByBc(fragmentData,shader, shadow);
    }
}

// 顶点坐标, 法线, 纹理都会在这个阶段被插值
void RenderLoop::triangleByBc(FragmentData *fragmentData, IShader* shader, bool shadow)
{
    int minx = width;
    int miny = height;
    int maxx = 0;
    int maxy = 0;
    for (int i = 0; i < 3; ++i) {
        minx = std::min(minx, static_cast<int>(fragmentData[i].screen_pos.x));
        miny = std::min(miny, static_cast<int>(fragmentData[i].screen_pos.y));
        maxx = std::max(maxx, static_cast<int>(ceil(fragmentData[i].screen_pos.x)));
        maxy = std::max(maxy, static_cast<int>(ceil(fragmentData[i].screen_pos.y)));
//        std::cout << fragmentData[i].screen_pos[0] << " " << fragmentData[i].screen_pos[1] << " " << fragmentData[i].screen_pos[2] << std::endl;
    }
//    minx = std::max(minx, 0);miny = std::max(miny, 0);
//    maxx = std::min(maxx, width); maxx = std::min(maxy, height);
    vec2 p;
    vec3 p_barycentric;

    int cnt = 0;

//    std::cout << minx << " " << miny << " " << maxx << " " << maxy << std::endl;
    FragmentData pixelData;
    for (int x = minx; x <= maxx; x++) {
        for (int y = miny; y <= maxy; y++) {

            p[0] = x; p[1] = y;
            p_barycentric = barycentric(fragmentData, p);

            if (p_barycentric.x < 0 || p_barycentric.y < 0 || p_barycentric.z < 0) continue;
            cnt++;
            float w = 0;

            for (int i = 0; i < 3; ++i) {
                w += p_barycentric[i] * 1/fragmentData[i].screen_pos.w;
            }
            w = 1/w;

            float dep = (w-near)/(far-near);
//            std::cout << dep << " " << w << std::endl;
            if (setZBuffer(x, y, dep)) {
                // 通过深度测试
                vec3 coef(w/fragmentData[0].screen_pos.w * p_barycentric[0], w/fragmentData[1].screen_pos.w * p_barycentric[1], w/fragmentData[2].screen_pos.w * p_barycentric[2]);
                pixelData.uv[0] = pixelData.uv[1] = 0.0f;
                for (int k = 0; k < 2; ++k) {
                    for (int i = 0; i < 3; ++i) {
                        pixelData.uv[k] += (fragmentData[i].uv[k] * coef[i]);
                    }
                }

                for (int k = 0; k < 3; ++k) {
                    pixelData.lightDir[k] = 0;
                    pixelData.world_pos[k] = 0;
                    pixelData.viewDir[k] = 0;
                    pixelData.lightSpacePos[k] = 0;
                    for (int i = 0; i < 3; ++i) {
                        pixelData.world_pos[k] += (fragmentData[i].world_pos[k] * coef[i]);
                        pixelData.lightDir[k] += (fragmentData[i].lightDir[k] * coef[i]);
                        pixelData.viewDir[k] += (fragmentData[i].viewDir[k] * coef[i]);
                        pixelData.lightSpacePos[k] += (fragmentData[i].lightSpacePos[k] * coef[i]);
                    }
                }

                pixelData.lightColor = vec3(1.);

                vec4 color = shader->fragmentShader(pixelData);
                Util::printVec4(color);
                frameBuffer.WritePoint(x, y, Vec4c(static_cast<unsigned char>(color[0]*255), static_cast<unsigned char>(color[1]*255), static_cast<unsigned char>(color[2]*255), static_cast<unsigned char>(color[3]*255)));
            }
        }
    }
}

void RenderLoop::triangleByBcOrtho(FragmentData *fragmentData, IShader *shader)
{
    int minx = width;
    int miny = height;
    int maxx = 0;
    int maxy = 0;
    for (int i = 0; i < 3; ++i) {
        minx = std::min(minx, static_cast<int>(fragmentData[i].screen_pos.x));
        miny = std::min(miny, static_cast<int>(fragmentData[i].screen_pos.y));
        maxx = std::max(maxx, static_cast<int>(ceil(fragmentData[i].screen_pos.x)));
        maxy = std::max(maxy, static_cast<int>(ceil(fragmentData[i].screen_pos.y)));
//        std::cout << fragmentData[i].screen_pos[0] << " " << fragmentData[i].screen_pos[1] << " " << fragmentData[i].screen_pos[2] << std::endl;
    }
//    minx = std::max(minx, 0);miny = std::max(miny, 0);
//    maxx = std::min(maxx, width); maxx = std::min(maxy, height);
    vec2 p;
    vec3 p_barycentric;
//    std::cout << minx << " " << miny << " " << maxx << " " << maxy << std::endl;

    for (int x = minx; x <= maxx; x++) {
        for (int y = miny; y <= maxy; y++) {
            FragmentData pixelData;
            p[0] = x; p[1] = y;
            p_barycentric = barycentric(fragmentData, p);

            if (p_barycentric.x < 0 || p_barycentric.y < 0 || p_barycentric.z < 0) continue;
            float z = 0;
            for (int i = 0; i < 3; ++i) {
                z += p_barycentric[i] * fragmentData[i].screen_pos.z;
            }

            if (setZBuffer(x, y, z)) {
                // 通过深度测试
                pixelData.screen_pos[2] = z;
                vec4 color = shader->fragmentShader(pixelData);
                std::cout << x + y * width << " " << color[0] << " " << width << std::endl;
                shadowMap[size_t(x + y * width)] = color[0];
//                frameBuffer.WritePoint(x, y, Vec4c(static_cast<unsigned char>(color[0]*255), static_cast<unsigned char>(color[1]*255), static_cast<unsigned char>(color[2]*255), static_cast<unsigned char>(color[3]*255)));
            }
        }
    }
}

void RenderLoop::ClearZbuffer()
{
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            zbuffer[i + j * width] = 1;
        }
    }
}

bool RenderLoop::setZBuffer(int x, int y, float z)
{
    if (abs(x + y* width) >= width*height) return false;
    if (x < 0 || x >= width || y < 0 || y >= height) return false;
//    std::cout << zbuffer[x+y*width] <<" " << z << std::endl;
    if (zbuffer[x+y*width] > z) {
        zbuffer[x+y*width] = z;
        return true;
    }
    return false;
}

vec3 RenderLoop::barycentric(FragmentData *fragmentData, vec2& p)
{

    vec3 u = cross(vec3(fragmentData[2].screen_pos.x-fragmentData[0].screen_pos.x, fragmentData[1].screen_pos.x-fragmentData[0].screen_pos.x, fragmentData[0].screen_pos.x-p.x),(
        vec3(fragmentData[2].screen_pos.y-fragmentData[0].screen_pos.y, fragmentData[1].screen_pos.y-fragmentData[0].screen_pos.y, fragmentData[0].screen_pos.y-p.y)));
    if (std::abs(u.z) < 1)
        return vec3(-1, 1, 1);
    return vec3(1-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}



void RenderLoop::keyPressEvent(float speed, char type)
{
//    Vector3f dir;
//    switch (type) {
//    case 'W':
//        dir = camera->GetLookDirection();
//        break;
//    case 'S':
//        dir = -camera->GetLookDirection();
//        break;
//    case 'A':
//        dir = -camera->GetLookDirection().cross(camera->GetUpDirection()).normalized();
//        break;
//    case 'D':
//        dir = camera->GetLookDirection().cross(camera->GetUpDirection()).normalized();
//        break;
//    }
//    camera->SetPosition(speed * dir + camera->GetPosition());
}

void RenderLoop::mouseMoveEvent(float pitch, float yaw)
{

}

bool RenderLoop::faceCulling(const vec3 &v1, const vec3 &v2, const vec3 &v3)
{
    vec3 tmp1 = v2-v1;
    vec3 tmp2 = v3-v2;
    vec3 norm = cross(tmp1, tmp2);
    vec3 view(0, 0, -1);
    return dot(view, norm) > 0;
}


void RenderLoop::stopRender()
{
    stop = true;
}


