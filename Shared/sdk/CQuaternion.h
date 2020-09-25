/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/C CQuaternion.h
 *  PURPOSE:     CQuaternion class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CVector.h"

// Debug only
#define MTA_SSE_MATH_ENABLED

#ifdef MTA_SSE_MATH_ENABLED
#include <emmintrin.h>
#endif

class CQuaternion
{
public:
    CQuaternion()
#ifndef MTA_SSE_MATH_ENABLED
        :fW(1.0f),
        fX(0.0f),
        fY(0.0f),
        fZ(0.0f)
#endif
    {
#ifdef MTA_SSE_MATH_ENABLED
        _mm_storeu_ps(&fW, _mm_set_ps(0.f, 0.f, 0.f, 1.f));
#endif
    }

    CQuaternion(const CQuaternion& quat)
#if defined(MTA_SSE_MATH_ENABLED)
    {
        _mm_storeu_ps(&fW, _mm_loadu_ps(&quat.fW));
    }
#else
        :fW(quat.fW),
        fX(quat.fX),
        fY(quat.fY),
        fZ(quat.fZ)
    {
    }
#endif

    CQuaternion(float w, float x, float y, float z) noexcept
#ifndef MTA_SSE_MATH_ENABLED
        :fW(w),
        fX(x),
        fY(y),
        fZ(z)
#endif
    {
#ifdef MTA_SSE_MATH_ENABLED
        _mm_storeu_ps(&fW, _mm_set_ps(z, y, x, w));
#endif
    }

    CQuaternion(const CVector4D& vecQuat) noexcept
#ifndef MTA_SSE_MATH_ENABLED
        :fW(vecQuat.fW),
        fX(vecQuat.fX),
        fY(vecQuat.fY),
        fZ(vecQuat.fZ)
#endif
    {
#ifdef MTA_SSE_MATH_ENABLED
        _mm_storeu_ps(&fW, _mm_set_ps(vecQuat.fZ, vecQuat.fY, vecQuat.fX, vecQuat.fW));
#endif
    }


#ifdef MTA_SSE_MATH_ENABLED
    explicit CQuaternion(__m128 wxyz) noexcept
    {
        _mm_storeu_ps(&fW, wxyz);
    }
#endif

    CQuaternion& operator =(const CQuaternion& quat) noexcept
    {
#if defined(MTA_SSE_MATH_ENABLED)
        _mm_storeu_ps(&fW, _mm_loadu_ps(&quat.fW));
#else
        fW = quat.fW;
        fX = quat.fX;
        fY = quat.fY;
        fZ = quat.fZ;
#endif
        return *this;
    }

    CQuaternion& operator +=(const CQuaternion& quat)
    {
#ifdef MTA_SSE_MATH_ENABLED
        _mm_storeu_ps(&fW, _mm_add_ps(_mm_loadu_ps(&fW), _mm_loadu_ps(&quat.fW)));
#else
        fW += quat.fW;
        fX += quat.fX;
        fY += quat.fY;
        fZ += quat.fZ;
#endif
        return *this;
    }

    CQuaternion& operator *=(float fScalar)
    {
#ifdef MTA_SSE_MATH_ENABLED
        _mm_storeu_ps(&fW, _mm_mul_ps(_mm_loadu_ps(&fW), _mm_set1_ps(fScalar)));
#else
        fW *= fScalar;
        fX *= fScalar;
        fY *= fScalar;
        fZ *= fScalar;
#endif
        return *this;
    }

    bool operator ==(const CQuaternion& quat) const
    {
#ifdef MTA_SSE_MATH_ENABLED
        __m128 c = _mm_cmpeq_ps(_mm_loadu_ps(&fW), _mm_loadu_ps(&quat.fW));
        c = _mm_and_ps(c, _mm_movehl_ps(c, c));
        c = _mm_and_ps(c, _mm_shuffle_ps(c, c, _MM_SHUFFLE(1, 1, 1, 1)));
        return _mm_cvtsi128_si32(_mm_castps_si128(c)) == -1;
#else
        return fW == quat.fW && fX == quat.fX && fY == quat.fY && fZ == quat.fZ;
#endif
    }

