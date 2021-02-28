#include "FrameBuffer.h"

void FrameBuffer::Resize(const int& w, const int& h) {
    Width = w;
    Height = h;
    colorBuffer.resize(w*h*4);
}

void FrameBuffer::ClearColorBuffer(const Vec4c& color) {
    for (auto it = colorBuffer.begin(); it != colorBuffer.end();) {
        *it = color.r;
        *(++it) = color.g;
        *(++it) = color.b;
        *(++it) = color.a;
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

unsigned char* FrameBuffer::data() {
    return colorBuffer.data();
}

void FrameBuffer::print() {
    
}
