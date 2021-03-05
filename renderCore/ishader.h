#ifndef ISHADER_H
#define ISHADER_H

#include "../utils.h"
#include "../renderCore/fragmentdata.h"
class IShader
{
public:
    IShader();
    virtual ~IShader();
    virtual Vector3f vertexShader(Vector3f pos) = 0;
    virtual Vector4i fragmentShader(FragmentData& fragmentData) = 0;
    Vector3f texture(Vector2f& uv, TGAImage &image)
    {
        uv[0] *= image.get_width();
        uv[1] *= image.get_height();
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
            res[i] = (down.raw[i] * t2 + up.raw[i] * (1-t2))/255;
        }
        return res;
    }
    void setScreenSize(int w, int h) {
        SCREEN_WIDTH = w;
        SCREEN_HEIGHT = h;
    }
    int SCREEN_HEIGHT, SCREEN_WIDTH;
};

#endif // ISHADER_H