    bool operator !=(const CQuaternion& quat) const { return !(*this == quat); }

    CQuaternion operator *(float fScalar) const
    {
#ifdef MTA_SSE_MATH_ENABLED
        return CQuaternion(_mm_mul_ps(_mm_loadu_ps(&fW), _mm_set1_ps(fScalar)));
#else
        return CQuaternion(fW * fScalar, fX * fScalar, fY * fScalar, fZ * fScalar);
#endif
    }

    CQuaternion operator -() const
    {
#ifdef MTA_SSE_MATH_ENABLED
        return CQuaternion(_mm_xor_ps(_mm_loadu_ps(&fW), _mm_castsi128_ps(_mm_set1_epi32((int)0x80000000UL))));
#else
        return CQuaternion(-fW, -fX, -fY, -fZ);
#endif
    }

    CQuaternion operator +(const CQuaternion& quat) const
    {
#ifdef MTA_SSE_MATH_ENABLED
        return CQuaternion(_mm_add_ps(_mm_loadu_ps(&fW), _mm_loadu_ps(&quat.fW)));
#else
        return CQuaternion(fW + quat.fW, fX + quat.fX, fY + quat.fY, fZ + quat.fZ);
#endif
    }

    CQuaternion operator -(const CQuaternion& quat) const
    {
#ifdef MTA_SSE_MATH_ENABLED
        return CQuaternion(_mm_sub_ps(_mm_loadu_ps(&fW), _mm_loadu_ps(&quat.fW)));
#else
        return CQuaternion(fW - quat.fW, fX - quat.fX, fY - quat.fY, fZ - quat.fZ);
#endif
    }

    CQuaternion operator *(const CQuaternion& quat) const
    {
#ifdef MTA_SSE_MATH_ENABLED
        __m128 q1 = _mm_loadu_ps(&fW);
        __m128 q2 = _mm_loadu_ps(&quat.fW);
        q2 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(0, 3, 2, 1));
        const __m128 signy = _mm_castsi128_ps(_mm_set_epi32((int)0x80000000UL, (int)0x80000000UL, 0, 0));
        const __m128 signx = _mm_shuffle_ps(signy, signy, _MM_SHUFFLE(2, 0, 2, 0));
        const __m128 signz = _mm_shuffle_ps(signy, signy, _MM_SHUFFLE(3, 0, 0, 3));
        __m128 out = _mm_mul_ps(_mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 1, 1, 1)), _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(0, 1, 2, 3)));
        out = _mm_add_ps(_mm_mul_ps(_mm_xor_ps(signy, _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(2, 2, 2, 2))), _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(1, 0, 3, 2))), _mm_xor_ps(signx, out));
        out = _mm_add_ps(_mm_mul_ps(_mm_xor_ps(signz, _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 3, 3, 3))), _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(2, 3, 0, 1))), out);
        out = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(q1, q1, _MM_SHUFFLE(0, 0, 0, 0)), q2), out);
        return CQuaternion(_mm_shuffle_ps(out, out, _MM_SHUFFLE(2, 1, 0, 3)));
#else
        return CQuaternion(
            fW * quat.fW - fX * quat.fX - fY * quat.fY - fZ * quat.fZ,
            fW * quat.fX + fX * quat.fW + fY * quat.fZ - fZ * quat.fY,
            fW * quat.fY + fY * quat.fW + fZ * quat.fX - fX * quat.fZ,
            fW * quat.fZ + fZ * quat.fW + fX * quat.fY - fY * quat.fX
        );
