#ifndef FRAGMENTDATA_H
#define FRAGMENTDATA_H
#include "../utils.h"
class FragmentData {
public:
    FragmentData() {}
    ~FragmentData() {}
    vec3 lightDir;
    vec3 viewDir;
    vec4 screen_pos;
    vec3 world_pos;
    vec3 normal;
    vec2 uv;
    vec3 lightColor;
    std::vector<float> shadowMap;
    vec4 lightSpacePos;
};

#endif // FRAGMENTDATA_H
