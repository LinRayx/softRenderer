#include "MatrixTransform.h"

MatrixTransform::MatrixTransform()
{
    I = mat4(1.);

}

mat4 MatrixTransform::GetTranslationMatrix4x4(vec3 coords)
{

    return translate(I, coords);

}

mat4 MatrixTransform::GetRotationMatrix4x4(vec3 angle)
{

//    std::cout << rot << std::endl;
//    qDebug() << thetax << thetay << thetaz;

    mat4 Rx = rotate(I, angle.x, vec3(1., 0, 0));
    mat4 Ry = rotate(I, angle.y, vec3(0, 1., 0));
    mat4 Rz = rotate(I, angle.z, vec3(0, 0, 1.));
    return Rx * Ry * Rz;
}

mat4 MatrixTransform::GetScaleMatrix4x4(vec3 sca)
{
    mat4 m = scale(I, sca);
    return m;
}

mat4 MatrixTransform::GetPersepectiveMatrix4x4(float fov, float ratio, float near, float far)
{
    return perspective(fov, ratio, near, far);
}

mat4 MatrixTransform::GetModelMatrix4x4(vec3 trans, vec3 rot, vec3 scale)
{

    return GetTranslationMatrix4x4(trans) * GetRotationMatrix4x4(rot) * GetScaleMatrix4x4(scale);
}

mat4 MatrixTransform::GetOrthoMatrix4x4(float width, float height, float near, float far)
{

    mat4 ort = ortho(-width/2, width/2, -height/2, height/2, near, far);
    return ort;
}


mat4 MatrixTransform::GetLookAtMatrix4x4(vec3 &eye, vec3 &center, vec3 &up)
{
    return lookAt(eye, center, up);
}

float MatrixTransform::Radians(float angle) {
    return radians(angle);
}
