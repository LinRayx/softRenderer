#include "MatrixTransform.h"

MatrixTransform::MatrixTransform()
{

}

Matrix4f MatrixTransform::GetTranslationMatrix4x4(Vector3f coords)
{
    Matrix4Xf m;
    m << 1, 0, 0, coords.x() ,
            0, 1, 0, coords.y() ,
            0, 0, 1, coords.z() ,
            0, 0, 0, 1;
    return m;
}

Matrix4f MatrixTransform::GetRotationMatrix4x4(Vector3f rot)
{
    Matrix4f Rx = _getRotationMatrix4x4(Vector3f(1, 0, 0), rot.x() / 180 * EIGEN_PI);
    Matrix4f Ry = _getRotationMatrix4x4(Vector3f(0, 1, 0), rot.y() / 180 * EIGEN_PI);
    Matrix4f Rz = _getRotationMatrix4x4(Vector3f(0, 0, 1), rot.z() / 180 * EIGEN_PI);
    return Rx * Ry * Rz;
}

Matrix4f MatrixTransform::GetScaleMatrix4x4(Vector3f scale)
{
    Matrix4Xf m;
    m << scale.x(), 0, 0, 0,
         0, scale.y(), 0, 0,
          0, 0, scale.z(), 0,
                0, 0, 0, 1;
    return m;
}

Matrix4f MatrixTransform::GetPersepectiveMatrix4x4(float fov, float ratio, float near, float far)
{
    float t = near * tanf(fov/2);
    float r = t * ratio;
    float f = far;
    float n = near;
    Matrix4f m;
    m << near / r, 0, 0, 0,
            0, n / t, 0, 0,
            0, 0, -(f+n)/(f-n), - 2*n*f/(f-n),
            0, 0, -1, 0;
    return m;
}

Matrix4f MatrixTransform::GetModelMatrix4x4(Vector3f trans, Vector3f rot, Vector3f scale)
{
    Matrix4f m = GetScaleMatrix4x4(scale) * GetTranslationMatrix4x4(trans) * GetRotationMatrix4x4(rot);
    return m;
}

Matrix4f MatrixTransform::GetViewMatrix4x4(Camera *camera)
{
    Vector3f up = camera->GetUpDirection();
    Vector3f right = camera->GetRightDirection();
    Vector3f look = camera->GetLookDirection();
    Vector3f pos = camera->GetPosition();

    Matrix4f m1, m2;
    m1 << right.x(), right.y(), right.z(), 0,
            up.x(), up.y(), up.z(), 0,
            look.x(), look.y(), look.z(), 0,
            0, 0, 0, 1;
    m2 << 1, 0, 0, -pos.x(),
            0, 1, 0, -pos.y(),
            0, 0, 1, -pos.z(),
            0, 0, 0, 1;
    return m1 * m2;
}

Matrix4f MatrixTransform::_getRotationMatrix4x4(Vector3f rot, float theta)
{
    Matrix4Xf m;
    float c = cosf(theta);
    float s = sinf(theta);
    qDebug() << c << s;
    float Ax = rot.x(), Ay = rot.y(), Az = rot.z();
    m << c+(1-c)*Ax*Ax, (1-c)*Ax*Ay-s*Az, (1-c)*Ax*Az+s*Ay, 0,
            (1-c)*Ax*Ay+s*Az, c+(1-c)*Ay*Ay, (1-c)*Ay*Az-s*Ax, 0,
            (1-c)*Ax*Az-s*Ay, (1-c)*Ay*Az+s*Ax, c+(1-c)*Az*Az, 0,
            0, 0, 0, 1;
    return m;
}

