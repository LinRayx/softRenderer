#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include "geometry.h"
#include <vector>
class FrameBuffer {
public:
    FrameBuffer(const int &w = 500, const int &h = 500) {
        this->Width = w;
        this->Height = h;
        colorBuffer.resize(static_cast<unsigned int>(Width*Height*4)); //RGBA
    }
    ~FrameBuffer() = default;
    void Resize(const int& w, const int& h);
    void ClearColorBuffer();
    void WritePoint(const int &x, const int &y, const Vec4c& color);
    void WritePoint(const int &i, const Vec4c& color);
    unsigned char* data();
    void print();

private:
    int Width;
    int Height;
    std::vector<unsigned char> colorBuffer;

};

#endif
