#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) {
        return;
    }
    std::string line;
    std::vector<int> f;
    std::vector<int> tex;
    std::vector<int> norm;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            f.clear(); tex.clear(); norm.clear();
            int texIdx, idx, normalIdx;
            iss >> trash;
            while (iss >> idx >> trash >> texIdx >> trash >> normalIdx) {
                idx--; // in wavefront obj all indices start at 1, not zero
                texIdx--;
                f.push_back(idx);
                tex.push_back(texIdx);
                norm.push_back(normalIdx);
            }
            faces_.push_back(f);
            faceTexs_.push_back(tex);
            faceNormal_.push_back(norm);
        } else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v.raw[i];
            // std::cout << v << std::endl;
            texs_.push_back(Vec2f(v.raw[0], v.raw[1]));
        } else if(!line.compare(0, 3, "vn")) {
            iss >> trash >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v.raw[i];
            normals_.push_back(v);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << " vt# " <<  texs_.size() << " vn# " << normals_.size() <<std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(size_t idx) {
    return faces_[idx];
}

std::vector<int> Model::faceTex(size_t idx) {
    return faceTexs_[idx];
}

Vec3f Model::vert(size_t i) {
    return verts_[i];
}

Vec2f Model::tex(size_t i) {
    return texs_[i];
}

void Model::setVert(int i, Vec3f coord)
{
    verts_[i] = coord;
}
