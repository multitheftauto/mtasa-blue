/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/CVector.h
 *  PURPOSE:     3D vector math implementation
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cmath>

#include "CVector4D.h"

class CVector2D;

/**
 * CVector Structure used to store a 3D vertex.
 */
class CVector
{
private:
    static constexpr float FLOAT_EPSILON = 0.0001f;

public:
    float fX;
    float fY;
    float fZ;

    struct NoInit {};
    CVector(NoInit) noexcept {}

    constexpr CVector() noexcept : fX(0.0f), fY(0.0f), fZ(0.0f) {}
    
    constexpr explicit CVector(float x, float y = 0.0f, float z = 0.0f) noexcept : fX(x), fY(y), fZ(z) {}

    constexpr CVector(const CVector4D& vec) noexcept : fX(vec.fX), fY(vec.fY), fZ(vec.fZ) {}

    constexpr CVector& operator=(const CVector4D& vec) noexcept
    {
        fX = vec.fX;
        fY = vec.fY;
        fZ = vec.fZ;
        return *this;
    }

    constexpr CVector Clone() const { return *this; }

    // Normalize returns the normalized length of the vector.
    float Normalize()
    {
        const float t = Length();
        if (t > FLOAT_EPSILON)
        {
            fX /= t;
            fY /= t;
            fZ /= t;

            return t;
        }
        else
            return 0;
    }

    float Length() const { return sqrt((fX * fX) + (fY * fY) + (fZ * fZ)); }

    // LengthSquared returns Length() without sqrt applied (i.e. returns x*x* + y*y + z*z).
    // This can be useful if you only want to compare lengths.
    float LengthSquared() const { return (fX * fX) + (fY * fY) + (fZ * fZ); }

    float DotProduct(const CVector* param) const { return fX * param->fX + fY * param->fY + fZ * param->fZ; }

    void CrossProduct(const CVector* param)
    {
        const float _fX = fX, _fY = fY, _fZ = fZ;
        fX = _fY * param->fZ - param->fY * _fZ;
        fY = _fZ * param->fX - param->fZ * _fX;
        fZ = _fX * param->fY - param->fX * _fY;
    }

    // Convert (direction) to rotation
    CVector ToRotation() const
    {
        CVector vecRotation;
        vecRotation.fZ = atan2(fY, fX);
        CVector vecTemp(std::hypotf(fX, fY), fZ, 0);
        vecTemp.Normalize();
        constexpr float pi = 3.14159265358979323846f;
        vecRotation.fY = atan2(vecTemp.fX, vecTemp.fY) - pi / 2;
        return vecRotation;
    }

    // Return a perpendicular direction
    CVector GetOtherAxis() const
    {
        CVector vecResult;
        if (std::abs(fX) > std::abs(fY))
            vecResult = CVector(fZ, 0, -fX);
        else
            vecResult = CVector(0, -fZ, fY);

        vecResult.Normalize();
        return vecResult;
    }

    // Intersections code based on https://github.com/juj/MathGeoLib/blob/master/src/Geometry/Plane.h
    bool IntesectsLinePlane(const CVector& vecRay, const CVector& vecNormal, const CVector& vecPosition, float* fOutDist) const noexcept
    {
        const float fDenom = vecNormal.DotProduct(&vecRay);

        if (fabs(fDenom) > 1e-4f)
        {
            *fOutDist = (vecNormal.DotProduct(&vecPosition) - vecNormal.DotProduct(this)) / fDenom;
            return true;
        }

        if (fDenom != 0.0f)
        {
            *fOutDist = (vecNormal.DotProduct(&vecPosition) - vecNormal.DotProduct(this)) / fDenom;
            return fabs(*fOutDist) < 1e-4f;
        }

        *fOutDist = 0.0f;
        return fabs(vecNormal.DotProduct(this) - vecNormal.DotProduct(&vecPosition)) < 1e-3f;
        ;
    }

