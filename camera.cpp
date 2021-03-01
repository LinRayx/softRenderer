#include "camera.h"

Camera::Camera()
{

}

Vector3f Camera::GetUpDirection()
{
    return up;
}

Vector3f Camera::GetPosition()
{
    return pos;
}

Vector3f Camera::GetRightDirection()
{
    return up.cross(lookDir).normalized();
}

Vector3f Camera::GetLookDirection()
{
    return lookDir;
}

void Camera::SetLookDirection(Vector3f _lookDir)
{
    lookDir = _lookDir.normalized();
}

void Camera::SetUpDirection(Vector3f _up)
{
    up = _up.normalized();
}

void Camera::SetPosition(Vector3f _pos)
{
    pos = _pos;
}
