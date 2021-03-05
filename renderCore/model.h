#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec2f> texs_;
    std::vector<Vec3f> normals_;

public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
    Vec3f vert(size_t i);
    Vec2f tex(size_t i);
    void setVert(size_t i, Vec3f);
    std::vector<int> face(size_t idx);
    std::vector<int> faceTex(size_t idx);
    std::vector<std::vector<int> > faces_;
    std::vector<std::vector<int> > faceTexs_;
    std::vector<std::vector<int> > faceNormal_;
};

#endif //__MODEL_H__
