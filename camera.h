
#ifndef CAMERA_H
#define CAMERA_H
#include "utils.h"
class Camera
{
public:
    Camera();
    Vector3f GetUpDirection();
    Vector3f GetPosition();
    Vector3f GetRightDirection();
    Vector3f GetLookDirection();
    Matrix4f GetLookAtMatrix4x4();
    void SetLookDirection(Vector3f);
    void SetUpDirection(Vector3f);
    void SetPosition(Vector3f);
    void Rotate(Matrix3f);
private:
    Vector3f up;
    Vector3f pos;
    Vector3f lookDir;
};

#endif // CAMERA_H
