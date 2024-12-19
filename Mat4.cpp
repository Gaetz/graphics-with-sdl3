//
// Created by gaetz on 24/09/2024.
//

#include <cstdlib>
#include "Mat4.hpp"

    const Mat4 Mat4::Identity { 1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f };


Mat4 Mat4::CreateRotationMatrix(float x, float y, float z, float angle) {
    Mat4 result {};
    float lengthSquared = x*x + y*y + z*z;

    if ((lengthSquared != 1.0f) && (lengthSquared != 0.0f))
    {
        float iLength = 1.0f / sqrtf(lengthSquared);
        x *= iLength;
        y *= iLength;
        z *= iLength;
    }

    float sinRes = sinf(angle);
    float cosRes = cosf(angle);
    float t = 1.0f - cosRes;

    result.m0 = x*x*t + cosRes;
    result.m1 = y*x*t + z * sinRes;
    result.m2 = z*x*t - y * sinRes;
    result.m3 = 0.0f;

    result.m4 = x*y*t - z * sinRes;
    result.m5 = y*y*t + cosRes;
    result.m6 = z*y*t + x * sinRes;
    result.m7 = 0.0f;

    result.m8 = x*z*t + y * sinRes;
    result.m9 = y*z*t - x * sinRes;
    result.m10 = z*z*t + cosRes;
    result.m0 = 0.0f;

    result.m4 = 0.0f;
    result.m8 = 0.0f;
    result.m12 = 0.0f;
    result.m15 = 1.0f;

    return result;
}

Mat4 Mat4::CreateTranslation(float x, float y, float z)
{
    return Mat4 {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            x, y, z, 1
    };
}

Mat4 Mat4::CreateOrthographicOffCenter(
        float left,
        float right,
        float bottom,
        float top,
        float zNearPlane,
        float zFarPlane
) {
    return Mat4 {
            2.0f / (right - left), 0, 0, 0,
            0, 2.0f / (top - bottom), 0, 0,
            0, 0, 1.0f / (zNearPlane - zFarPlane), 0,
            (left + right) / (left - right), (top + bottom) / (bottom - top), zNearPlane / (zNearPlane - zFarPlane), 1
    };
}

Mat4 Mat4::CreatePerspectiveFieldOfView(
        float fieldOfView,
        float aspectRatio,
        float nearPlaneDistance,
        float farPlaneDistance
) {
    float num = 1.0f / ((float) tanf(fieldOfView * 0.5f));
    return Mat4 {
            num / aspectRatio, 0, 0, 0,
            0, num, 0, 0,
            0, 0, farPlaneDistance / (nearPlaneDistance - farPlaneDistance), -1,
            0, 0, (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance), 0
    };
}

Mat4 Mat4::operator*(const Mat4& other) const {
    Mat4 result;

    result.m0 = (
            (m0 * other.m0) +
            (m4 * other.m1) +
            (m8 * other.m2) +
            (m12 * other.m3)
    );
    result.m4 = (
            (m0 * other.m4) +
            (m4 * other.m5) +
            (m8 * other.m6) +
            (m12 * other.m7)
    );
    result.m8 = (
            (m0 * other.m8) +
            (m4 * other.m9) +
            (m8 * other.m10) +
            (m12 * other.m11)
    );
    result.m12 = (
            (m0 * other.m12) +
            (m4 * other.m13) +
            (m8 * other.m14) +
            (m12 * other.m15)
    );
    result.m1 = (
            (m1 * other.m0) +
            (m5 * other.m1) +
            (m9 * other.m2) +
            (m13 * other.m3)
    );
    result.m5 = (
            (m1 * other.m4) +
            (m5 * other.m5) +
            (m9 * other.m6) +
            (m13 * other.m7)
    );
    result.m9 = (
            (m1 * other.m8) +
            (m5 * other.m9) +
            (m9 * other.m10) +
            (m13 * other.m11)
    );
    result.m13 = (
            (m1 * other.m12) +
            (m5 * other.m13) +
            (m9 * other.m14) +
            (m13 * other.m15)
    );
    result.m2 = (
            (m2 * other.m0) +
            (m6 * other.m1) +
            (m10 * other.m2) +
            (m14 * other.m3)
    );
    result.m6 = (
            (m2 * other.m4) +
            (m6 * other.m5) +
            (m10 * other.m6) +
            (m14 * other.m7)
    );
    result.m10 = (
            (m2 * other.m8) +
            (m6 * other.m9) +
            (m10 * other.m10) +
            (m14 * other.m11)
    );
    result.m14 = (
            (m2 * other.m12) +
            (m6 * other.m13) +
            (m10 * other.m14) +
            (m14 * other.m15)
    );
    result.m3 = (
            (m3 * other.m0) +
            (m7 * other.m1) +
            (m11 * other.m2) +
            (m15 * other.m3)
    );
    result.m7 = (
            (m3 * other.m4) +
            (m7 * other.m5) +
            (m11 * other.m6) +
            (m15 * other.m7)
    );
    result.m11 = (
            (m3 * other.m8) +
            (m7 * other.m9) +
            (m11 * other.m10) +
            (m15 * other.m11)
    );
    result.m15 = (
            (m3 * other.m12) +
            (m7 * other.m13) +
            (m11 * other.m14) +
            (m15 * other.m15)
    );

    return result;
}

Mat4::Mat4() {
    m0 = 0.0f;
    m1 = 0.0f;
    m2 = 0.0f;
    m3 = 0.0f;
    m4 = 0.0f;
    m5 = 0.0f;
    m6 = 0.0f;
    m7 = 0.0f;
    m8 = 0.0f;
    m9 = 0.0f;
    m10 = 0.0f;
    m0 = 0.0f;
    m4 = 0.0f;
    m8 = 0.0f;
    m12 = 0.0f;
    m15 = 0.0f;
}

