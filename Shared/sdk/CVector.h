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

#ifdef WIN32
#include <windows.h>
#endif

#include <cmath>

#define FLOAT_EPSILON 0.0001f
#include "CVector4D.h"

/**
 * CVector Structure used to store a 3D vertex.
 */
class CVector
{
public:
    float fX, fY, fZ;

    CVector()
    {
        this->fX = 0;
        this->fY = 0;
        this->fZ = 0;
    };

    CVector(float fX, float fY, float fZ)
    {
        this->fX = fX;
        this->fY = fY;
        this->fZ = fZ;
    }

    float Normalize()
    {
        float t = sqrt(fX * fX + fY * fY + fZ * fZ);
        if (t > FLOAT_EPSILON)
        {
            float fRcpt = 1 / t;
            fX *= fRcpt;
            fY *= fRcpt;
            fZ *= fRcpt;
        }
        else
            t = 0;
        return t;
    }

    float Length() const { return sqrt((fX * fX) + (fY * fY) + (fZ * fZ)); }

    float LengthSquared() const { return (fX * fX) + (fY * fY) + (fZ * fZ); }

    float DotProduct(const CVector* param) const { return fX * param->fX + fY * param->fY + fZ * param->fZ; }

    void CrossProduct(const CVector* param)
    {
        float _fX = fX, _fY = fY, _fZ = fZ;
        fX = _fY * param->fZ - param->fY * _fZ;
        fY = _fZ * param->fX - param->fZ * _fX;
        fZ = _fX * param->fY - param->fX * _fY;
    }

    // Convert (direction) to rotation
    CVector ToRotation() const
    {
        CVector vecRotation;
        vecRotation.fZ = atan2(fY, fX);
        CVector vecTemp(sqrt(fX * fX + fY * fY), fZ, 0);
        vecTemp.Normalize();
        vecRotation.fY = atan2(vecTemp.fX, vecTemp.fY) - PI / 2;
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

    CVector Clone() const
    {
        CVector vecResult;
        vecResult.fX = fX;
        vecResult.fY = fY;
        vecResult.fZ = fZ;
        return vecResult;
    }

    // Intersections code based on https://github.com/juj/MathGeoLib/blob/master/src/Geometry/Plane.h
    bool IntesectsLinePlane(const CVector& vecRay, const CVector& vecNormal, const CVector& vecPosition, float* fOutDist) const
    {
        bool bIntersects = false;

        float fDenom = vecNormal.DotProduct(&vecRay);
        if (fabs(fDenom) > 1e-4f)
        {
            *fOutDist = (vecPosition.Length() - vecNormal.DotProduct(this)) / fDenom;
            return true;
        }

        if (fDenom != 0.0f)
        {
            *fOutDist = (vecPosition.Length() - vecNormal.DotProduct(this)) / fDenom;
            return fabs(*fOutDist) < 1e-4f;
        }

        *fOutDist = 0.0f;
        return fabs(vecNormal.DotProduct(this) - vecPosition.Length()) < 1e-3f;;
    }

    bool IntersectsSegmentPlane(const CVector& vecSegment, const CVector& vecNormal, const CVector& vecPosition, CVector* outVec) const
    {
        float fDist;
        CVector vecRay = vecSegment;
        vecRay.Normalize();
        bool bIntersects = IntesectsLinePlane(vecRay, vecNormal, vecPosition, &fDist);
        const float fSegLength = vecSegment.Length();
        
        *outVec = *this + vecRay * fDist;
        return bIntersects && fDist >= 0 && (fDist <= fSegLength);
    }

    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    bool IntersectsSegmentTriangle(const CVector& vecSegment, const CVector& vecVert1, const CVector& vecVert2, const CVector& vecVert3, CVector* outVec) const
    {
        const float fEpsilon = 1e-6f;

        CVector vecEdge1, vecEdge2, h, s;
        float a, f, u, v;

        CVector vecRay = vecSegment;
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
        if (t > fEpsilon && t <= vecSegment.Length())
        {
            *outVec = *this + vecRay * t;
            return true;
        }
        return false;
    }

    CVector operator+(const CVector& vecRight) const { return CVector(fX + vecRight.fX, fY + vecRight.fY, fZ + vecRight.fZ); }

    CVector operator-(const CVector& vecRight) const { return CVector(fX - vecRight.fX, fY - vecRight.fY, fZ - vecRight.fZ); }

    CVector operator*(const CVector& vecRight) const { return CVector(fX * vecRight.fX, fY * vecRight.fY, fZ * vecRight.fZ); }

    CVector operator*(float fRight) const { return CVector(fX * fRight, fY * fRight, fZ * fRight); }

    CVector operator/(const CVector& vecRight) const { return CVector(fX / vecRight.fX, fY / vecRight.fY, fZ / vecRight.fZ); }

    CVector operator/(float fRight) const
    {
        float fRcpValue = 1 / fRight;
        return CVector(fX * fRcpValue, fY * fRcpValue, fZ * fRcpValue);
    }

    CVector operator-() const { return CVector(-fX, -fY, -fZ); }

    void operator+=(float fRight)
    {
        fX += fRight;
        fY += fRight;
        fZ += fRight;
    }

    void operator+=(const CVector& vecRight)
    {
        fX += vecRight.fX;
        fY += vecRight.fY;
        fZ += vecRight.fZ;
    }

    void operator-=(float fRight)
    {
        fX -= fRight;
        fY -= fRight;
        fZ -= fRight;
    }

    void operator-=(const CVector& vecRight)
    {
        fX -= vecRight.fX;
        fY -= vecRight.fY;
        fZ -= vecRight.fZ;
    }

    void operator*=(float fRight)
    {
        fX *= fRight;
        fY *= fRight;
        fZ *= fRight;
    }

    void operator*=(const CVector& vecRight)
    {
        fX *= vecRight.fX;
        fY *= vecRight.fY;
        fZ *= vecRight.fZ;
    }

    void operator/=(float fRight)
    {
        float fRcpValue = 1 / fRight;
        fX *= fRcpValue;
        fY *= fRcpValue;
        fZ *= fRcpValue;
    }

    void operator/=(const CVector& vecRight)
    {
        fX /= vecRight.fX;
        fY /= vecRight.fY;
        fZ /= vecRight.fZ;
    }

    bool operator==(const CVector& param) const
    {
        return ((fabs(fX - param.fX) < FLOAT_EPSILON) && (fabs(fY - param.fY) < FLOAT_EPSILON) && (fabs(fZ - param.fZ) < FLOAT_EPSILON));
    }

    bool operator!=(const CVector& param) const
    {
        return ((fabs(fX - param.fX) >= FLOAT_EPSILON) || (fabs(fY - param.fY) >= FLOAT_EPSILON) || (fabs(fZ - param.fZ) >= FLOAT_EPSILON));
    }

    CVector& operator=(const CVector4D& vec)
    {
        fX = vec.fX;
        fY = vec.fY;
        fZ = vec.fZ;
        return *this;
    }
};
