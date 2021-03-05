#ifndef FRAGMENTDATA_H
#define FRAGMENTDATA_H
#include "../utils.h"
class FragmentData {
public:
    FragmentData();
    ~FragmentData();
    Vector3f tangentLightPos;
    Vector3f tangentViewPos;
    Vector3f tangentWorldPos;
    Vector3f screen_pos;
    Vector3f world_pos;
    Vector3f normal;
    Vector2f uv;
    Vector3f lightColor;
};

#endif // FRAGMENTDATA_H
