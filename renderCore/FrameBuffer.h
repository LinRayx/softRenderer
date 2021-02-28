#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__
#include "geometry.h"
#include <vector>
class FrameBuffer {
public:
    FrameBuffer(const int &w = 1024, const int &h = 1024) {
        this->Width = w;
        this->Height = h;
        colorBuffer.resize(Width*Height*4); //RGBA
    }
    ~FrameBuffer() = default;
    void Resize(const int& w, const int& h);
    void ClearColorBuffer(const Vec4c& color);
    void WritePoint(const int &x, const int &y, const Vec4c& color);
    unsigned char* data();
    void print();

private:
    int Width;
    int Height;
    std::vector<unsigned char> colorBuffer;

};

#endif
