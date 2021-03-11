#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) {
        std::cerr << "read obj failed" << std::endl;
        return;
    }
    std::string line;
    std::vector<vec3> ver;
    std::vector<vec2> tex;
    std::vector<vec3> norm;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            vec3 v;
            for (int i=0;i<3;i++) iss >> v[i];
            vertexs.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            ver.clear(); tex.clear(); norm.clear();
            size_t texIdx, vertexIdx, normalIdx;
            iss >> trash;
            while (iss >> vertexIdx >> trash >> texIdx >> trash >> normalIdx) {
                vertexIdx--; // in wavefront obj all indices start at 1, not zero
                texIdx--;
                normalIdx--;
                ver.push_back(vertexs[vertexIdx]);
                tex.push_back(uvs[texIdx]);
                norm.push_back(normals[normalIdx]);
            }
            triVertexs.push_back(ver);
            triUvs.push_back(tex);
            triNormals.push_back(norm);

        } else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            vec2 v;
            for (int i=0;i<2;i++) iss >> v[i];
            uvs.push_back(v);
            iss >> v[0];
        } else if(!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            vec3 v;
            for (int i=0;i<3;i++) iss >> v[i];
            normals.push_back(v);
        }
    }
    Tris = triVertexs.size();
    std::cerr << "# v# " << vertexs.size() << " f# "  << triVertexs.size() << " vt# " <<  uvs.size() << " vn# " << normals.size() <<std::endl;
}

Model::~Model() {
}


void Model::getTriangleUv(vec2 &v, size_t i, size_t j)
{

    v = triUvs[i][j];
}

void Model::getTriangleNormal(vec3 &v, size_t i, size_t j)
{
    v = triNormals[i][j];
}

void Model::getTriangleVertex(vec3 &v, size_t i, size_t j)
{
    v = triVertexs[i][j];
}


