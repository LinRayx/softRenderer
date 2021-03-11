#include "camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}


vec3 Camera::GetPosition()
{
    return pos;
}

void Camera::SetPosition(vec3 _pos)
{
    pos = _pos;
}
