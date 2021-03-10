#ifndef VERTEXDATA_H
#define VERTEXDATA_H
#include "../utils.h"
class VertexData {
public:
    VertexData() {}
    ~VertexData() {}
    Vector3f vertex_pos;
    Vector3f normal;
    Vector3f light_pos;
    Vector3f view_pos;
    Vector2f uv;
    Matrix3f TBN;

};

#endif // VERTEXDATA_H
