#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "../utils.h"
class Model {
private:
    std::vector<std::vector<vec3> > triVertexs;
    std::vector<std::vector<vec2> > triUvs;
    std::vector<std::vector<vec3> > triNormals;
    std::vector<vec3> vertexs;
    std::vector<vec2> uvs;
    std::vector<vec3> normals;
    size_t Tris;
public:
	Model(const char *filename);
	~Model();
    void getTriangleVertex(vec3& v, size_t i, size_t j);
    void getTriangleUv(vec2& v, size_t i, size_t j);
    void getTriangleNormal(vec3& v, size_t i, size_t j);
    size_t size() {return Tris;}
};

#endif //__MODEL_H__
