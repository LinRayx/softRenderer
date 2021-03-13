#include "ishader.h"

IShader::IShader(int& w, int& h)
{
    this->setScreenSize(w, h);

}

IShader::~IShader()
{

}

float *IShader::GetZBuffer() {
    return zBuffer;
}

void IShader::ClearZBuffer() {
    for (int i = 0; i < SCREEN_WIDTH; ++i) {
        for (int j = 0; j < SCREEN_HEIGHT; ++j) {
            zBuffer[i+j*SCREEN_WIDTH] = 1;
        }
    }
}

bool IShader::SetZBuffer(int x, int y, float z) {
    if (abs(x + y* SCREEN_WIDTH) >= SCREEN_WIDTH*SCREEN_HEIGHT) return false;
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return false;
//    std::cout << z <<" " << zBuffer[x+y*SCREEN_WIDTH] << std::endl;
    if (zBuffer[x+y*SCREEN_WIDTH] > z) {
        zBuffer[x+y*SCREEN_WIDTH] = z;
        return true;
    }
    return false;
}

vec3 IShader::texture(vec2 uv, TGAImage &image)
{
    int width = image.get_width();
    int height = image.get_height();
    float x = uv[0] * width; float y = uv[1] * height;
    //四个临近像素坐标x像素值
    vec3 f1,f2,f3,f4;
    vec3 f12, f34;
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
        return vec3(0.f);
    }
    else{
        if(fabs(x - width+1)<=epsilon) //如果计算点在右测边缘
        {
            //如果差值点在图像的最右下角
            if(fabs(y - height+1)<=epsilon)
            {
                f1 = image.getV3(x1,y1);
                return f1/255.f;
            }else {
                f1 = image.getV3(x1,y1);
                f3 = image.getV3(x1,y2);

                //图像右方的插值
                return (f1 + (y-y1)*(f3-f1))/255.f;
            }
        }
        //如果插入点在图像的下方
        else if(fabs(y - height+1)<=epsilon){
            f1 = image.getV3(x1,y1);
            f2 = image.getV3(x2,y1);

            //图像下方的插值
            return  (f1 + (x-x1)*(f2-f1))/255.f;
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
            return (f12 + (y-y1)*(f34-f12))/255.f;
        }
    }
}

vec3 IShader::textureShadow(vec2 uv, const std::vector<float> &shadowMap, int width, int height)
{
    float x = uv[0] * width; float y = uv[1] * height;
    //四个临近像素坐标x像素值
    vec3 f1,f2,f3,f4;
    vec3 f12, f34;
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
        return vec3(0.f);
    }
    else{
        if(fabs(x - width+1)<=epsilon) //如果计算点在右测边缘
        {
            //如果差值点在图像的最右下角
            if(fabs(y - height+1)<=epsilon)
            {
                f1 = vec3(shadowMap[x + y * width]);
                return f1;
            }else {
                f1 = vec3(shadowMap[x1 + y1 * width]);
                f3 = vec3(shadowMap[x1 + y2 * width]);

                //图像右方的插值
                return (f1 + (y-y1)*(f3-f1));
            }
        }
        //如果插入点在图像的下方
        else if(fabs(y - height+1)<=epsilon){
            f1 = vec3(shadowMap[x1 + y1 * width]);
            f2 = vec3(shadowMap[x2 + y1 * width]);

            //图像下方的插值
            return  (f1 + (x-x1)*(f2-f1));
        }
        else {
            //得计算四个临近点像素值
            f1 = vec3(shadowMap[x1 + y1 * width]);
            f2 = vec3(shadowMap[x2 + y1 * width]);
            f3 = vec3(shadowMap[x1 + y2 * width]);
            f4 = vec3(shadowMap[x2 + y2 * width]);

            //第一次插值
            f12 = f1 + (x-x1)*(f2-f1); //f(x,0)

            //第二次插值
            f34 = f3 + (x-x1)*(f4-f3); //f(x,1)

            //最终插值
            return (f12 + (y-y1)*(f34-f12));
        }
    }
}

void IShader::setScreenSize(int w, int h) {
    SCREEN_WIDTH = w;
    SCREEN_HEIGHT = h;
}

void IShader::setModelMat(mat4 &m)
{
    model = m;
}

void IShader::setViewMat(mat4 &m)
{
    view = m;
}

void IShader::setProjectionMat(mat4 &m)
{
    projection = m;
}

void IShader::setZBuffer(float *zb)
{
    zBuffer = zb;
}
