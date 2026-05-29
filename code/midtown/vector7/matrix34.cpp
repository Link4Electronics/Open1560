/*
    Open1560 - An Open Source Re-Implementation of Midtown Madness 1 Beta
    Copyright (C) 2020 Brick

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

define_dummy_symbol(vector7_matrix34);

#include "matrix34.h"

#include "data7/metadefine.h"

#include <cmath>

META_DEFINE("Matrix34", Matrix34)
{
    META_FIELD("a", m0);
    META_FIELD("b", m1);
    META_FIELD("c", m2);
    META_FIELD("d", m3);
}

// this = arg1 * arg2 (3x4 matrix mul)
void Matrix34::Dot(const Matrix34& a, const Matrix34& b)
{
    m0.x = a.m0.x * b.m0.x + a.m0.y * b.m1.x + a.m0.z * b.m2.x;
    m0.y = a.m0.x * b.m0.y + a.m0.y * b.m1.y + a.m0.z * b.m2.y;
    m0.z = a.m0.x * b.m0.z + a.m0.y * b.m1.z + a.m0.z * b.m2.z;

    m1.x = a.m1.x * b.m0.x + a.m1.y * b.m1.x + a.m1.z * b.m2.x;
    m1.y = a.m1.x * b.m0.y + a.m1.y * b.m1.y + a.m1.z * b.m2.y;
    m1.z = a.m1.x * b.m0.z + a.m1.y * b.m1.z + a.m1.z * b.m2.z;

    m2.x = a.m2.x * b.m0.x + a.m2.y * b.m1.x + a.m2.z * b.m2.x;
    m2.y = a.m2.x * b.m0.y + a.m2.y * b.m1.y + a.m2.z * b.m2.y;
    m2.z = a.m2.x * b.m0.z + a.m2.y * b.m1.z + a.m2.z * b.m2.z;

    m3.x = a.m0.x * b.m3.x + a.m0.y * b.m3.y + a.m0.z * b.m3.z + a.m3.x;
    m3.y = a.m1.x * b.m3.x + a.m1.y * b.m3.y + a.m1.z * b.m3.z + a.m3.y;
    m3.z = a.m2.x * b.m3.x + a.m2.y * b.m3.y + a.m2.z * b.m3.z + a.m3.z;
}

// Replace the rotation part (m0,m1,m2) with rotation of (axis, angle).
// Preserves translation part (m3).
void Matrix34::RotateAbs(const Vector3& axis, f32 angle)
{
    f32 c = std::cos(angle);
    f32 s = std::sin(angle);

    // Handle axis-aligned cases efficiently
    if (axis.z != 0.0f)
    {
        if (axis.x == 0.0f && axis.y == 0.0f)
        {
            f32 sign = (axis.z > 0.0f) ? 1.0f : -1.0f;
            f32 cs = c;
            f32 ss = s * sign;
            m0 = Vector3{cs, ss, 0.0f};
            m1 = Vector3{-ss, cs, 0.0f};
            m2 = Vector3{0.0f, 0.0f, 1.0f};
            return;
        }
    }
    else if (axis.y != 0.0f)
    {
        if (axis.x == 0.0f)
        {
            f32 sign = (axis.y > 0.0f) ? 1.0f : -1.0f;
            f32 cs = c;
            f32 ss = s * sign;
            m0 = Vector3{cs, 0.0f, -ss};
            m1 = Vector3{0.0f, 1.0f, 0.0f};
            m2 = Vector3{ss, 0.0f, cs};
            return;
        }
    }
    else if (axis.x != 0.0f)
    {
        f32 sign = (axis.x > 0.0f) ? 1.0f : -1.0f;
        f32 cs = c;
        f32 ss = s * sign;
        m0 = Vector3{1.0f, 0.0f, 0.0f};
        m1 = Vector3{0.0f, cs, ss};
        m2 = Vector3{0.0f, -ss, cs};
        return;
    }

    // Zero axis — set identity rotation
    m0 = Vector3{1.0f, 0.0f, 0.0f};
    m1 = Vector3{0.0f, 1.0f, 0.0f};
    m2 = Vector3{0.0f, 0.0f, 1.0f};
}

void Matrix34::FastInverse(const Matrix34& a)
{
    // Transpose the 3x3 rotation part
    m0.x = a.m0.x;
    m0.y = a.m1.x;
    m0.z = a.m2.x;

    m1.x = a.m0.y;
    m1.y = a.m1.y;
    m1.z = a.m2.y;

    m2.x = a.m0.z;
    m2.y = a.m1.z;
    m2.z = a.m2.z;

    // Inverse translation: -R^T * t
    m3.x = -(a.m3.x * m0.x + a.m3.y * m1.x + a.m3.z * m2.x);
    m3.y = -(a.m3.x * m0.y + a.m3.y * m1.y + a.m3.z * m2.y);
    m3.z = -(a.m3.x * m0.z + a.m3.y * m1.z + a.m3.z * m2.z);
}

// Post-multiply by rotation around axis: this = this * R(axis, angle)
void Matrix34::Rotate(const Vector3& axis, f32 angle)
{
    f32 c = std::cos(angle);
    f32 s = std::sin(angle);
    f32 t = 1.0f - c;

    f32 x = axis.x;
    f32 y = axis.y;
    f32 z = axis.z;

    // Rodrigues' rotation formula
    f32 tx = t * x;
    f32 ty = t * y;
    f32 tz = t * z;

    Vector3 r0(tx * x + c, tx * y + s * z, tx * z - s * y);
    Vector3 r1(tx * y - s * z, ty * y + c, ty * z + s * x);
    Vector3 r2(tx * z + s * y, ty * z - s * x, tz * z + c);

    // Post-multiply: this = this * R (only rotation part, translation unchanged)
    Vector3 n0 = r0 * m0.x + r1 * m0.y + r2 * m0.z;
    Vector3 n1 = r0 * m1.x + r1 * m1.y + r2 * m1.z;
    Vector3 n2 = r0 * m2.x + r1 * m2.y + r2 * m2.z;

    m0 = n0;
    m1 = n1;
    m2 = n2;
}

// this = a * b (3x3 dot product, no translation)
void Matrix34::Dot3x3(const Matrix34& a, const Matrix34& b)
{
    m0.x = a.m0.x * b.m0.x + a.m0.y * b.m1.x + a.m0.z * b.m2.x;
    m0.y = a.m0.x * b.m0.y + a.m0.y * b.m1.y + a.m0.z * b.m2.y;
    m0.z = a.m0.x * b.m0.z + a.m0.y * b.m1.z + a.m0.z * b.m2.z;

    m1.x = a.m1.x * b.m0.x + a.m1.y * b.m1.x + a.m1.z * b.m2.x;
    m1.y = a.m1.x * b.m0.y + a.m1.y * b.m1.y + a.m1.z * b.m2.y;
    m1.z = a.m1.x * b.m0.z + a.m1.y * b.m1.z + a.m1.z * b.m2.z;

    m2.x = a.m2.x * b.m0.x + a.m2.y * b.m1.x + a.m2.z * b.m2.x;
    m2.y = a.m2.x * b.m0.y + a.m2.y * b.m1.y + a.m2.z * b.m2.y;
    m2.z = a.m2.x * b.m0.z + a.m2.y * b.m1.z + a.m2.z * b.m2.z;
}

// this = *this * S (post-multiply by scale matrix)
void Matrix34::Scale(f32 s)
{
    m0 *= s;
    m1 *= s;
    m2 *= s;
}

void Matrix34::Scale(f32 sx, f32 sy, f32 sz)
{
    m0 *= sx;
    m1 *= sy;
    m2 *= sz;
}

// this = S * *this (pre-multiply by scale matrix, replacing rotation)
void Matrix34::ScaleAbs(f32 sx, f32 sy, f32 sz)
{
    m0.x *= sx;
    m0.y *= sy;
    m0.z *= sz;

    m1.x *= sx;
    m1.y *= sy;
    m1.z *= sz;

    m2.x *= sx;
    m2.y *= sy;
    m2.z *= sz;
}

// this = S * *this (pre-multiply by scale matrix, replacing full matrix)
void Matrix34::ScaleFullAbs(f32 sx, f32 sy, f32 sz)
{
    m0.x *= sx;
    m0.y *= sy;
    m0.z *= sz;

    m1.x *= sx;
    m1.y *= sy;
    m1.z *= sz;

    m2.x *= sx;
    m2.y *= sy;
    m2.z *= sz;

    m3.x *= sx;
    m3.y *= sy;
    m3.z *= sz;
}

// Normalize rotation rows to unit length
void Matrix34::Normalize()
{
    m0 = ~m0;
    m1 = ~m1;
    m2 = ~m2;
}

// Full inverse (for non-orthonormal matrices, uses cofactors)
Matrix34 Matrix34::Inverse() const
{
    f32 det = m0.x * (m1.y * m2.z - m1.z * m2.y) - m0.y * (m1.x * m2.z - m1.z * m2.x) +
              m0.z * (m1.x * m2.y - m1.y * m2.x);

    if (det == 0.0f)
        return IDENTITY;

    f32 inv_det = 1.0f / det;

    Matrix34 result;
    result.m0.x = (m1.y * m2.z - m1.z * m2.y) * inv_det;
    result.m0.y = (m0.z * m2.y - m0.y * m2.z) * inv_det;
    result.m0.z = (m0.y * m1.z - m0.z * m1.y) * inv_det;

    result.m1.x = (m1.z * m2.x - m1.x * m2.z) * inv_det;
    result.m1.y = (m0.x * m2.z - m0.z * m2.x) * inv_det;
    result.m1.z = (m0.z * m1.x - m0.x * m1.z) * inv_det;

    result.m2.x = (m1.x * m2.y - m1.y * m2.x) * inv_det;
    result.m2.y = (m0.y * m2.x - m0.x * m2.y) * inv_det;
    result.m2.z = (m0.x * m1.y - m0.y * m1.x) * inv_det;

    result.m3.x = -(m3.x * result.m0.x + m3.y * result.m1.x + m3.z * result.m2.x);
    result.m3.y = -(m3.x * result.m0.y + m3.y * result.m1.y + m3.z * result.m2.y);
    result.m3.z = -(m3.x * result.m0.z + m3.y * result.m1.z + m3.z * result.m2.z);

    return result;
}