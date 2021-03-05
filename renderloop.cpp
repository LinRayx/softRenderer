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
    std::string normalImgPath = APP_PATH + "image" + OS_FILE_INTERVEL + "african_head_diffuse.tga";
//    Texture text;
//    text.loadTexture(diffuseImgPath.c_str());
//    return;
    if (diffuseImg.read_tga_file(diffuseImgPath.c_str()) == false) return;
    diffuseImg.flip_vertically();
    if (normalImg.read_tga_file(normalImgPath.c_str()) == false) return;
    normalImg.flip_vertically();
    TGAImage image(width, height, TGAImage::RGBA);
    std::string objPath = APP_PATH + "obj" + OS_FILE_INTERVEL + "african_head.obj";
    model = new Model(objPath.c_str());
    float *zBuffer = new float[width*height];

    Vec3f light_dir(0,0,-1);
    int fps = 0;
    camera->SetUpDirection(Vector3f(0, 1, 0));
    camera->SetPosition(Vector3f(0, 0, 5));
    camera->SetLookDirection(Vector3f(0, 0, 0)-camera->GetPosition());
    // 加载模型
    std::vector<std::vector<Vector3f>> faces;
    std::vector<std::vector<Vector2f>> texs;
    std::vector<Vector3f> v1;
    std::vector<Vector2f> v2;
    for (int i = 0; i < model->nfaces(); ++i) {
        v1.clear();
        v2.clear();
        auto tri = model->face(i);
        auto texId = model->faceTex(i);
        for (int j = 0; j < 3; ++j) {
            Vec3f pos = model->vert(tri[j]);
            Vec2f uv = model->tex(texId[j]);
            v1.push_back(Vector3f(pos.x, pos.y, pos.z));
            v2.push_back(Vector2f(uv.x, uv.y));
        }
        faces.push_back(v1);
        texs.push_back(v2);
    }
    // 加载模型End
    shader->setDiffuseImage(diffuseImg);
    shader->setNormalImage(normalImg);
    shader->setScreenSize(width, height);
    std::cout << faces.size() << std::endl;
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

        rot = rot + 300 * speed;
        Matrix4f rotate = transform->GetRotationMatrix4x4(Vector3f(0, rot, 0));
        Matrix4f view = transform->GetViewMatrix4x4(camera);
        Matrix4f projection = transform->GetPersepectiveMatrix4x4((float)45/180*EIGEN_PI, (float)width/height, 0.1f, 100.0f);
        Vector3f screen_coords[3];
        Vector3f world_coords[3];
        Vector2f uvs[3];
//        MatrixX4f MVP = projection * view * rotate;
        shader->setModelMat(rotate);
        shader->setViewMat(view);
        shader->setProjectionMat(projection);
//        double tot = 0;
        int cnt = 0;
        // 几何阶段
        for (size_t i = 0; i < faces.size(); ++i) {
            for (size_t j = 0; j < faces[i].size(); ++j) {
            // 坐标变换 vertexShader
                screen_coords[j] = shader->vertexShader(faces[i][j]);
                world_coords[j] = rotate * faces[i][j];
                uvs[j] = texs[i][j];
            }
            if (faceCulling(screen_coords[0], screen_coords[1], screen_coords[2])) continue;
            cnt++;
//            auto s = clock();
            triangleByBc(screen_coords, uvs, world_coords, normalImg, zBuffer, diffuseImg, 0.1f, 100.0f, cnt);
//            auto e = clock();
//            std::cout << (double)(e-s)/CLOCKS_PER_SEC << std::endl;
//            tot += (double)(e-s)/CLOCKS_PER_SEC;
        }
//        std::cout << cnt << std::endl;
        finish = clock();
        deltaFrameTime = (double)(finish-start)/CLOCKS_PER_SEC;
