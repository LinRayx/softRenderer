#include "renderloop.h"
RenderLoop::RenderLoop(int _width, int _height, QObject *parent) : QObject(parent), width(_width), height(_height)
{
    stop = false;
    camera = new Camera;
    transform = new MatrixTransform;
    shader = new phoneShader;
}

RenderLoop::~RenderLoop()
{

    if(model) delete model;

    if(camera) delete camera;

    if(transform) delete transform;

    if(shader) delete shader;

}

void RenderLoop::loop()
{
    frameBuffer.Resize(width, height);
    TGAImage diffuseImg, normalImg;
    std::string diffuseImgPath = APP_PATH + "image" + OS_FILE_INTERVEL + "african_head_diffuse.tga";
    std::string normalImgPath = APP_PATH + "image" + OS_FILE_INTERVEL + "african_head_nm_tangent.tga";
    if (diffuseImg.read_tga_file(diffuseImgPath.c_str()) == false) return;
    diffuseImg.flip_vertically();
    if (normalImg.read_tga_file(normalImgPath.c_str()) == false) return;
    normalImg.flip_vertically();

    std::string objPath = APP_PATH + "obj" + OS_FILE_INTERVEL + "african_head.obj";
    model = new Model(objPath.c_str());
    float *zBuffer = new float[width*height];
    int fps = 0;
    camera->SetUpDirection(Vector3f(0, 1, 0));
    camera->SetPosition(Vector3f(0, 0, 3));
    camera->SetLookDirection(Vector3f(0, 0, 0)-camera->GetPosition());
    lightPos = Vector3f(5, 0, 5);
    shader->setDiffuseImage(diffuseImg);
    shader->setNormalImage(normalImg);
    shader->setScreenSize(width, height);
    float speed = 0.01f, rot = 0.0f;

    while(!stop) {
        start = clock();
        for (int i = 0; i < width; ++i) {
            for (int j = 0; j <height; ++j) {
                zBuffer[i+j*width] = std::numeric_limits<float>::min();
            }
        }
        frameBuffer.ClearColorBuffer(Vec4c(0, 0, 0, 0));
        if (rot > 360) rot = 0;

//        rot = rot + 300 * speed;
        rot = 0;
        Matrix4f rotate = transform->GetRotationMatrix4x4(Vector3f(0, rot, 0));
        Matrix4f view = transform->GetViewMatrix4x4(camera);
        Matrix4f projection = transform->GetPersepectiveMatrix4x4(45.f/180.f*PI, 1.f*width/height, 0.1f, 100.0f);
        Matrix3f model3x3 = transform->GetRotationMatrix3x3(Vector3f(0, rot, 0));
        VertexData vertexData[3];
        FragmentData fragmentData[3];

        shader->setModelMat(rotate);
        shader->setViewMat(view);
        shader->setProjectionMat(projection);
        int cnt = 0;
        // 几何阶段
        for (size_t i = 0; i < model->size(); ++i) {
            for (size_t j = 0; j < 3; ++j) {
            // 初始化vertex shader数据
                model->getTriangleVertex(vertexData[j].vertex_pos, i, j);
                model->getTriangleUv(vertexData[j].uv, i, j);
                model->getTriangleNormal(vertexData[j].normal, i, j);
                vertexData[j].light_pos = lightPos;
                vertexData[j].view_pos = camera->GetPosition();
            }
            // 计算每个点的切线空间矩阵，应该放在顶点着色器中，传出切线空间的信息
            Vector3f E1 = vertexData[2].vertex_pos - vertexData[0].vertex_pos;
            Vector3f E2 = vertexData[1].vertex_pos - vertexData[0].vertex_pos;
            Vector2f deltaUV1 = vertexData[2].uv - vertexData[0].uv;
            Vector2f deltaUV2 = vertexData[1].uv - vertexData[0].uv;

            float f = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y());
            Matrix3f TBN[3];
            Vector3f tangentTri = Vector3f(f * (deltaUV2.y() * E1.x() - deltaUV1.y() * E2.x()), f * (deltaUV2.y() * E1.y() - deltaUV1.y() * E2.y()), f * (deltaUV2.y() * E1.z() - deltaUV1.y() * E2.z())).normalized();
//            Vector3f bitangent = Vector3f(-f * (deltaUV2.y() * E1.x() + deltaUV1.y() * E2.x()), -f * (deltaUV2.y() * E1.y() + deltaUV1.y() * E2.y()), -f * (deltaUV2.y() * E1.z() + deltaUV1.y() * E2.z())).normalized();
            // 每个点都要算切线空间

            for (int j = 0; j < 3; ++j) {
                Vector3f tangent = model3x3 * (tangentTri - tangentTri.dot(vertexData[j].normal) * vertexData[j].normal).normalized();
                Vector3f bitangent = model3x3 * tangent.cross(vertexData[j].normal).normalized();
                Vector3f normal = model3x3 * vertexData[j].normal;
                TBN[j] << tangent.x(), tangent.y(), tangent.z(),
                        bitangent.x(), bitangent.y(), bitangent.z(),
                        normal.x(), normal.y(), normal.z();
//                TBN[i] = TBN[j].transpose();//转置 == 逆
                vertexData[j].TBN = TBN[j];
                // 开始vertex shader
                shader->vertexShader(vertexData[j], fragmentData[j]);
            }
            if (faceCulling(fragmentData[0].screen_pos, fragmentData[1].screen_pos, fragmentData[2].screen_pos)) continue;
            cnt++;
            triangleByBc(fragmentData, zBuffer, cnt);
        }
//        std::cout << cnt << std::endl;
        finish = clock();
        deltaFrameTime = static_cast<double>(finish-start)/CLOCKS_PER_SEC;
        emit frameOut(frameBuffer.data(), deltaFrameTime, fps);
    }
    delete[] zBuffer;
    qDebug() << "render quit";
}
// 顶点坐标, 法线, 纹理都会在这个阶段被插值
void RenderLoop::triangleByBc(FragmentData *fragmentData, float *zBuffer, int& tm)
{
    int minx = width+100;
    int miny = height+100;
    int maxx = 0;
    int maxy = 0;
    for (int i = 0; i < 3; ++i) {
        minx = std::min(minx, static_cast<int>(fragmentData[i].screen_pos.x()));
        miny = std::min(miny, static_cast<int>(fragmentData[i].screen_pos.y()));
        maxx = std::max(maxx, static_cast<int>(ceil(fragmentData[i].screen_pos.x())));
        maxy = std::max(maxy, static_cast<int>(ceil(fragmentData[i].screen_pos.y())));
    }
    Vector2i p;
    Vector3f p_barycentric;
    Vec4c color2;
    int cnt = 0;
    FragmentData pixelData;
    for (int x = minx; x <= maxx; x++) {
        for (int y = miny; y <= maxy; y++) {
            p[0] = x; p[1] = y;
            p_barycentric = barycentric(fragmentData, p);

            if (p_barycentric.x() < 0 || p_barycentric.y() < 0 || p_barycentric.z() < 0) continue;
            cnt++;
            float z = 0;
            for (int i = 0; i < 3; ++i) {
                z += p_barycentric[i] * 1/fragmentData[i].screen_pos.z();
            }
            z = 1/z;
            if (abs(x + y* width) >= width*height) continue;
            if (zBuffer[x+y*width] < z) {
                // 通过深度测试
                zBuffer[x+y*width] = z;
                Vector3f coef(z/fragmentData[0].screen_pos.z() * p_barycentric[0], z/fragmentData[1].screen_pos.z() * p_barycentric[1], z/fragmentData[2].screen_pos.z() * p_barycentric[2]);
                pixelData.uv[0] = pixelData.uv[1] = 0.0f;
                for (int k = 0; k < 2; ++k) {
                    for (int i = 0; i < 3; ++i) {
                        pixelData.uv[k] += (fragmentData[i].uv[k] * coef[i]);
                    }
                }

                for (int k = 0; k < 3; ++k) {
                    for (int i = 0; i < 3; ++i) {
                        pixelData.world_pos[k] += (fragmentData[i].world_pos[k] * coef[i]);
                        pixelData.lightDir[k] += (fragmentData[i].lightDir[k] * coef[i]);
                        pixelData.viewDir[k] += (fragmentData[i].viewDir[k] * coef[i]);
                    }
                }

                pixelData.lightColor = Vector3f(1., 1., 1.);
                Vector4f color = shader->fragmentShader(pixelData);

                frameBuffer.WritePoint(x, y, Vec4c(static_cast<unsigned char>(color[0]*255), static_cast<unsigned char>(color[1]*255), static_cast<unsigned char>(color[2]*255), static_cast<unsigned char>(color[3]*255)));
            }
        }
    }
//    tm += cnt;
//    std::cout << cnt << std::endl;
}


