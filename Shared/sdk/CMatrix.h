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

    CMatrix Inverse ( void ) const
    {
        float fDeterminant = vRight.fX * ( vFront.fY * vUp.fZ    - vUp.fY    * vFront.fZ ) -
                             vFront.fX * ( vRight.fY * vUp.fZ    - vUp.fY    * vRight.fZ ) +
                             vUp.fX    * ( vRight.fY * vFront.fZ - vFront.fY * vRight.fZ );

        if ( fDeterminant == 0.0f )
            return CMatrix ();

        float fRcp = 1.0f / fDeterminant;

        CMatrix matResult;
        matResult.vRight.fX =  fRcp * ( vFront.fY * vUp.fZ    - vUp.fY    * vFront.fZ );
        matResult.vRight.fY = -fRcp * ( vRight.fY * vUp.fZ    - vUp.fY    * vRight.fZ );
        matResult.vRight.fZ =  fRcp * ( vRight.fY * vFront.fZ - vFront.fY * vRight.fZ );

        matResult.vFront.fX = -fRcp * ( vFront.fX * vUp.fZ    - vUp.fX    * vFront.fZ );
        matResult.vFront.fY =  fRcp * ( vRight.fX * vUp.fZ    - vUp.fX    * vRight.fZ );
        matResult.vFront.fZ = -fRcp * ( vRight.fX * vFront.fZ - vFront.fX * vRight.fZ );

        matResult.vUp.fX    =  fRcp * ( vFront.fX * vUp.fY    - vUp.fX    * vFront.fY );
        matResult.vUp.fY    = -fRcp * ( vRight.fX * vUp.fY    - vUp.fX    * vRight.fY );
        matResult.vUp.fZ    =  fRcp * ( vRight.fX * vFront.fY - vFront.fX * vRight.fY );

        matResult.vPos.fX   = -fRcp * ( vFront.fX * ( vUp.fY    * vPos.fZ   - vUp.fZ    * vPos.fY  ) -
                                        vUp.fX    * ( vFront.fY * vPos.fZ   - vFront.fZ * vPos.fY  ) +
                                        vPos.fX   * ( vFront.fY * vUp.fZ    - vFront.fZ * vUp.fY   ) );

        matResult.vPos.fY   =  fRcp * ( vRight.fX * ( vUp.fY    * vPos.fZ   - vUp.fZ    * vPos.fY  ) -
                                        vUp.fX    * ( vRight.fY * vPos.fZ   - vRight.fZ * vPos.fY  ) +
                                        vPos.fX   * ( vRight.fY * vUp.fZ    - vRight.fZ * vUp.fY   ) );

        matResult.vPos.fZ   = -fRcp * ( vRight.fX * ( vFront.fY * vPos.fZ   - vFront.fZ * vPos.fY   ) -
                                        vFront.fX * ( vRight.fY * vPos.fZ   - vRight.fZ * vPos.fY   ) +
                                        vPos.fX   * ( vRight.fY * vFront.fZ - vRight.fZ * vFront.fY ) );
        return matResult;
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
    CVector TransformVector ( const CVector& vec ) const
    {
        return CVector ( vec.fX * vRight.fX + vec.fY * vFront.fX + vec.fZ * vUp.fX + vPos.fX,
            vec.fX * vRight.fY + vec.fY * vFront.fY + vec.fZ * vUp.fY + vPos.fY,
            vec.fX * vRight.fZ + vec.fY * vFront.fZ + vec.fZ * vUp.fZ + vPos.fZ );
    }

    CVector vRight;
    CVector vFront;
    CVector vUp;
    CVector vPos;
};

#endif
