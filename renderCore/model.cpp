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
            std::vector<int> f;
            std::vector<int> tex;
            int texIdx, idx, itrash;
            iss >> trash;
            while (iss >> idx >> trash >> texIdx >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                texIdx--;
                f.push_back(idx);
                tex.push_back(texIdx);
            }
            faces_.push_back(f);
            faceTexs_.push_back(tex);
        } else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v.raw[i];
            // std::cout << v << std::endl;
            texs_.push_back(Vec2f(v.raw[0], v.raw[1]));
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << " vt# " <<  texs_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

std::vector<int> Model::faceTex(int idx) {
    return faceTexs_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec2f Model::tex(int i) {
    return texs_[i];
}
