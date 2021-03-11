#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "utils.h"
#include "camera.h"
class MatrixTransform
{

public:
    MatrixTransform();

    mat4 GetTranslationMatrix4x4(vec3 coords);
    mat4 GetRotationMatrix4x4(vec3 rot);
    mat4 GetScaleMatrix4x4(vec3 scale);
    mat4 GetPersepectiveMatrix4x4(float fov, float ratio, float near, float far);
    mat4 GetModelMatrix4x4(vec3 trans, vec3 rot, vec3 scale);
    mat4 GetOrthoMatrix4x4(float width, float height, float near, float far);
    mat4 GetLookAtMatrix4x4(vec3& eye, vec3& center, vec3& Up);
    mat4 GetEularMatrix3x3(float pitch, float yaw, float roll);
    float Radians(float angle);

    mat3 GetRotationMatrix3x3(vec3 rot);
private:
    mat4 _getRotationMatrix4x4(vec3 rot, float theta);
    mat3 _getRotationMatrix3x3(vec3 rot, float theta);
    mat4 I;
};

#endif // TRANSFORM_H
