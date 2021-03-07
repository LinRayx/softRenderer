#include "ishader.h"

IShader::IShader(int& w, int& h)
{
    this->setScreenSize(w, h);
    zBuffer = new float[SCREEN_WIDTH * SCREEN_HEIGHT];
}

IShader::~IShader()
{
    if (zBuffer) delete[] zBuffer;
}

float *IShader::GetZBuffer() {
    return zBuffer;
}

void IShader::ClearZBuffer() {
    for (int i = 0; i < SCREEN_WIDTH; ++i) {
        for (int j = 0; j < SCREEN_HEIGHT; ++j) {
            zBuffer[i+j*SCREEN_WIDTH] = std::numeric_limits<float>::min();
        }
    }
}

bool IShader::SetZBuffer(int x, int y, float z) {
    if (abs(x + y* SCREEN_WIDTH) >= SCREEN_WIDTH*SCREEN_HEIGHT) return false;
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return false;
    if (zBuffer[x+y*SCREEN_WIDTH] < z) {
        zBuffer[x+y*SCREEN_WIDTH] = z;
        return true;
    }
    return false;
}

Vector3f IShader::texture(Vector2f uv, TGAImage &image)
{
    int width = image.get_width();
    int height = image.get_height();
    float x = uv[0] * width; float y = uv[1] * height;
    //四个临近像素坐标x像素值
    Vector3f f1,f2,f3,f4;
    Vector3f color;
    Vector3f f12, f34;
    float epsilon = 1e-4f;
    int x1,x2,y1,y2;
    //计算四个临近坐标
    x1 = int(x);
    x2 = x1 + 1;
    y1 = int(y);
    y2 = y1+1;
    //不在图片的范围内
    if((x < 0) || (x > width - 1) || (y < 0) || (y > height - 1))
    {
        return Vector3f(0, 0, 0);
    }
    else{
        if(fabs(x - width+1)<=epsilon) //如果计算点在右测边缘
        {
            //如果差值点在图像的最右下角
            if(fabs(y - height+1)<=epsilon)
            {
                f1 = image.getV3(x1,y1);
                return f1/255;
            }else {
                f1 = image.getV3(x1,y1);
                f3 = image.getV3(x1,y2);

                //图像右方的插值
                return (f1 + (y-y1)*(f3-f1))/255;
            }
        }
        //如果插入点在图像的下方
        else if(fabs(y - height+1)<=epsilon){
            f1 = image.getV3(x1,y1);
            f2 = image.getV3(x2,y1);

            //图像下方的插值
            return  (f1 + (x-x1)*(f2-f1))/255;
        }
        else {
            //得计算四个临近点像素值
            f1 = image.getV3(x1,y1);
            f2 = image.getV3(x2,y1);
            f3 = image.getV3(x1,y2);
            f4 = image.getV3(x2,y2);

            //第一次插值
            f12 = f1 + (x-x1)*(f2-f1); //f(x,0)

            //第二次插值
            f34 = f3 + (x-x1)*(f4-f3); //f(x,1)

            //最终插值
            return (f12 + (y-y1)*(f34-f12))/255;
        }
    }
}

void IShader::setScreenSize(int w, int h) {
    SCREEN_WIDTH = w;
    SCREEN_HEIGHT = h;
}

void IShader::setModelMat(Matrix4f &m)
{
    model = m;
}

void IShader::setViewMat(Matrix4f &m)
{
    view = m;
}

void IShader::setProjectionMat(Matrix4f &m)
{
    projection = m;
}
