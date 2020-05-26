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
#include "CVector.h"

/**
 * CVector4D Structure used to store a 2D vertex.
 */
class CVector4D
{
public:
    constexpr CVector4D() :
        fX(0),
        fY(0),
        fZ(0),
        fW(0)
    {
    };

    constexpr CVector4D(float x, float y, float z, float w) :
        fX(x),
        fY(y),
        fZ(z),
        fW(w)
    {
    }

    constexpr CVector4D(const CVector4D& vec) :
        fX(vec.fX),
        fY(vec.fY),
        fZ(vec.fZ),
        fW(vec.fW)
    {
    }

    CVector4D& operator=(const CVector4D& vec)
    {
        fX = vec.fX;
        fY = vec.fY;
        fZ = vec.fZ;
        fW = vec.fW;
        return *this;
    }

    float DotProduct(CVector4D& other) const { return fX * other.fX + fY * other.fY + fZ * other.fZ; }

    float Length() const { return sqrt(fX * fX + fY * fY + fZ * fZ + fW * fW); }

    float LengthSquared() const { return (fX * fX) + (fY * fY) + (fZ * fZ) + (fW * fW); }

    void Normalize()
    {
        float fLength = Length();
        if (fLength > 0.0f)
        {
            fX /= fLength;
            fY /= fLength;
            fZ /= fLength;
            fW /= fLength;
        }
    }

    constexpr CVector4D operator*(float fRight) const { return CVector4D(fX * fRight, fY * fRight, fZ * fRight, fW * fRight); }

    constexpr CVector4D operator/(float fRight) const
    {
        float fRcpValue = 1 / fRight;
        return CVector4D(fX * fRcpValue, fY * fRcpValue, fZ * fRcpValue, fW * fRcpValue);
    }

    constexpr CVector4D operator+(const CVector4D& vecRight) const { return CVector4D(fX + vecRight.fX, fY + vecRight.fY, fZ + vecRight.fZ, fW + vecRight.fW); }

    constexpr CVector4D operator-(const CVector4D& vecRight) const { return CVector4D(fX - vecRight.fX, fY - vecRight.fY, fZ - vecRight.fZ, fW - vecRight.fW); }

    constexpr CVector4D operator*(const CVector4D& vecRight) const { return CVector4D(fX * vecRight.fX, fY * vecRight.fY, fZ * vecRight.fZ, fW * vecRight.fW); }

    constexpr CVector4D operator/(const CVector4D& vecRight) const { return CVector4D(fX / vecRight.fX, fY / vecRight.fY, fZ / vecRight.fZ, fW / vecRight.fW); }

    void operator+=(float fRight)
    {
        fX += fRight;
        fY += fRight;
        fZ += fRight;
        fW += fRight;
    }

    void operator+=(const CVector4D& vecRight)
    {
        fX += vecRight.fX;
        fY += vecRight.fY;
        fZ += vecRight.fZ;
        fW += vecRight.fW;
    }

    void operator-=(float fRight)
    {
        fX -= fRight;
        fY -= fRight;
        fZ -= fRight;
        fW -= fRight;
    }

    void operator-=(const CVector4D& vecRight)
    {
        fX -= vecRight.fX;
        fY -= vecRight.fY;
        fZ -= vecRight.fZ;
        fW -= vecRight.fW;
    }

    void operator*=(float fRight)
    {
        fX *= fRight;
        fY *= fRight;
        fZ *= fRight;
        fW *= fRight;
    }

    void operator*=(const CVector4D& vecRight)
    {
        fX *= vecRight.fX;
        fY *= vecRight.fY;
        fZ *= vecRight.fZ;
        fW *= vecRight.fW;
    }

    void operator/=(float fRight)
    {
        fX /= fRight;
        fY /= fRight;
        fZ /= fRight;
        fW /= fRight;
    }

    void operator/=(const CVector4D& vecRight)
    {
        fX /= vecRight.fX;
        fY /= vecRight.fY;
        fZ /= vecRight.fZ;
        fW /= vecRight.fW;
    }

    bool operator==(const CVector4D& param) const
    {
        return ((fabs(fX - param.fX) < FLOAT_EPSILON) && (fabs(fY - param.fY) < FLOAT_EPSILON) && (fabs(fZ - param.fZ) < FLOAT_EPSILON) &&
                (fabs(fW - param.fW) < FLOAT_EPSILON));
    }

    bool operator!=(const CVector4D& param) const
    {
        return ((fabs(fX - param.fX) >= FLOAT_EPSILON) || (fabs(fY - param.fY) >= FLOAT_EPSILON) || (fabs(fZ - param.fZ) >= FLOAT_EPSILON) ||
                (fabs(fW - param.fW) >= FLOAT_EPSILON));
    }

    float fX;
    float fY;
    float fZ;
    float fW;
};
