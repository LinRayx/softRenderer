
#ifndef CAMERA_H
#define CAMERA_H
#include "utils.h"
class Camera
{
public:
    Camera();
    ~Camera();
    vec3 GetPosition();
    void SetPosition(vec3);
private:
    vec3 pos;
};

#endif // CAMERA_H
