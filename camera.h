
#ifndef CAMERA_H
#define CAMERA_H
#include "utils.h"
class Camera
{
public:
    Camera();
    ~Camera();
    Vector3f GetPosition();
    void SetPosition(Vector3f);
private:
    Vector3f pos;
};

#endif // CAMERA_H
