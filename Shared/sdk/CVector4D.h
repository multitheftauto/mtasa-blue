/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/CVector4D.h
 *  PURPOSE:     4D vector class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cmath>

/**
 * CVector4D Structure used to store a 2D vertex.
 */
class CVector4D
{
public:
    float fX = 0.0f;
    float fY = 0.0f;
    float fZ = 0.0f;
    float fW = 0.0f;

    constexpr CVector4D() noexcept = default;

    constexpr CVector4D(const CVector4D&) noexcept = default;

    constexpr CVector4D& operator=(const CVector4D&) noexcept = default;

    constexpr CVector4D(float x, float y, float z, float w) noexcept :
        fX(x),
        fY(y),
        fZ(z),
        fW(w)
    {
    }

    // Warning, this function is returning the wrong value(fW is missing), its kept because nothing uses it, only
    // CLuaVector4DDefs.
    constexpr float DotProduct(const CVector4D& other) const noexcept { return fX * other.fX + fY * other.fY + fZ * other.fZ; }

    float Length() const noexcept { return sqrt(fX * fX + fY * fY + fZ * fZ + fW * fW); }

    // LengthSquared returns Length() without sqrt applied (i.e. returns x*x* + y*y + z*z + w*w).
    // This can be useful if you only want to compare lengths.
    constexpr float LengthSquared() const noexcept { return (fX * fX) + (fY * fY) + (fZ * fZ) + (fW * fW); }

    void Normalize() noexcept
    {
        const float fLength = Length();
        if (fLength > 0.0f)
        {
            fX /= fLength;
            fY /= fLength;
            fZ /= fLength;
            fW /= fLength;
        }
    }

    constexpr CVector4D operator*(const CVector4D& vecRight) const { return CVector4D(fX * vecRight.fX, fY * vecRight.fY, fZ * vecRight.fZ, fW * vecRight.fW); }

    constexpr CVector4D operator*(const float fRight) const noexcept { return CVector4D(fX * fRight, fY * fRight, fZ * fRight, fW * fRight); }


    constexpr CVector4D operator+(const CVector4D& vecRight) const noexcept { return CVector4D(fX + vecRight.fX, fY + vecRight.fY, fZ + vecRight.fZ, fW + vecRight.fW); }

    constexpr CVector4D operator-(const CVector4D& vecRight) const noexcept { return CVector4D(fX - vecRight.fX, fY - vecRight.fY, fZ - vecRight.fZ, fW - vecRight.fW); }


    constexpr CVector4D operator/(const CVector4D& vecRight) const noexcept { return CVector4D(fX / vecRight.fX, fY / vecRight.fY, fZ / vecRight.fZ, fW / vecRight.fW); }

    constexpr CVector4D operator/(const float fRight) const noexcept { return CVector4D(fX / fRight, fY / fRight, fZ / fRight, fW / fRight); }

    constexpr void operator+=(const float fRight) noexcept
    {
        fX += fRight;
        fY += fRight;
        fZ += fRight;
        fW += fRight;
    }

    constexpr void operator+=(const CVector4D& vecRight) noexcept
    {
        fX += vecRight.fX;
        fY += vecRight.fY;
        fZ += vecRight.fZ;
        fW += vecRight.fW;
    }

    constexpr void operator-=(const float fRight) noexcept
    {
        fX -= fRight;
        fY -= fRight;
        fZ -= fRight;
        fW -= fRight;
    }

    constexpr void operator-=(const CVector4D& vecRight) noexcept
    {
        fX -= vecRight.fX;
        fY -= vecRight.fY;
        fZ -= vecRight.fZ;
        fW -= vecRight.fW;
    }

    constexpr void operator*=(const float fRight) noexcept
    {
        fX *= fRight;
        fY *= fRight;
        fZ *= fRight;
        fW *= fRight;
    }


    constexpr void operator/=(const float fRight) noexcept
    {
        fX /= fRight;
        fY /= fRight;
        fZ /= fRight;
        fW /= fRight;
    }

    constexpr void operator/=(const CVector4D& vecRight) noexcept
    {
        fX /= vecRight.fX;
        fY /= vecRight.fY;
        fZ /= vecRight.fZ;
        fW /= vecRight.fW;
    }

    bool operator==(const CVector4D& param) const noexcept
    {
        return ((fabs(fX - param.fX) < FLOAT_EPSILON) && (fabs(fY - param.fY) < FLOAT_EPSILON) && (fabs(fZ - param.fZ) < FLOAT_EPSILON) &&
                (fabs(fW - param.fW) < FLOAT_EPSILON));
    }

     bool operator!=(const CVector4D& param) const noexcept { return !(*this == param); }
};
