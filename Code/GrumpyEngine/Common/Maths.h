#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

using Vec2i = glm::ivec2;
using Vec3i = glm::ivec3;
using Vec4i = glm::ivec4;

using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;

using Mat2 = glm::mat2;
using Mat3 = glm::mat3;
using Mat4 = glm::mat4;

using Quat = glm::quat;

namespace Math
{
    //////////////////////////////////////////////////////////////////////////
    //
    inline float ToDegrees(float radians)
    {
        return glm::degrees(radians);
    }

    inline Vec2 ToDegrees(const Vec2& radians)
    {
        return glm::degrees(radians);
    }

    inline Vec3 ToDegrees(const Vec3& radians)
    {
        return glm::degrees(radians);
    }

    inline Vec4 ToDegrees(const Vec4& radians)
    {
        return glm::degrees(radians);
    }
    //
    //////////////////////////////////////////////////////////////////////////
    //
    inline float ToRadians(float degrees)
    {
        return glm::radians(degrees);
    }

    inline Vec2 ToRadians(const Vec2& degrees)
    {
        return glm::radians(degrees);
    }

    inline Vec3 ToRadians(const Vec3& degrees)
    {
        return glm::radians(degrees);
    }

    inline Vec4 ToRadians(const Vec4& degrees)
    {
        return glm::radians(degrees);
    }
    //
    //////////////////////////////////////////////////////////////////////////
    //
    inline float Length(const Vec2& vector)
    {
        return glm::length(vector);
    }

    inline float Length(const Vec3& vector)
    {
        return glm::length(vector);
    }

    inline float Length(const Vec4& vector)
    {
        return glm::length(vector);
    }
    //
    //////////////////////////////////////////////////////////////////////////
    //
    inline Vec2 Normalize(const Vec2& vector)
    {
        return glm::normalize(vector);
    }

    inline Vec3 Normalize(const Vec3& vector)
    {
        return glm::normalize(vector);
    }

    inline Vec4 Normalize(const Vec4& vector)
    {
        return glm::normalize(vector);
    }

    inline Quat Normalize(const Quat& quat)
    {
        return glm::normalize(quat);
    }
    //
    //////////////////////////////////////////////////////////////////////////
    //
    inline float Mix(float a, float b, float f)
    {
        return glm::mix(a, b, f);
    }

    inline Vec2 Mix(Vec2 a, Vec2 b, float f)
    {
        return glm::mix(a, b, f);
    }

    inline Vec3 Mix(Vec3 a, Vec3 b, float f)
    {
        return glm::mix(a, b, f);
    }

    inline Vec4 Mix(Vec4 a, Vec4 b, float f)
    {
        return glm::mix(a, b, f);
    }
    //
    //////////////////////////////////////////////////////////////////////////
    //
    inline float Distance(float a, float b)
    {
        return glm::distance(a, b);
    }

    inline float Distance(const Vec2& a, const Vec2& b)
    {
        return glm::distance(a, b);
    }

    inline float Distance(const Vec3& a, const Vec3& b)
    {
        return glm::distance(a, b);
    }

    inline float Distance(const Vec4& a, const Vec4& b)
    {
        return glm::distance(a, b);
    }
    //
    //////////////////////////////////////////////////////////////////////////
    //
    inline float Clamp(float value, float min, float max)
    {
        return glm::clamp(value, min, max);
    }

    inline Vec2 Clamp(const Vec2& value, const Vec2& min, const Vec2& max)
    {
        return glm::clamp(value, min, max);
    }

    inline Vec3 Clamp(const Vec3& value, const Vec3& min, const Vec3& max)
    {
        return glm::clamp(value, min, max);
    }

    inline Vec4 Clamp(const Vec4& value, const Vec4& min, const Vec4& max)
    {
        return glm::clamp(value, min, max);
    }
    //
    //////////////////////////////////////////////////////////////////////////
    //
    inline float Max(float a, float b)
    {
        return glm::max(a, b);
    }
    inline float Min(float a, float b)
    {
        return glm::min(a, b);
    }
    //
    //////////////////////////////////////////////////////////////////////////
    //
    inline float Sin(float x)
    {
        return sin(x);
    }

