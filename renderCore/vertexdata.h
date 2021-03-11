#ifndef VERTEXDATA_H
#define VERTEXDATA_H
#include "../utils.h"
class VertexData {
public:
    VertexData() {}
    ~VertexData() {}
    vec3 vertex_pos;
    vec3 normal;
    vec3 light_pos;
    vec3 view_pos;
    vec2 uv;
    mat3 TBN;

};

#endif // VERTEXDATA_H