//        std::cout << (double)(e-s)/CLOCKS_PER_SEC << " " << (double)(finish-start)/CLOCKS_PER_SEC <<  std::endl;
        emit frameOut(frameBuffer.data(), deltaFrameTime, fps);
    }
    delete[] zBuffer;
    qDebug() << "render quit";
}
// 顶点坐标, 法线, 纹理都会在这个阶段被插值
void RenderLoop::triangleByBc(Vector3f *screen_point, Vector2f* uvs, Vector3f* world_pos, Vector3f& normal, TGAImage& normalImg, float *zBuffer, TGAImage &image, float near, float far, int& tm)
{

    int minx = std::min(screen_point[2][0], std::min(screen_point[0][0], screen_point[1][0]));
    int miny = std::min(screen_point[2][1], std::min(screen_point[0][1], screen_point[1][1]));
    int maxx = std::max(screen_point[2][0], std::max(screen_point[0][0], screen_point[1][0]));
    int maxy = std::max(screen_point[2][1], std::max(screen_point[0][1], screen_point[1][1]));

    Vector2i p;
    Vector3f p_barycentric;
    Vec4c color2;
    int cnt = 0;

    FragmentData fragmentData;

    // 计算每个点的切线空间矩阵，应该放在顶点着色器中，传出切线空间的信息
    Vector3f E1 = world_pos[2] - world_pos[0];
    Vector3f E2 = world_pos[1] - world_pos[0];
    Vector2f deltaUV1 = uvs[2] - uvs[0];
    Vector2f deltaUV2 = uvs[1] - uvs[0];

    float f = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y());
    Matrix3f TBN;
    Vector3f tangent = Vector3f(f * (deltaUV2.y() * E1.x() - deltaUV1.y() * E2.x()), f * (deltaUV2.y() * E1.y() - deltaUV1.y() * E2.y()), f * (deltaUV2.y() * E1.z() - deltaUV1.y() * E2.z())).normalized();
//    Vector3f bitangent = Vector3f(-f * (deltaUV2.y() * E1.x() + deltaUV1.y() * E2.x()), -f * (deltaUV2.y() * E1.y() + deltaUV1.y() * E2.y()), -f * (deltaUV2.y() * E1.z() + deltaUV1.y() * E2.z())).normalized();

    tangent = (tangent - tangent.dot(normal) * normal).normalized();
    Vector3f bitangent = tangent.cross(normal).normalized();
    TBN << tangent.x(), tangent.y(), tangent.z(),
            bitangent.x(), bitangent.y(), bitangent.z(),
            normal.x(), normal.y(), normal.z();

    fragmentData.tangentLightPos = TBN * lightPos;
    fragmentData.tangentViewPos = TBN * camera->GetPosition();

    for (int x = minx; x <= maxx; x++) {
        for (int y = miny; y <= maxy; y++) {
            p[0] = x; p[1] = y;
            p_barycentric = barycentric(screen_point, p);

            if (p_barycentric.x() < 0 || p_barycentric.y() < 0 || p_barycentric.z() < 0) continue;
            cnt++;
            float z = 0;
            for (int i = 0; i < 3; ++i) {
                z += p_barycentric[i] * 1/screen_point[i].z();
            }
            z = 1/z;
            fragmentData.uv[0] = fragmentData.uv[1] = 0.0f;
            for (int k = 0; k < 2; ++k) {
                for (int i = 0; i < 3; ++i) {
                    fragmentData.uv[k] += (uvs[i][k] * z/screen_point[i].z() * p_barycentric[i]);
                }
            }

            for (int k = 0; k < 3; ++k) {
                for (int i = 0; i < 3; ++i) {
                    fragmentData.world_pos[k] += (world_pos[i][k] * p_barycentric[i]);
                }
            }
            fragmentData.tangentWorldPos = TBN * fragmentData.world_pos;

            if (abs(x + y* width) >= width*height) continue;
            if (zBuffer[x+y*width] < z) {
                zBuffer[x+y*width] = z;
                // 通过深度测试
//                shader->setUV(uv);
                Vector4f color = shader->fragmentShader(fragmentData);

                frameBuffer.WritePoint(x, y, Vec4c(static_cast<unsigned char>(color[0]*255), static_cast<unsigned char>(color[1]*255), static_cast<unsigned char>(color[2]*255), static_cast<unsigned char>(color[3]*255)));
            }
        }
    }
    tm += cnt;
