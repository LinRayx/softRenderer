#include "FrameBuffer.h"

void FrameBuffer::Resize(const int& w, const int& h) {
    Width = w;
    Height = h;
    colorBuffer.resize(static_cast<unsigned int>(w*h*4));    
}

void FrameBuffer::ClearColorBuffer() {
    for (auto it = colorBuffer.begin(); it != colorBuffer.end();) {
        *it = 0;
        *(++it) = 0;
        *(++it) = 0;
        *(++it) = 255;
//        *it = 255;
//        *(++it) = 255;
//        *(++it) = 255;
//        *(++it) = 255;
        ++it;
    }
}

void FrameBuffer::WritePoint(const int &x, const int &y, const Vec4c& color) {
    if (x < 0 || x >= Width || y < 0 || y >= Height)
			return;

    int xy = (y * Width + x);
    unsigned char * p = colorBuffer.data();
    *(p + xy*4) = color.r;
    *(p + xy*4 + 1) = color.g;
    *(p + xy*4 + 2) = color.b;
    *(p + xy*4 + 3) = color.a;

}

void FrameBuffer::WritePoint(const int &i, const Vec4c &color)
{
    unsigned char * p = colorBuffer.data();
    *(p + i*4) = color.r;
    *(p + i*4 + 1) = color.g;
    *(p + i*4 + 2) = color.b;
    *(p + i*4 + 3) = color.a;
}

void FrameBuffer::DoMsaa(const int i, const float coef)
{
    unsigned char * p = colorBuffer.data();
    *(p + i*4) = static_cast<unsigned char>(*(p + i*4) * coef);
    *(p + i*4 + 1) = static_cast<unsigned char>(*(p + i*4 + 1) * coef);
    *(p + i*4 + 2) = static_cast<unsigned char>(*(p + i*4 + 2) * coef);

}

unsigned char* FrameBuffer::data() {
    return colorBuffer.data();
}

void FrameBuffer::print() {
    
}
