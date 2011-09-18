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

#ifndef __CMatrix_H
#define __CMatrix_H

#include "CVector.h"

/**
 * Contains full positional data for a point
 */
class CMatrix
{
public:
    CMatrix ( )
    {
        // Load the identity matrix
        vRight = CVector ( 1.0f, 0.0f, 0.0f );
        vFront = CVector ( 0.0f, 1.0f, 0.0f );
        vUp    = CVector ( 0.0f, 0.0f, 1.0f );
        vPos   = CVector ( 0.0f, 0.0f, 0.0f );
    }

    CMatrix operator+ ( const CMatrix& other ) const
    {
        CMatrix matResult;
        matResult.vRight = vRight + other.vRight;
        matResult.vFront = vFront + other.vFront;
        matResult.vUp    = vUp    + other.vUp;
        matResult.vPos   = vPos   + other.vPos;
        return matResult;
    }

    CMatrix operator- ( const CMatrix& other ) const
    {
        CMatrix matResult;
        matResult.vRight = vRight - other.vRight;
        matResult.vFront = vFront - other.vFront;
        matResult.vUp    = vUp    - other.vUp;
        matResult.vPos   = vPos   - other.vPos;
        return matResult;
    }

    CMatrix operator* ( const CMatrix& other ) const
    {
        CMatrix matResult;
        matResult.vRight = (*this) * other.vRight;
        matResult.vFront = (*this) * other.vFront;
        matResult.vUp    = (*this) * other.vUp;
        matResult.vPos   = (*this) * other.vPos;
        return matResult;
    }

    CMatrix operator/ ( CMatrix other ) const
    {
        other.Invert ();
        return (*this) * other;
    }

    CVector operator* ( const CVector& vec ) const
    {
        return CVector (
            vRight.fX*vec.fX + vFront.fX*vec.fY + vUp.fX*vec.fZ,
            vRight.fY*vec.fX + vFront.fY*vec.fY + vUp.fY*vec.fZ,
            vRight.fZ*vec.fX + vFront.fZ*vec.fY + vUp.fZ*vec.fZ
        );
    }

    void Invert ()
    {
        // Transpose the rotation matrix and negate the position
        CVector vOldRight = vRight, vOldFront = vFront, vOldUp = vUp;
        vRight = CVector ( vOldRight.fX, vOldFront.fX, vOldUp.fX );
        vFront = CVector ( vOldRight.fY, vOldFront.fY, vOldUp.fY );
        vUp    = CVector ( vOldRight.fZ, vOldFront.fZ, vOldUp.fZ );
        vPos  *= -1.0f;
    }

    CVector vRight;
    CVector vFront;
    CVector vUp;
    CVector vPos;
};

#endif
