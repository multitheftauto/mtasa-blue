/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/CMatrix.h
 *  PURPOSE:     4x3 GTA matrix class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CVector.h"
#include <array>

/**
 * Contains full positional data for a point
 */

class CMatrix
{
public:
    CMatrix()
    {
        // Load the identity matrix
        vRight = CVector(1.0f, 0.0f, 0.0f);
        vFront = CVector(0.0f, 1.0f, 0.0f);
        vUp = CVector(0.0f, 0.0f, 1.0f);
        vPos = CVector(0.0f, 0.0f, 0.0f);
    }

    CMatrix(const CVector& vecPosition)
    {
        vRight = CVector(1.0f, 0.0f, 0.0f);
        vFront = CVector(0.0f, 1.0f, 0.0f);
        vUp = CVector(0.0f, 0.0f, 1.0f);
        SetPosition(vecPosition);
    }

    CMatrix(const CVector& vecPosition, const CVector& vecRotation)
    {
        // Initialize as identity, so rotation isn't broken by no scale set.
        vRight = CVector(1.0f, 0.0f, 0.0f);
        vFront = CVector(0.0f, 1.0f, 0.0f);
        vUp = CVector(0.0f, 0.0f, 1.0f);
        SetRotation(vecRotation);
        SetPosition(vecPosition);
    }

    CMatrix(const CVector& vecPosition, const CVector& vecRotation, const CVector& vecScale)
    {
        // Initialize with scale set
        vRight = CVector(vecScale.fX, 0.0f, 0.0f);
        vFront = CVector(0.0f, vecScale.fY, 0.0f);
        vUp = CVector(0.0f, 0.0f, vecScale.fZ);
        SetRotation(vecRotation);
        SetPosition(vecPosition);
    }

    CMatrix operator+(const CMatrix& other) const
    {
        CMatrix matResult;
        matResult.vRight = vRight + other.vRight;
        matResult.vFront = vFront + other.vFront;
        matResult.vUp = vUp + other.vUp;
        matResult.vPos = vPos + other.vPos;
        return matResult;
    }

    CMatrix operator-(const CMatrix& other) const
    {
        CMatrix matResult;
        matResult.vRight = vRight - other.vRight;
        matResult.vFront = vFront - other.vFront;
        matResult.vUp = vUp - other.vUp;
        matResult.vPos = vPos - other.vPos;
        return matResult;
    }

    CMatrix operator*(const CMatrix& other) const
    {
        CMatrix matResult;
        matResult.vRight.fX = vRight.fX * other.vRight.fX + vRight.fY * other.vFront.fX + vRight.fZ * other.vUp.fX;
        matResult.vRight.fY = vRight.fX * other.vRight.fY + vRight.fY * other.vFront.fY + vRight.fZ * other.vUp.fY;
        matResult.vRight.fZ = vRight.fX * other.vRight.fZ + vRight.fY * other.vFront.fZ + vRight.fZ * other.vUp.fZ;

        matResult.vFront.fX = vFront.fX * other.vRight.fX + vFront.fY * other.vFront.fX + vFront.fZ * other.vUp.fX;
        matResult.vFront.fY = vFront.fX * other.vRight.fY + vFront.fY * other.vFront.fY + vFront.fZ * other.vUp.fY;
        matResult.vFront.fZ = vFront.fX * other.vRight.fZ + vFront.fY * other.vFront.fZ + vFront.fZ * other.vUp.fZ;

        matResult.vUp.fX = vUp.fX * other.vRight.fX + vUp.fY * other.vFront.fX + vUp.fZ * other.vUp.fX;
        matResult.vUp.fY = vUp.fX * other.vRight.fY + vUp.fY * other.vFront.fY + vUp.fZ * other.vUp.fY;
        matResult.vUp.fZ = vUp.fX * other.vRight.fZ + vUp.fY * other.vFront.fZ + vUp.fZ * other.vUp.fZ;

        matResult.vPos.fX = vPos.fX * other.vRight.fX + vPos.fY * other.vFront.fX + vPos.fZ * other.vUp.fX + other.vPos.fX;
        matResult.vPos.fY = vPos.fX * other.vRight.fY + vPos.fY * other.vFront.fY + vPos.fZ * other.vUp.fY + other.vPos.fY;
        matResult.vPos.fZ = vPos.fX * other.vRight.fZ + vPos.fY * other.vFront.fZ + vPos.fZ * other.vUp.fZ + other.vPos.fZ;

        return matResult;
    }

    CMatrix operator/(CMatrix other) const
    {
        other.Invert();
        return (*this) * other;
    }

    CVector operator*(const CVector& vec) const
    {
        return CVector(vRight.fX * vec.fX + vFront.fX * vec.fY + vUp.fX * vec.fZ, vRight.fY * vec.fX + vFront.fY * vec.fY + vUp.fY * vec.fZ,
                       vRight.fZ * vec.fX + vFront.fZ * vec.fY + vUp.fZ * vec.fZ);
    }

