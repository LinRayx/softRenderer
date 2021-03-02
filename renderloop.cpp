#include "renderloop.h"
#include "renderCore/texture.h"
RenderLoop::RenderLoop(int _width, int _height, QObject *parent) : QObject(parent), width(_width), height(_height)
{
    stop = false;
    camera = new Camera;
    transform = new MatrixTransform;
}

RenderLoop::~RenderLoop()
{

    if(model) delete model;

    if(camera) delete camera;

    if(transform) delete transform;

}

void RenderLoop::loop()
{

    frameBuffer.Resize(width, height);
    TGAImage diffuseImg;
    std::string diffuseImgPath = APP_PATH + "image" + OS_FILE_INTERVEL + "african_head_diffuse.tga";
//    Texture text;
//    text.loadTexture(diffuseImgPath.c_str());
//    return;
    if (diffuseImg.read_tga_file(diffuseImgPath.c_str()) == false) return;
    diffuseImg.flip_vertically();
    TGAImage image(width, height, TGAImage::RGBA);
    std::string objPath = APP_PATH + "obj" + OS_FILE_INTERVEL + "african_head.obj";
    model = new Model(objPath.c_str());
    float *zBuffer = new float[width*height];

    Vec3f light_dir(0,0,-1);

    camera->SetUpDirection(Vector3f(0, 1, 0));
    camera->SetPosition(Vector3f(0, 0, 5));
    camera->SetLookDirection(Vector3f(0, 0, 0)-camera->GetPosition());
    int fps = 0;

    while(!stop) {
        start = clock();
        for (int i = 0; i < width; ++i) {
            for (int j = 0; j <height; ++j) {
                zBuffer[i+j*width] = std::numeric_limits<float>::min();
            }
        }
        frameBuffer.ClearColorBuffer(Vec4c(0, 0, 0, 0));
//        qDebug() << start;
        MatrixX4f rotate = transform->GetRotationMatrix4x4(Vector3f(0, 30.0f, 0));
        MatrixX4f view = transform->GetViewMatrix4x4(camera);
        MatrixX4f projection = transform->GetPersepectiveMatrix4x4((float)45/180*EIGEN_PI, (float)width/height, 0.1f, 100.0f);
        std::vector<int> face, tex;
        auto it1 = model->faces_.begin();
        auto it2 = model->faceTexs_.begin();
        int cnt = 0;
        for (int i = 0; i < model->nfaces(); ++i) {
            face = *(it1+i);
            tex = *(it2+i);
            Vector4f test_world_coords[3];
            Vector3f screen_coords[3];
            Vec3f world_coords[3];
            Vector2f uvs[3];
            Vec3f triangleColor[3];
            Vector3f perCoords[3];
            for (int j = 0; j <3; ++j) {
                world_coords[j] = model->vert(face[j]);

                test_world_coords[j] = Vector4f(world_coords[j].x, world_coords[j].y, world_coords[j].z, 1.0);
                Vector4f sc = projection * view * rotate * test_world_coords[j];
                screen_coords[j] = Vector3f(sc.x()/sc.w(), sc.y()/sc.w(), sc.z()/sc.w());
                screen_coords[j] = Vector3f((screen_coords[j].x()+1.)*width/2., (screen_coords[j].y()+1.)*height/2., screen_coords[j].z());
                Vec2f uv = model->tex(tex[j]);
                uvs[j] = Vector2f(uv.u * diffuseImg.get_width(), uv.v * diffuseImg.get_height());
            }
//            auto t = clock();
            if (faceCulling(perCoords[0], perCoords[1], perCoords[2])) {
                continue;
            }
            triangleByBc(screen_coords, uvs, zBuffer, diffuseImg, 0.1f, 100.0f);
//            auto s = clock();
//            std::cout << (double)(s-t)/CLOCKS_PER_SEC * model->nfaces() << std::endl;
        }

        finish = clock();
        deltaFrameTime = (double)(finish-start)/CLOCKS_PER_SEC;
//        std::cout << cnt << std::endl;
        emit frameOut(frameBuffer.data(), deltaFrameTime, fps);
    }
    delete[] zBuffer;
    qDebug() << "render quit";
}

void RenderLoop::triangleByBc(Vector3f *screen_point, Vector2f* uvs, float *zBuffer, TGAImage &image, float near, float far)
{
    int minx = std::min(screen_point[2].x(), std::min(screen_point[0].x(), screen_point[1].x()));
    int miny = std::min(screen_point[2].y(), std::min(screen_point[0].y(), screen_point[1].y()));
    int maxx = std::max(screen_point[2].x(), std::max(screen_point[0].x(), screen_point[1].x()));
    int maxy = std::max(screen_point[2].y(), std::max(screen_point[0].y(), screen_point[1].y()));
    Vector3i triangleColor[3];
    for (int i = 0; i < 3; ++i) {
        triangleColor[i] = texture(uvs[i], image);
    }
    for (int x = minx; x <= maxx; x++) {
        for (int y = miny; y <= maxy; y++) {
            Vector2i p(x, y);
            Vector3f p_barycentric = barycentric(screen_point, p);
            if (p_barycentric.x() < 0 || p_barycentric.y() < 0 || p_barycentric.z() < 0) continue;
            float z = 0;
            Vec4c color2(0, 0, 0, 255);

            for (int i = 0; i < 3; ++i) {

                z += p_barycentric[i] * 1/screen_point[i].z();
            }

            z = 1/z;

            for (int k = 0; k < 3; ++k) {
                for (int i = 0; i < 3; ++i) {
                    color2.raw[k] += int(triangleColor[i][k] * z/screen_point[i].z() * p_barycentric[i]);
                }
                color2.raw[k] *= z;
            }
            float zb = 0;

            for (int i = 0; i < 3; ++i) {

                zb += p_barycentric[i] * 1/screen_point[i].z();
            }
            zb = 1/zb;
            if (abs(x + y* width) >= width*height) continue;
            if (zBuffer[x+y*width] < zb) {
                zBuffer[x+y*width] = zb;
                frameBuffer.WritePoint(x, y, color2);
            }
        }
    }
}

Vector3i RenderLoop::texture(Vector2f& uv, TGAImage &image)
{
    Vector2i leftCorner = Vector2i((int)uv.x(), (int)uv.y());
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