    inline float Cos(float x)
    {
        return cos(x);
    }

    inline float Tan(float x)
    {
        return tan(x);
    }
    //
    //////////////////////////////////////////////////////////////////////////
    //
    inline Mat4 ToMat3(const Quat& quat)
    {
        return glm::toMat3(quat);
    }

    inline Mat4 ToMat4(const Quat& quat)
    {
        return glm::toMat4(quat);
    }
    //
    //////////////////////////////////////////////////////////////////////////
    //
    inline Vec2 MakeVec2(float* data)
    {
        return glm::make_vec2(data);
    }

    inline Vec3 MakeVec3(float* data)
    {
        return glm::make_vec3(data);
    }

    inline Vec4 MakeVec4(float* data)
    {
        return glm::make_vec4(data);
    }

    inline Quat MakeQuat(float* data)
    {
        return glm::make_quat(data);
    }

    inline Vec2 MakeVec2(double* data)
    {
        return glm::make_vec2(data);
    }

    inline Vec3 MakeVec3(double* data)
    {
        return glm::make_vec3(data);
    }

    inline Vec4 MakeVec4(double* data)
    {
        return glm::make_vec4(data);
    }

    inline Quat MakeQuat(double* data)
    {
        return glm::make_quat(data);
    }
    //
    //////////////////////////////////////////////////////////////////////////
    //
    inline Mat4 Translate(const Vec3& translation)
    {
        return glm::translate(Mat4(1.0), translation);
    }

    inline Mat4 Rotate(const Vec3& rotation)
    {
        Mat4 mat(1.0);
        mat *= glm::rotate(Mat4(1.0), Math::ToRadians(rotation.x), Vec3(1, 0, 0));
        mat *= glm::rotate(Mat4(1.0), Math::ToRadians(rotation.y), Vec3(0, 1, 0));
        mat *= glm::rotate(Mat4(1.0), Math::ToRadians(rotation.z), Vec3(0, 0, 1));
        return mat;
    }

    inline Mat4 Rotate(const Quat& quat)
    {
        return glm::toMat4(quat);
    }

    inline Quat Rotate(const Quat& quat, float angle, const Vec3& axis)
    {
        return glm::rotate(quat, angle, axis);
    }

    inline Mat4 Scale(const Vec3& scale)
    {
        return glm::scale(Mat4(1.0), scale);
    }

    inline Mat4 CreateTransformMatrix(const Vec3& position, const Quat& rotation, const Vec3& scale)
    {
        return  glm::translate(Mat4(1.0f), position) *
            Math::ToMat4(rotation) *
            glm::scale(Mat4(1.0f), scale);
    }

    inline Quat AngleAxis(float angle, Vec3 axis)
    {
        return glm::angleAxis(angle, axis);
    }

    inline Mat4 LookAt(const Vec3& pos, const Vec3& target, const Vec3& up)
    {
        return glm::lookAt(pos, target, up);
    }
    //
    //////////////////////////////////////////////////////////////////////////

    inline Vec3 GetEulerAnglesFromQuaternionRotation(const Quat& quatRot)
    {
        Vec3 eulerAngleVector = Vec3(0.0);

        float e = 1.0f;
        eulerAngleVector.y = glm::asin(2.0f * ((quatRot.w * quatRot.y) + (e * (quatRot.x * quatRot.z))));

        if (eulerAngleVector.y > glm::half_pi<float>() - 0.0001f && eulerAngleVector.y < glm::half_pi<float>() + 0.0001f)
        {
            eulerAngleVector.x = atan2f(quatRot.x, quatRot.w);
            eulerAngleVector.z = 0.0f;
        }
        else
        {
            eulerAngleVector.x = atan2f((2.0f * ((quatRot.w * quatRot.x) - (e * (quatRot.y * quatRot.z)))), (1.0f - (2.0f * ((quatRot.x * quatRot.x) + (quatRot.y * quatRot.y)))));
            eulerAngleVector.z = atan2f((2.0f * ((quatRot.w * quatRot.z) - (e * (quatRot.x * quatRot.y)))), (1.0f - (2.0f * ((quatRot.y * quatRot.y) + (quatRot.z * quatRot.z)))));
        }

        return eulerAngleVector;
    }