    CMatrix Inverse() const
    {
        float fDeterminant = vRight.fX * (vFront.fY * vUp.fZ - vUp.fY * vFront.fZ) - vFront.fX * (vRight.fY * vUp.fZ - vUp.fY * vRight.fZ) +
                             vUp.fX * (vRight.fY * vFront.fZ - vFront.fY * vRight.fZ);

        if (fDeterminant == 0.0f)
            return CMatrix();

        float fRcp = 1.0f / fDeterminant;

        CMatrix matResult;
        matResult.vRight.fX = fRcp * (vFront.fY * vUp.fZ - vUp.fY * vFront.fZ);
        matResult.vRight.fY = -fRcp * (vRight.fY * vUp.fZ - vUp.fY * vRight.fZ);
        matResult.vRight.fZ = fRcp * (vRight.fY * vFront.fZ - vFront.fY * vRight.fZ);

        matResult.vFront.fX = -fRcp * (vFront.fX * vUp.fZ - vUp.fX * vFront.fZ);
        matResult.vFront.fY = fRcp * (vRight.fX * vUp.fZ - vUp.fX * vRight.fZ);
        matResult.vFront.fZ = -fRcp * (vRight.fX * vFront.fZ - vFront.fX * vRight.fZ);

        matResult.vUp.fX = fRcp * (vFront.fX * vUp.fY - vUp.fX * vFront.fY);
        matResult.vUp.fY = -fRcp * (vRight.fX * vUp.fY - vUp.fX * vRight.fY);
        matResult.vUp.fZ = fRcp * (vRight.fX * vFront.fY - vFront.fX * vRight.fY);

        matResult.vPos.fX = -fRcp * (vFront.fX * (vUp.fY * vPos.fZ - vUp.fZ * vPos.fY) - vUp.fX * (vFront.fY * vPos.fZ - vFront.fZ * vPos.fY) +
                                     vPos.fX * (vFront.fY * vUp.fZ - vFront.fZ * vUp.fY));

        matResult.vPos.fY = fRcp * (vRight.fX * (vUp.fY * vPos.fZ - vUp.fZ * vPos.fY) - vUp.fX * (vRight.fY * vPos.fZ - vRight.fZ * vPos.fY) +
                                    vPos.fX * (vRight.fY * vUp.fZ - vRight.fZ * vUp.fY));

        matResult.vPos.fZ = -fRcp * (vRight.fX * (vFront.fY * vPos.fZ - vFront.fZ * vPos.fY) - vFront.fX * (vRight.fY * vPos.fZ - vRight.fZ * vPos.fY) +
                                     vPos.fX * (vRight.fY * vFront.fZ - vRight.fZ * vFront.fY));
        return matResult;
    }

    CMatrix Clone() const
    {
        CMatrix matResult;
        matResult.vRight = vRight.Clone();
        matResult.vFront = vFront.Clone();
        matResult.vUp = vUp.Clone();
        matResult.vPos = vPos.Clone();

        return matResult;
    }

    void Invert()
    {
        // Transpose the rotation matrix and negate the position
        CVector vOldRight = vRight, vOldFront = vFront, vOldUp = vUp;
        vRight = CVector(vOldRight.fX, vOldFront.fX, vOldUp.fX);
        vFront = CVector(vOldRight.fY, vOldFront.fY, vOldUp.fY);
        vUp = CVector(vOldRight.fZ, vOldFront.fZ, vOldUp.fZ);
        vPos *= -1.0f;
    }
    CVector TransformVector(const CVector& vec) const
    {
        return CVector(vec.fX * vRight.fX + vec.fY * vFront.fX + vec.fZ * vUp.fX + vPos.fX, vec.fX * vRight.fY + vec.fY * vFront.fY + vec.fZ * vUp.fY + vPos.fY,
                       vec.fX * vRight.fZ + vec.fY * vFront.fZ + vec.fZ * vUp.fZ + vPos.fZ);
    }

    //
    // Ensure matrix component axes are normalized and orthogonal to each other.
    // Axis A direction is preserved, B is adjusted, the other is overwritten.
    //
    void OrthoNormalize(const uint A, const uint B)
    {
        // Deduce 3rd axis index
        const uint C = 3 - A - B;

        // Ensure indices are in range and unique
        assert(A < 3 && B < 3 && C < 3 && A != B && C != A && C != B);

        CVector& vPriAxis = GetAxis(A);
        CVector& vSecAxis = GetAxis(B);
        CVector& vLastAxis = GetAxis(C);

        if (vPriAxis.Normalize() == 0)
            vPriAxis = CVector(0, 0, 1);
        vSecAxis.Normalize();

        // Ensure vSecAxis is perpendicular to vPriAxis
        float fDot = vPriAxis.DotProduct(&vSecAxis);
        vSecAxis += vPriAxis * -fDot;
        if (vSecAxis.Normalize() == 0)
            vSecAxis = vPriAxis.GetOtherAxis();

        // Do correct crossproduct calculation for the other axis
        if (((C + 1) % 3) != A)
        {
            vLastAxis = vSecAxis;
            vLastAxis.CrossProduct(&vPriAxis);
        }
        else
        {
            vLastAxis = vPriAxis;
            vLastAxis.CrossProduct(&vSecAxis);
        }
    }