    bool IntersectsSegmentPlane(const CVector& vecSegment, const CVector& vecNormal, const CVector& vecPosition, CVector* outVec) const noexcept
    {
        float   fDist;
        CVector vecRay = vecSegment;
        vecRay.Normalize();
        bool bIntersects = IntesectsLinePlane(vecRay, vecNormal, vecPosition, &fDist);

        const float fSegLength = vecSegment.Length();

        *outVec = *this + vecRay * fDist;
        return bIntersects && fDist >= 0 && (fDist <= fSegLength);
    }

    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    bool IntersectsSegmentTriangle(const CVector& dir, const CVector& vecVert1, const CVector& vecVert2, const CVector& vecVert3,
                                   CVector* outVec, CVector* outHitBary = nullptr) const noexcept
    {
        constexpr float fEpsilon = 1e-6f;

        CVector vecEdge1, vecEdge2, h, s;
        float   a, f, u, v;

        CVector vecRay = dir;
        vecRay.Normalize();
        h = vecRay;

        vecEdge1 = vecVert2 - vecVert1;
        vecEdge2 = vecVert3 - vecVert1;

        h.CrossProduct(&vecEdge2);
        a = vecEdge1.DotProduct(&h);

        if (a > -fEpsilon && a < fEpsilon)
        {
            return false;
        }

        f = 1.0f / a;
        s = *this - vecVert1;
        u = f * s.DotProduct(&h);
        if (u < 0.0f || u > 1.0f)
        {
            return false;
        }

        CVector sCrossE1 = s;
        sCrossE1.CrossProduct(&vecEdge1);
        v = f * vecRay.DotProduct(&sCrossE1);
        if (v < 0.0f || u + v > 1.0f)
        {
            return false;
        }

        float t = f * vecEdge2.DotProduct(&sCrossE1);
        if (t > fEpsilon && t <= dir.Length())
        {
            *outVec = *this + vecRay * t;
            if (outHitBary) { // Calculate all barycentric coords if necessary
                *outHitBary = CVector( 1.f - u - v, u, v ); // For vertices A, B, C [I assume?]
            }
            return true;
        }

        return false;
    }

    constexpr CVector operator+(const CVector& vecRight) const noexcept { return CVector(fX + vecRight.fX, fY + vecRight.fY, fZ + vecRight.fZ); }

    constexpr CVector operator-(const CVector& vecRight) const noexcept { return CVector(fX - vecRight.fX, fY - vecRight.fY, fZ - vecRight.fZ); }

    constexpr CVector operator-() const noexcept { return CVector(-fX, -fY, -fZ); }

    constexpr CVector operator*(const CVector& vecRight) const noexcept { return CVector(fX * vecRight.fX, fY * vecRight.fY, fZ * vecRight.fZ); }

    constexpr CVector operator*(const float fRight) const noexcept { return CVector(fX * fRight, fY * fRight, fZ * fRight); }

    constexpr CVector operator/(const CVector& vecRight) const noexcept { return CVector(fX / vecRight.fX, fY / vecRight.fY, fZ / vecRight.fZ); }

    constexpr CVector operator/(const float fRight) const noexcept { return CVector(fX / fRight, fY / fRight, fZ / fRight); }

    constexpr void operator+=(const float fRight) noexcept
    {
        fX += fRight;
        fY += fRight;
        fZ += fRight;
    }

    constexpr void operator+=(const CVector& vecRight) noexcept
    {
        fX += vecRight.fX;
        fY += vecRight.fY;
        fZ += vecRight.fZ;
    }

    constexpr void operator-=(const float fRight) noexcept
    {
        fX -= fRight;
        fY -= fRight;
        fZ -= fRight;
    }

    constexpr void operator-=(const CVector& vecRight) noexcept
    {
        fX -= vecRight.fX;
        fY -= vecRight.fY;
        fZ -= vecRight.fZ;
    }

    constexpr void operator*=(const float fRight) noexcept
    {
        fX *= fRight;
        fY *= fRight;
        fZ *= fRight;
    }

    constexpr void operator*=(const CVector& vecRight) noexcept
    {
        fX *= vecRight.fX;
        fY *= vecRight.fY;
        fZ *= vecRight.fZ;
    }

    constexpr void operator/=(const float fRight) noexcept
    {
        fX /= fRight;
        fY /= fRight;
        fZ /= fRight;
    }

    constexpr void operator/=(const CVector& vecRight) noexcept
    {
        fX /= vecRight.fX;
        fY /= vecRight.fY;
        fZ /= vecRight.fZ;
    }

    bool operator==(const CVector& param) const noexcept
    {
        return ((fabs(fX - param.fX) < FLOAT_EPSILON) && (fabs(fY - param.fY) < FLOAT_EPSILON) && (fabs(fZ - param.fZ) < FLOAT_EPSILON));
    }

    bool operator!=(const CVector& param) const noexcept { return !(*this == param); }

    float operator[](size_t i) const noexcept { return ((float*)this)[i]; }
};
