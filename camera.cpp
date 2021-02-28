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
    return up.cross(lookDir);
}

Vector3f Camera::GetLookDirection()
{
    return lookDir;
}

void Camera::SetUpDirection(Vector3f _up)
{
    up = _up;
}

void Camera::SetPosition(Vector3f _pos)
{
    pos = _pos;
}
