/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/CVector2D.h
 *  PURPOSE:     2D vector class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cmath>
#include "CVector.h"

/**
 * CVector2D Structure used to store a 2D vertex.
 */
class CVector2D
{
public:
    float fX = 0.0f;
    float fY = 0.0f;

    constexpr CVector2D() noexcept = default;

    constexpr CVector2D(float x, float y) noexcept : fX(x), fY(y) {}

    constexpr CVector2D(const CVector& vec) noexcept : fX(vec.fX), fY(vec.fY) {}

    constexpr CVector2D(const CVector4D& vec) noexcept : fX(vec.fX), fY(vec.fY) {}

    constexpr CVector2D& operator=(const CVector& vec) noexcept
    {
        fX = vec.fX;
        fY = vec.fY;
        return *this;
    }

    constexpr CVector2D& operator=(const CVector4D& vec) noexcept
    {
        fX = vec.fX;
        fY = vec.fY;
        return *this;
    }

    constexpr float DotProduct(CVector2D& other) const { return fX * other.fX + fY * other.fY; }

    float Length() const { return std::hypotf(fX, fY); }

    // LengthSquared returns Length() without sqrt applied (i.e. returns x*x* + y*y).
    // This can be useful if you only want to compare lengths.
    constexpr float LengthSquared() const noexcept { return (fX * fX) + (fY * fY); }

    void Normalize() noexcept
    {
        const float fLength = Length();
        if (fLength > 0.0f)
        {
            fX /= fLength;
            fY /= fLength;
        }
    }

    constexpr CVector2D operator*(float fRight) const noexcept { return CVector2D(fX * fRight, fY * fRight); }

    constexpr CVector2D operator/(float fRight) const noexcept { return CVector2D(fX / fRight, fY / fRight); }

    constexpr CVector2D operator+(const CVector2D& vecRight) const noexcept { return CVector2D(fX + vecRight.fX, fY + vecRight.fY); }

    constexpr CVector2D operator-(const CVector2D& vecRight) const noexcept { return CVector2D(fX - vecRight.fX, fY - vecRight.fY); }

    constexpr CVector2D operator*(const CVector2D& vecRight) const noexcept { return CVector2D(fX * vecRight.fX, fY * vecRight.fY); }

    constexpr CVector2D operator/(const CVector2D& vecRight) const noexcept { return CVector2D(fX / vecRight.fX, fY / vecRight.fY); }

    constexpr void operator+=(float fRight) noexcept
    {
        fX += fRight;
        fY += fRight;
    }

    constexpr void operator+=(const CVector2D& vecRight) noexcept
    {
        fX += vecRight.fX;
        fY += vecRight.fY;
    }

    constexpr void operator-=(float fRight) noexcept
    {
        fX -= fRight;
        fY -= fRight;
    }

    constexpr void operator-=(const CVector2D& vecRight) noexcept
    {
        fX -= vecRight.fX;
        fY -= vecRight.fY;
    }

    constexpr void operator*=(float fRight) noexcept
    {
        fX *= fRight;
        fY *= fRight;
    }

    constexpr void operator*=(const CVector2D& vecRight) noexcept
    {
        fX *= vecRight.fX;
        fY *= vecRight.fY;
    }

    constexpr void operator/=(float fRight) noexcept
    {
        fX /= fRight;
        fY /= fRight;
    }

    constexpr void operator/=(const CVector2D& vecRight) noexcept
    {
        fX /= vecRight.fX;
        fY /= vecRight.fY;
    }

    bool operator==(const CVector2D& param) const noexcept { return ((fabs(fX - param.fX) < FLOAT_EPSILON) && (fabs(fY - param.fY) < FLOAT_EPSILON)); }

    bool operator!=(const CVector2D& param) const noexcept { return !(*this == param); }
};
