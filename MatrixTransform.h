#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "utils.h"
#include "camera.h"
class MatrixTransform
{

public:
    MatrixTransform();

    Matrix4f GetTranslationMatrix4x4(Vector3f coords);
    Matrix4f GetRotationMatrix4x4(Vector3f rot);
    Matrix4f GetScaleMatrix4x4(Vector3f scale);
    Matrix4f GetPersepectiveMatrix4x4(float fov, float ratio, float near, float far);
    Matrix4f GetModelMatrix4x4(Vector3f trans, Vector3f rot, Vector3f scale);
    Matrix4f GetViewMatrix4x4(Camera *camera);


    Matrix3f GetEularMatrix3x3(float pitch, float yaw, float roll);
    float Radians(float angle);

    Matrix3f GetRotationMatrix3x3(Vector3f rot);
private:
    Matrix4f _getRotationMatrix4x4(Vector3f rot, float theta);
    Matrix3f _getRotationMatrix3x3(Vector3f rot, float theta);
};

#endif // TRANSFORM_H