    CMatrix GetRotationMatrix() const
    {
        // Operate only on rotation, ignore scale.
        CMatrix matClone = Clone();
        CVector vecScale = GetScale();
        matClone.vRight /= vecScale.fX;
        matClone.vFront /= vecScale.fY;
        matClone.vUp /= vecScale.fZ;
        matClone.vPos = CVector(0, 0, 0);
        return matClone;
    }

    // Get matrix rotation as angles
    // Inverted to match MTAized rotations for vehicles and players (and objects on the server)
    // Should produce the same results as ( CVector(0,0,0) - ConvertToEulerAngles() )
    CVector GetRotation() const
    {
        // Operate only on rotation, ignore scale.
        CMatrix matRot = GetRotationMatrix();

        float fRotY = atan2(matRot.vRight.fZ, sqrtf(Square(matRot.vRight.fX) + Square(matRot.vRight.fY)));
        float fRotZ = atan2(matRot.vRight.fY, matRot.vRight.fX);

        float fSinZ = -sin(fRotZ);
        float fCosZ = cos(fRotZ);
        float fRotX = atan2(matRot.vUp.fX * fSinZ + matRot.vUp.fY * fCosZ, matRot.vFront.fX * fSinZ + matRot.vFront.fY * fCosZ);
        return CVector(-fRotX, -fRotY, fRotZ);
    }

    // Set matrix rotational part
    // Inverted to match MTAized rotations for vehicles and players (and objects on the server)
    // Should produce the same results as ( CVector(0,0,0) - ConvertFromEulerAngles() )
    void SetRotation(const CVector& vecRotation)
    {
        float fCosX = cos(-vecRotation.fX);
        float fCosY = cos(-vecRotation.fY);
        float fCosZ = cos(vecRotation.fZ);
        float fSinX = sin(-vecRotation.fX);
        float fSinY = sin(-vecRotation.fY);
        float fSinZ = sin(vecRotation.fZ);

        // Keep current scale even after rotation.
        CVector vecScale = GetScale();

        vRight.fX = fCosY * fCosZ;
        vRight.fY = fCosY * fSinZ;
        vRight.fZ = fSinY;
        vRight *= vecScale.fX;

        vFront.fX = fSinX * fSinY * fCosZ - fCosX * fSinZ;
        vFront.fY = fSinX * fSinY * fSinZ + fCosX * fCosZ;
        vFront.fZ = -fSinX * fCosY;
        vFront *= vecScale.fY;

        vUp.fX = -(fCosX * fSinY * fCosZ + fSinX * fSinZ);
        vUp.fY = fCosZ * fSinX - fCosX * fSinY * fSinZ;
        vUp.fZ = fCosX * fCosY;
        vUp *= vecScale.fZ;
    }

    // Get matrix translational part
    const CVector& GetPosition() const { return vPos; }

    // Set matrix translational part
    void SetPosition(const CVector& vecPosition) { vPos = vecPosition; }

    CVector GetScale() const
    {
        return CVector(vRight.Length(), vFront.Length(), vUp.Length());
    }

    void SetScale(const CVector& vecScale)
    {
        CMatrix matRot = GetRotationMatrix();
        vRight = matRot.vRight * vecScale.fX;
        vFront = matRot.vFront * vecScale.fY;
        vUp = matRot.vUp * vecScale.fZ;
    }
    //
    // Get reference to component axis by index
    //
    CVector& GetAxis(const uint uiIndex)
    {
        if (uiIndex == AXIS_UP)
            return vUp;
        if (uiIndex == AXIS_FRONT)
            return vFront;
        assert(uiIndex == AXIS_RIGHT);
        return vRight;
    }

    void GetBuffer(float* array)
    {
        array[0] = vRight.fX;
        array[1] = vRight.fY;
        array[2] = vRight.fZ;
        array[3] = 0.0f;

        array[4] = vFront.fX;
        array[5] = vFront.fY;
        array[6] = vFront.fZ;
        array[7] = 0.0f;

        array[8] = vUp.fX;
        array[9] = vUp.fY;
        array[10] = vUp.fZ;
        array[11] = 0.0f;

        array[12] = vPos.fX;
        array[13] = vPos.fY;
        array[14] = vPos.fZ;
        array[15] = 1.0f;
    }

    std::array<std::array<float, 4>, 4> To4x4Array() const noexcept
    {
        return {
            vRight.fX, vRight.fY, vRight.fZ, 0.0f,
            vFront.fX, vFront.fY, vFront.fZ, 0.0f,
            vUp.fX,    vUp.fY,    vUp.fZ,    0.0f,
            vPos.fX,   vPos.fY,   vPos.fZ,   1.0f
        };
    }

    enum EMatrixAxes
    {
        AXIS_RIGHT,
        AXIS_FRONT,
        AXIS_UP,
    };

    CVector vRight;
    CVector vFront;
    CVector vUp;
    CVector vPos;
};
