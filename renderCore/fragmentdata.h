#ifndef FRAGMENTDATA_H
#define FRAGMENTDATA_H
#include "../utils.h"
class FragmentData {
public:
    FragmentData() {}
    ~FragmentData() {}
    Vector3f lightDir;
    Vector3f viewDir;
    Vector3f screen_pos;
    Vector3f world_pos;
    Vector3f normal;
    Vector2f uv;
    Vector3f lightColor;
};

#endif // FRAGMENTDATA_H