    inline Vec3 GetForwardVector(const Vec3& rotation)
    {
        Vec3 forward(0.0f, 0.0f, 1.0f);
        Vec3 rotationInRadians = Vec3(Math::ToRadians(rotation.x), Math::ToRadians(rotation.y), Math::ToRadians(rotation.z));
        Quat quat = Quat(rotationInRadians);
        Vec3 v = quat * forward;
        Vec3 result = Vec3(v.x, v.y, v.z);
        return normalize(result);
    }

    inline Vec3 GetUpVector(const Vec3& rotation)
    {
        Vec3 up(0.0f, 1.0f, 0.0f);
        Vec3 rotationInRadians = Vec3(Math::ToRadians(rotation.x), Math::ToRadians(rotation.y), Math::ToRadians(rotation.z));
        Quat quat = Quat(rotationInRadians);
        Vec3 v = quat * up;
        Vec3 result = Vec3(v.x, v.y, v.z);
        return normalize(result);
    }

    inline Vec3 GetSidewaysVector(const Vec3& rotation)
    {
        Vec3 sideway(1.0f, 0.0f, 0.0f);
        Vec3 rotationInRadians = Vec3(Math::ToRadians(rotation.x), Math::ToRadians(rotation.y), Math::ToRadians(rotation.z));
        Quat quat = Quat(rotationInRadians);
        Vec3 v = quat * sideway;
        Vec3 result = Vec3(v.x, v.y, v.z);
        return normalize(result);
    }

    inline Vec3 GetForwardVectorAL(const Vec3& rotation)
    {
        Vec3 forward(0.0f, 0.0f, 1.0f);
        Vec3 rotationInRadians = Vec3(Math::ToRadians(rotation.x), Math::ToRadians(rotation.y), Math::ToRadians(rotation.z));
        Quat quat = Quat(rotationInRadians);
        Vec3 v = quat * forward;
        Vec3 result = Vec3(-v.x, v.y, v.z);
        return normalize(result);
    }

    inline float RandomFloat(float min, float max)
    {
        return min + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (max - min));
    }

    inline Vec3 MoveTowards(const Vec3& current, const Vec3& goal, float delta)
    {
        Vec3 a = goal - current;
        float magnitude = glm::length(a);
        if (magnitude <= delta || magnitude == 0.0f)
        {
            return goal;
        }
        return current + a * delta;
    }

    inline float Damp(const float& current, const float& goal, float lambda, float delta)
    {
        return Math::Mix(current, goal, 1.0f - exp(-lambda * delta));
    }

    inline Vec2 Damp(const Vec2& current, const Vec2& goal, float lambda, float delta)
    {
        return Math::Mix(current, goal, 1.0f - exp(-lambda * delta));
    }

    inline Vec3 Damp(const Vec3& current, const Vec3& goal, float lambda, float delta)
    {
        return Math::Mix(current, goal, 1.0f - exp(-lambda * delta));
    }

    inline float Abs(const float value)
    {
        return abs(value);
    }

    inline Mat4 Inverse(const Mat4& mat)
    {
        return glm::inverse(mat);
    }

    inline Vec3 Cross(const Vec3& a, const Vec3& b)
    {
        return glm::cross(a, b);
    }

    inline float Dot(const Vec3& a, const Vec3& b)
    {
        return glm::dot(a, b);
    }

    inline Mat4 Mat4Cast(const Quat& quat)
    {
        return glm::mat4_cast(quat);
    }
}