#endif
    }

    CVector operator *(const CVector& vecRhs) const
    {
#ifdef MTA_SSE_MATH_ENABLED
        __m128 q = _mm_loadu_ps(&fW);
        q = _mm_shuffle_ps(q, q, _MM_SHUFFLE(0, 3, 2, 1));
        __m128 v = _mm_set_ps(0.f, vecRhs.fZ, vecRhs.fY, vecRhs.fX);
        const __m128 W = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 3, 3, 3));
        const __m128 a_yzx = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 x = _mm_mul_ps(q, _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 0, 2, 1)));
        __m128 qxv = _mm_sub_ps(x, _mm_mul_ps(a_yzx, v));
        __m128 Wv = _mm_mul_ps(W, v);
        __m128 s = _mm_add_ps(qxv, _mm_shuffle_ps(Wv, Wv, _MM_SHUFFLE(3, 1, 0, 2)));
        __m128 qs = _mm_mul_ps(q, s);
        __m128 y = _mm_shuffle_ps(qs, qs, _MM_SHUFFLE(3, 1, 0, 2));
        s = _mm_sub_ps(_mm_mul_ps(a_yzx, s), y);
        s = _mm_add_ps(s, s);
        s = _mm_add_ps(s, v);

        return CVector(
            _mm_cvtss_f32(s),
            _mm_cvtss_f32(_mm_shuffle_ps(s, s, _MM_SHUFFLE(1, 1, 1, 1))),
            _mm_cvtss_f32(_mm_movehl_ps(s, s)));
#else
        CVector vecQ(fX, fY, fZ);
        CVector vecCross1(vecQ);
        vecCross1.CrossProduct(&vecRhs);
        CVector vecCross2(vecQ);
        vecCross2.CrossProduct(&vecCross1);

        return vecRhs + (vecCross1 * fW + vecCross2) * 2.0f;
#endif
    }  

    float LengthSquared() const
    {
#ifdef MTA_SSE_MATH_ENABLED
        __m128 q = _mm_loadu_ps(&fW);
        __m128 n = _mm_mul_ps(q, q);
        n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(2, 3, 0, 1)));
        n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(0, 1, 2, 3)));
        return _mm_cvtss_f32(n);
#else
        return fW * fW + fX * fX + fY * fY + fZ * fZ;
#endif
    }

    void Normalize()
    {
#ifdef MTA_SSE_MATH_ENABLED
        __m128 q = _mm_loadu_ps(&fW);
        __m128 n = _mm_mul_ps(q, q);
        n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(2, 3, 0, 1)));
        n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(0, 1, 2, 3)));
        __m128 e = _mm_rsqrt_ps(n);
        __m128 e3 = _mm_mul_ps(_mm_mul_ps(e, e), e);
        __m128 half = _mm_set1_ps(0.5f);
        n = _mm_add_ps(e, _mm_mul_ps(half, _mm_sub_ps(e, _mm_mul_ps(n, e3))));
        _mm_storeu_ps(&fW, _mm_mul_ps(q, n));
#else
        const float fLenSquared = LengthSquared();
        if (std::abs(fLenSquared - 1.0f) > FLOAT_EPSILON && fLenSquared > 0.0f)
        {
            float fIvLen = 1.0f / sqrtf(fLenSquared);
            fW *= fIvLen;
            fX *= fIvLen;
            fY *= fIvLen;
            fZ *= fIvLen;
        }
#endif
    }

    CQuaternion GetNormalized() const
    {
#ifdef MTA_SSE_MATH_ENABLED
        __m128 q = _mm_loadu_ps(&fW);
        __m128 n = _mm_mul_ps(q, q);
        n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(2, 3, 0, 1)));
        n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(0, 1, 2, 3)));
        __m128 e = _mm_rsqrt_ps(n);
        __m128 e3 = _mm_mul_ps(_mm_mul_ps(e, e), e);
        __m128 half = _mm_set1_ps(0.5f);
        n = _mm_add_ps(e, _mm_mul_ps(half, _mm_sub_ps(e, _mm_mul_ps(n, e3))));
        return CQuaternion(_mm_mul_ps(q, n));
#else
        const float fLenSquared = LengthSquared();
        if (std::abs(fLenSquared - 1.0f) > FLOAT_EPSILON && fLenSquared > 0.0f)
        {
            float invLen = 1.0f / sqrtf(fLenSquared);
            return *this * invLen;
        }
   
        return *this;
