#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "../utils.h"
class Model {
private:
    std::vector<std::vector<Vector3f> > triVertexs;
    std::vector<std::vector<Vector2f> > triUvs;
    std::vector<std::vector<Vector3f> > triNormals;
    std::vector<Vector3f> vertexs;
    std::vector<Vector2f> uvs;
    std::vector<Vector3f> normals;
    size_t Tris;
public:
	Model(const char *filename);
	~Model();
    void getTriangleVertex(Vector3f& v, size_t i, size_t j);
    void getTriangleUv(Vector2f& v, size_t i, size_t j);
    void getTriangleNormal(Vector3f& v, size_t i, size_t j);
    size_t size() {return Tris;}
};

#endif //__MODEL_H__
