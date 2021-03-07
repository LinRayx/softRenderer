#include "camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}


Vector3f Camera::GetPosition()
{
    return pos;
}

void Camera::SetPosition(Vector3f _pos)
{
    pos = _pos;
}