#endif
    }

    float DotProduct(const CQuaternion& rhs) const
    {
#ifdef MTA_SSE_MATH_ENABLED
        __m128 q1 = _mm_loadu_ps(&fW);
        __m128 q2 = _mm_loadu_ps(&rhs.fW);
        __m128 n = _mm_mul_ps(q1, q2);
        n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(2, 3, 0, 1)));
        n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(0, 1, 2, 3)));
        return _mm_cvtss_f32(n);
#else
        return fW * rhs.fW + fX * rhs.fX + fY * rhs.fY + fZ * rhs.fZ;
#endif
    }

    CQuaternion Inverse() const
    {
#ifdef MTA_SSE_MATH_ENABLED
        __m128 q = _mm_loadu_ps(&fW);
        __m128 n = _mm_mul_ps(q, q);
        n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(2, 3, 0, 1)));
        n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(0, 1, 2, 3)));
        return CQuaternion(_mm_div_ps(_mm_xor_ps(q, _mm_castsi128_ps(_mm_set_epi32((int)0x80000000UL, (int)0x80000000UL, (int)0x80000000UL, 0))), n));
#else
        const float fLenSquared = LengthSquared();
        if (std::abs(fLenSquared - 1.0f) < FLOAT_EPSILON)
            return Conjugate();
        else if (fLenSquared >= FLOAT_EPSILON)
            return Conjugate() * (1.0f / fLenSquared);
        else
            return CQuaternion();
