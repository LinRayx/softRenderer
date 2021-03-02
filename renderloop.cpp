#include "renderloop.h"
#include <QDebug>
#include <QDir>
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
    if (diffuseImg.read_tga_file(diffuseImgPath.c_str()) == false) return;
    diffuseImg.flip_vertically();
    TGAImage image(width, height, TGAImage::RGBA);
    std::string objPath = APP_PATH + "obj" + OS_FILE_INTERVEL + "african_head.obj";
    model = new Model(objPath.c_str());
    float *zBuffer = new float[width*height];

    Vec3f light_dir(0,0,-1);

    camera->SetUpDirection(Vector3f(0, 1, 0));
    camera->SetPosition(Vector3f(-5, 0, 5));
    camera->SetLookDirection(Vector3f(0, 0, 0)-camera->GetPosition());

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

        for (int i = 0; i < model->nfaces(); ++i) {
            std::vector<int> face = model->face(i);
            std::vector<int> tex = model->faceTex(i);

            Vector4f test_world_coords[3];
            Vec3f screen_coords[3];
            Vec3f world_coords[3];
            Vec2f uv[3];
            Vec3f triangleColor[3];
//            Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
//            n.normalize();
            // float intensity = n * light_dir;

            for (int j = 0; j <3; ++j) {
                world_coords[j] = model->vert(face[j]);

                test_world_coords[j] = Vector4f(world_coords[j].x, world_coords[j].y, world_coords[j].z, 1.0);
                Vector4f sc = projection * view * rotate * test_world_coords[j];

//                qDebug() << sc.w();
                screen_coords[j] = Vec3f(sc.x()/sc.w(), sc.y()/sc.w(), sc.z()/sc.w());
//                screen_coords[j].z = 2 *0.1 * 100 / ((100-0.1)*sc.w()) + (100.1/99.9);
                screen_coords[j] = Vec3f((screen_coords[j].x+1.)*width/2., (screen_coords[j].y+1.)*height/2., screen_coords[j].z);

                uv[j] = model->tex(tex[j]);
                uv[j] = Vec2f(uv[j].u * diffuseImg.get_width(), uv[j].v * diffuseImg.get_height());
                triangleColor[j] = texture(uv[j], diffuseImg);
            }

            // triangleByBcFg(screen_coords[0], screen_coords[1], screen_coords[2], zBuffer, image, diffuseImg, uv[0], uv[1], uv[2]);
            triangleByBc(screen_coords, uv, zBuffer, image, triangleColor, 0.1f, 100.0f);
        }
        finish = clock();
        deltaFrameTime = (double)(finish-start)/CLOCKS_PER_SEC;
        emit frameOut(frameBuffer.data(), deltaFrameTime);
    }
    delete zBuffer;
    qDebug() << "render quit";
//        image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
//        image.write_tga_file("output.tga");
}

void RenderLoop::triangleByBc(Vec3f *screen_point, Vec2f* uv, float *zBuffer, TGAImage &image, Vec3f *triangleColor, float near, float far)
{
    int minx = std::min(screen_point[2].x, std::min(screen_point[0].x, screen_point[1].x));
    int miny = std::min(screen_point[2].y, std::min(screen_point[0].y, screen_point[1].y));
    int maxx = std::max(screen_point[2].x, std::max(screen_point[0].x, screen_point[1].x));
    int maxy = std::max(screen_point[2].y, std::max(screen_point[0].y, screen_point[1].y));

    for (int x = minx; x <= maxx; x++) {
        for (int y = miny; y <= maxy; y++) {
            Vec2i p(x, y);
            Vec3f p_barycentric = barycentric(screen_point, p);
            if (p_barycentric.x < 0 || p_barycentric.y < 0 || p_barycentric.z < 0) continue;
            float z = 0;
            TGAColor color1(0, 0, 0, 255);
            Vec4c color2(0, 0, 0, 255);

            for (int i = 0; i < 3; ++i) {

                z += p_barycentric.raw[i] * 1/screen_point[i].z;
            }

            z = 1/z;

            for (int k = 0; k < 3; ++k) {
                for (int i = 0; i < 3; ++i) {
                    color2.raw[k] += int(triangleColor[i].raw[k] * z/screen_point[i].z * p_barycentric.raw[i]);
                }
                color2.raw[k] *= z;
            }
            float zb = 0;

            for (int i = 0; i < 3; ++i) {

                zb += p_barycentric.raw[i] * 1/screen_point[i].z;
            }
            zb = 1/zb;
//            qDebug() << zb << x  << y << p_barycentric.x << p_barycentric.y << p_barycentric.z;
            if (abs(x + y* width) >= width*height) continue;
//            qDebug() << zb << x  << y; //竟然爆内存
            if (zBuffer[x+y*width] < zb) {
                zBuffer[x+y*width] = zb;
                image.set(x, y, color1);
                frameBuffer.WritePoint(x, y, color2);
            }
        }
    }
}

Vec3f RenderLoop::texture(Vec2f uv, TGAImage &image)
{
    Vec2i leftCorner = Vec2i((int)uv.u, (int)uv.v);
    Vec2i rightCorner = Vec2i(int(uv.u)+1, (int)uv.v);
    Vec2i leftUp = Vec2i((int)uv.u, int(uv.v)+1);
    Vec2i rightUp = Vec2i(int(uv.u)+1, int(uv.v)+1);


    TGAColor leftCornerColor = image.get(leftCorner.u, leftCorner.v);

    TGAColor rightCornerColor = image.get(rightCorner.u, rightCorner.v);
    TGAColor leftUpColor = image.get(leftUp.u, leftUp.v);
    TGAColor rightUpColor = image.get(rightUp.u, rightUp.v);

    float t1 = (uv.u - leftCorner.u );
    float t2 = (uv.v - leftCorner.v);
    // std::cout << t1 <<" " << t2 << std::endl;
    TGAColor down = TGAColor(0, 0, 0, 255);
    TGAColor up = TGAColor(0, 0, 0, 255);
    for (int i = 0; i < 3; ++i) {
        down.raw[i] = leftCornerColor.raw[i] * t1 + rightCornerColor.raw[i] * (1-t1);
        up.raw[i] = leftUpColor.raw[i] * t1 + rightUpColor.raw[i] * (1-t1);
    }

    Vec3f res;
    for (int i = 0; i < 3; ++i) {
        res.raw[i] = (int)(down.raw[i] * t2 + up.raw[i] * (1-t2));
    }
    return res;
}

Vec3f RenderLoop::barycentric(Vec3f *pts, Vec2i p)
{
    Vec3f u = cross(Vec3f(pts[2].x-pts[0].x, pts[1].x-pts[0].x, pts[0].x-p.x),
        Vec3f(pts[2].y-pts[0].y, pts[1].y-pts[0].y, pts[0].y-p.y));
    if (std::abs(u.z) < 1)
        return Vec3f(-1, 1, 1);
    return Vec3f(1-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}

Vec3f RenderLoop::cross(const Vec3f &a, const Vec3f &b)
{
    return Vec3f(a.y*b.z - a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
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
    auto m1 = Eigen::AngleAxisf(pitch, Eigen::Vector3f::UnitX()) * Eigen::AngleAxisf(yaw, Eigen::Vector3f::UnitY());

//    std::cout << m1.matrix() << std::endl;
    auto m = m1.matrix();
    camera->SetLookDirection(m * camera->GetLookDirection());
    camera->SetUpDirection(  m * camera->GetUpDirection());
}

void RenderLoop::stopRender()
{
    stop = true;
}