Vector3f RenderLoop::barycentric(FragmentData *fragmentData, Vector2i& p)
{

    Vector3f u = Vector3f(fragmentData[2].screen_pos.x()-fragmentData[0].screen_pos.x(), fragmentData[1].screen_pos.x()-fragmentData[0].screen_pos.x(), fragmentData[0].screen_pos.x()-p.x()).cross(
        Vector3f(fragmentData[2].screen_pos.y()-fragmentData[0].screen_pos.y(), fragmentData[1].screen_pos.y()-fragmentData[0].screen_pos.y(), fragmentData[0].screen_pos.y()-p.y()));
    if (std::abs(u.z()) < 1)
        return Vector3f(-1, 1, 1);
    return Vector3f(1-(u.x()+u.y())/u.z(), u.y()/u.z(), u.x()/u.z());
}



void RenderLoop::keyPressEvent(float speed, char type)
{
    Vector3f dir;
    switch (type) {
    case 'W':
        dir = camera->GetLookDirection();
        break;
    case 'S':
        dir = -camera->GetLookDirection();
        break;
    case 'A':
        dir = -camera->GetLookDirection().cross(camera->GetUpDirection()).normalized();
        break;
    case 'D':
        dir = camera->GetLookDirection().cross(camera->GetUpDirection()).normalized();
        break;
    }
    camera->SetPosition(speed * dir + camera->GetPosition());
}

void RenderLoop::mouseMoveEvent(float pitch, float yaw)
{
    Matrix3f m = transform->GetEularMatrix3x3(transform->Radians(pitch), transform->Radians(yaw), 0);
    camera->Rotate(m);
}

bool RenderLoop::faceCulling(const Vector3f &v1, const Vector3f &v2, const Vector3f &v3)
{
    Vector3f tmp1 = v2-v1;
    Vector3f tmp2 = v3-v2;
    Vector3f norm = tmp1.cross(tmp2);
    Vector3f view(0, 0, 1);
    return view.dot(norm) > 0;
}


void RenderLoop::stopRender()
{
    stop = true;
}