#endif
    }

    CQuaternion Conjugate() const
    {
#ifdef MTA_SSE_MATH_ENABLED
        __m128 q = _mm_loadu_ps(&fW);
        return CQuaternion(_mm_xor_ps(q, _mm_castsi128_ps(_mm_set_epi32((int)0x80000000UL, (int)0x80000000UL, (int)0x80000000UL, 0))));
#else
        return CQuaternion(fW, -fX, -fY, -fZ);
#endif
    }

    CQuaternion Slerp(const CQuaternion& quat, float fT) const
    {
        float fCosAngle = DotProduct(quat);
        float fSign = 1.0f;

        // Shortest path rotation
        if (fCosAngle < 0.0f)
        {
            fCosAngle = -fCosAngle;
            fSign = -1.0f;
        }

        const float fAngle = acosf(fCosAngle);
        const float fSinAngle = sinf(fAngle);
        float fT0, fT1;

        if (fSinAngle > 0.001f)
        {
            float fInvSinAngle = 1.0f / fSinAngle;
            fT0 = sinf((1.0f - fT) * fAngle) * fInvSinAngle;
            fT1 = sinf(fT * fAngle) * fInvSinAngle;
        }
        else
        {
            fT0 = 1.0f - fT;
            fT1 = fT;
        }

        return *this * fT0 + (quat * fSign) * fT1;
    }

    CQuaternion Nlerp(const CQuaternion& quat, float fT, bool bShortestPath) const
    {
        CQuaternion result;

        float fCos = DotProduct(quat);
        if (fCos < 0.0f && bShortestPath)
            result = (*this) + (((-quat) - (*this)) * fT);
        else
            result = (*this) + ((quat - (*this)) * fT);

        result.Normalize();
        return result;
    }

    void FromRotationMatrix(const CMatrix& matrix)
    {
        const float t = matrix.vRight.fX + matrix.vFront.fY + matrix.vUp.fZ;

        if (t > 0.0f)
        {
            float fInvS = 0.5f / sqrtf(1.0f + t);

            fX = (matrix.vFront.fZ - matrix.vUp.fY) * fInvS;
            fY = (matrix.vUp.fX - matrix.vRight.fZ) * fInvS;
            fZ = (matrix.vRight.fY - matrix.vFront.fX) * fInvS;
            fW = 0.25f / fInvS;
        }
        else
        {
            if (matrix.vRight.fX > matrix.vFront.fY && matrix.vRight.fX > matrix.vUp.fZ)
            {
                float fInvS = 0.5f / sqrtf(1.0f + matrix.vRight.fX - matrix.vFront.fY - matrix.vUp.fZ);

                fX = 0.25f / fInvS;
                fY = (matrix.vFront.fX + matrix.vRight.fY) * fInvS;
                fZ = (matrix.vRight.fZ + matrix.vUp.fX) * fInvS;
                fW = (matrix.vFront.fZ - matrix.vUp.fY) * fInvS;
            }
            else if (matrix.vFront.fY > matrix.vUp.fZ)
            {
                float fInvS = 0.5f / sqrtf(1.0f + matrix.vFront.fY - matrix.vRight.fX - matrix.vUp.fZ);

                fX = (matrix.vFront.fX + matrix.vRight.fY) * fInvS;
                fY = 0.25f / fInvS;
                fZ = (matrix.vUp.fY + matrix.vFront.fZ) * fInvS;
                fW = (matrix.vUp.fX - matrix.vRight.fZ) * fInvS;
            }
            else
            {
                float fInvS = 0.5f / sqrtf(1.0f + matrix.vUp.fZ - matrix.vRight.fX - matrix.vFront.fY);

                fX = (matrix.vUp.fX + matrix.vRight.fZ) * fInvS;
                fY = (matrix.vUp.fY + matrix.vFront.fZ) * fInvS;
                fZ = 0.25f / fInvS;
                fW = (matrix.vRight.fY - matrix.vFront.fX) * fInvS;
            }
        }
    }

    CMatrix GetRotationMatrix() const
    {
        CMatrix matrix;
        matrix.vRight = CVector(1.0f - 2.0f * fY * fY - 2.0f * fZ * fZ,
            2.0f * fX * fY - 2.0f * -fW * fZ,
            2.0f * fX * fZ + 2.0f * -fW * fY);
        matrix.vFront = CVector(2.0f * fX * fY + 2.0f * -fW * fZ,
            1.0f - 2.0f * fX * fX - 2.0f * fZ * fZ,
            2.0f * fY * fZ - 2.0f * -fW * fX);
        matrix.vUp = CVector(2.0f * fX * fZ - 2.0f * -fW * fY,
            2.0f * fY * fZ + 2.0f * -fW * fX,
            1.0f - 2.0f * fX * fX - 2.0f * fY * fY);

        return matrix;
    }

    void FromAngleAxis(float fAngle, const CVector& vecAxis)
    {
        CVector vecNormAxis(vecAxis);
        vecNormAxis.Normalize();

        fAngle *= PI / 360.0f;
        const float fSinAngle = sinf(fAngle);
        const float fCosAngle = cosf(fAngle);

        fW = fCosAngle;
        fX = vecNormAxis.fX * fSinAngle;
        fY = vecNormAxis.fY * fSinAngle;
        fZ = vecNormAxis.fZ * fSinAngle;
    }

    void FromRotationTo(const CVector& vecStart, const CVector& vecEnd)
    {
        CVector vecNormStart(vecStart);
        vecNormStart.Normalize();

        CVector vecNormEnd(vecEnd);
        vecNormEnd.Normalize();

        const float fD = vecNormStart.DotProduct(&vecNormEnd);

        if (fD > FLOAT_EPSILON - 1.0f)
        {
            CVector c(vecNormStart);
            c.CrossProduct(&vecNormEnd);
  
            float fS = sqrtf((1.0f + fD) * 2.0f);
            float fInvS = 1.0f / fS;

            fX = c.fX * fInvS;
            fY = c.fY * fInvS;
            fZ = c.fZ * fInvS;
            fW = 0.5f * fS;
        }
        else
        {
            CVector vecAxis(1.0f, 0.0f, 0.0f);
            vecAxis.CrossProduct(&vecNormStart);
      
            if (vecAxis.Length() < FLOAT_EPSILON)
            {
                vecAxis = CVector(0.0f, 0.0f, 1.0f);
                vecAxis.CrossProduct(&vecNormStart);
            }

            FromAngleAxis(180.f, vecAxis);
        }
    }

    float fW;
    float fX;
    float fY;
    float fZ;    
};