//    std::cout << cnt << std::endl;
}

Vector3i RenderLoop::texture(Vector2f& uv, TGAImage &image)
{
    Vector2i leftCorner = Vector2i(static_cast<int>(uv.x()), static_cast<int>(uv.y()));
    Vector2i rightCorner = Vector2i(int(uv.x())+1, (int)uv.y());
    Vector2i leftUp = Vector2i((int)uv.x(), int(uv.y())+1);
    Vector2i rightUp = Vector2i(int(uv.x())+1, int(uv.y())+1);

    TGAColor leftCornerColor = image.get(leftCorner.x(), leftCorner.y());

    TGAColor rightCornerColor = image.get(rightCorner.x(), rightCorner.y());
    TGAColor leftUpColor = image.get(leftUp.x(), leftUp.y());
    TGAColor rightUpColor = image.get(rightUp.x(), rightUp.y());

    float t1 = (uv.x() - leftCorner.x() );
    float t2 = (uv.y() - leftCorner.y());
    // std::cout << t1 <<" " << t2 << std::endl;
    TGAColor down = TGAColor(0, 0, 0, 255);
    TGAColor up = TGAColor(0, 0, 0, 255);
    for (int i = 0; i < 3; ++i) {
        down.raw[i] = leftCornerColor.raw[i] * t1 + rightCornerColor.raw[i] * (1-t1);
        up.raw[i] = leftUpColor.raw[i] * t1 + rightUpColor.raw[i] * (1-t1);
    }

    Vector3i res;
    for (int i = 0; i < 3; ++i) {
        res[i] = (int)(down.raw[i] * t2 + up.raw[i] * (1-t2));
    }
    return res;
}

Vector3f RenderLoop::barycentric(Vector3f *pts, Vector2i& p)
{
    Vector3f u = Vector3f(pts[2].x()-pts[0].x(), pts[1].x()-pts[0].x(), pts[0].x()-p.x()).cross(
        Vector3f(pts[2].y()-pts[0].y(), pts[1].y()-pts[0].y(), pts[0].y()-p.y()));
    if (std::abs(u.z()) < 1)
        return Vector3f(-1, 1, 1);
    return Vector3f(1-(u.x()+u.y())/u.z(), u.y()/u.z(), u.x()/u.z());
}



void RenderLoop::triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    const TGAColor white = TGAColor(255, 255, 255, 255);
    const TGAColor red   = TGAColor(255, 0,   0,   255);
    const TGAColor green   = TGAColor(0, 128,   0,   255);
    std::vector<Vec2i> points;
    points.push_back(t0);
    points.push_back(t1);
    points.push_back(t2);
    std::sort(points.begin(), points.end());
    // for (int i = 0; i < 3; ++i) {
    //     // std::cout << points[i] << std::endl;
    //     if (i == 2) line(points[i].x, points[i].y, points[(i+1)%3].x, points[(i+1)%3].y, image, red);
    //     else line(points[i].x, points[i].y, points[(i+1)%3].x, points[(i+1)%3].y, image, color);
    // }

    int total_height = points[0].y - points[2].y;


    for (int y = points[2].y; y <= points[0].y; y++) {
        bool up_part = y > points[1].y;
        int segment_height = points[1].y - points[2].y;
        int up_segment_height = points[0].y - points[1].y;
        float alpha = (y - points[2].y) / (float) total_height;
        float beta = up_part ? (y - points[1].y) / (float) up_segment_height : (y - points[2].y) / (float) segment_height;

        Vec2i A = points[2] + (points[0] - points[2]) * alpha;
        Vec2i B = up_part ? points[1] + (points[0] - points[1]) * beta : points[2] + (points[1] - points[2]) * beta;
        image.set(A.x, A.y, red);
        image.set(B.x, B.y, green);
        if (A.x > B.x) std::swap(A, B);
        for (int x = A.x; x <= B.x; x++) {
            image.set(x, y, white);
        }
    }
}

void RenderLoop::line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x=x0; x<=x1; x++) {
        float t = (x-x0)/(float)(x1-x0);
        int y = y0*(1.-t) + y1*t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
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


