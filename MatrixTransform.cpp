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
    float thetax = rot.x() / 180.0f * PI;
    float thetay = rot.y() / 180.0f * PI;
    float thetaz = rot.z() / 180.0f * PI;
//    std::cout << rot << std::endl;
//    qDebug() << thetax << thetay << thetaz;
    Matrix4f Rx = _getRotationMatrix4x4(Vector3f(1, 0, 0), thetax);
    Matrix4f Ry = _getRotationMatrix4x4(Vector3f(0, 1, 0), thetay);
    Matrix4f Rz = _getRotationMatrix4x4(Vector3f(0, 0, 1), thetaz);
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
            0, near / t, 0, 0,
            0, 0, -(f+n)/(f-n), - 2*n*f/(f-n),
            0, 0, -1, 0;
    return m;
}

Matrix4f MatrixTransform::GetModelMatrix4x4(Vector3f trans, Vector3f rot, Vector3f scale)
{
    Matrix4f m = GetScaleMatrix4x4(scale) * GetTranslationMatrix4x4(trans) * GetRotationMatrix4x4(rot);
    return m;
}

Matrix4f MatrixTransform::GetOrthoMatrix4x4(float width, float height, float near, float far)
{
    Matrix4f m;
    m << 2/width, 0, 0, 0,
            0, 2/height, 0, 0,
            0, 0, -2/(far-near), -(far+near)/(far-near),
            0, 0, 0, 1;
    return m;
}

Matrix3f MatrixTransform::GetRotationMatrix3x3(Vector3f rot)
{
    float thetax = rot.x() / 180.0f * PI;
    float thetay = rot.y() / 180.0f * PI;
    float thetaz = rot.z() / 180.0f * PI;
//    std::cout << rot << std::endl;
//    qDebug() << thetax << thetay << thetaz;
    Matrix3f Rx = _getRotationMatrix3x3(Vector3f(1, 0, 0), thetax);
    Matrix3f Ry = _getRotationMatrix3x3(Vector3f(0, 1, 0), thetay);
    Matrix3f Rz = _getRotationMatrix3x3(Vector3f(0, 0, 1), thetaz);
    return Rx * Ry * Rz;
}


Matrix4f MatrixTransform::GetLookAtMatrix4x4(Vector3f &eye, Vector3f &center, Vector3f &Up)
{
    Vector3f z = (eye - center).normalized();
    Vector3f x = Up.cross(z).normalized();
    Vector3f y = z.cross(x).normalized();
    Matrix4f m = Matrix4f::Identity();

    Matrix4f t = Matrix4f::Identity();
    for (int i = 0; i < 3; ++i) {
        m(0, i) = x[i];
        m(1, i) = y[i];
        m(2, i) = z[i];
        t(i, 3) = -eye[i];
    }
    return m * t;
}

Matrix4f MatrixTransform::_getRotationMatrix4x4(Vector3f rot, float theta)
{

    Matrix4f m;
    float c = cosf(theta);
    float s = sinf(theta);
//    qDebug() << theta << c << s;
    float Ax = rot.x(), Ay = rot.y(), Az = rot.z();
    m << c+(1.f-c)*Ax*Ax, (1.f-c)*Ax*Ay-s*Az, (1.f-c)*Ax*Az+s*Ay, 0.f,
            (1.f-c)*Ax*Ay+s*Az, c+(1.f-c)*Ay*Ay, (1.f-c)*Ay*Az-s*Ax, 0.f,
            (1-c)*Ax*Az-s*Ay, (1-c)*Ay*Az+s*Ax, c+(1-c)*Az*Az, 0.f,
            0.f, 0.f, 0.f, 1.f;
    return m;
}

Matrix3f MatrixTransform::_getRotationMatrix3x3(Vector3f rot, float theta)
{

    Matrix3f m;
    float c = cosf(theta);
    float s = sinf(theta);
//    qDebug() << theta << c << s;
    float Ax = rot.x(), Ay = rot.y(), Az = rot.z();
    m << c+(1.f-c)*Ax*Ax, (1.f-c)*Ax*Ay-s*Az, (1.f-c)*Ax*Az+s*Ay,
            (1.f-c)*Ax*Ay+s*Az, c+(1.f-c)*Ay*Ay, (1.f-c)*Ay*Az-s*Ax,
            (1-c)*Ax*Az-s*Ay, (1-c)*Ay*Az+s*Ax, c+(1-c)*Az*Az;
    return m;
}

Matrix3f MatrixTransform::GetEularMatrix3x3(float pitch, float yaw, float roll) {
    Eigen::AngleAxisf rollAngle(roll, Eigen::Vector3f::UnitX());
    Eigen::AngleAxisf pitchAngle(pitch, Eigen::Vector3f::UnitY());
    Eigen::AngleAxisf yawAngle(yaw, Eigen::Vector3f::UnitZ());
    return yawAngle.matrix() * pitchAngle.matrix() * rollAngle.matrix();
}

float MatrixTransform::Radians(float angle) {
    return angle / 180 * PI;
}
