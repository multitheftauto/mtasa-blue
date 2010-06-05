/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/CMatrix_Pad.h
*  PURPOSE:		4x3 GTA padded matrix
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMATRIX_PAD_H
#define __CMATRIX_PAD_H

#include "CVector.h"
#include "CMatrix.h"

/**
 * CVector Structure used internally by GTA.
 */
class CMatrix_Padded
{
public:
	CVector vRight; // 0			RIGHT
	DWORD	 dwPadRoll; // 12
	CVector vFront; // 16	FOREWARDS
	DWORD	 dwPadDirection; // 28
	CVector vUp; // 32			UP
	DWORD	 dwPadWas; // 44	
	CVector vPos;  // 48		TRANSLATE
	DWORD	 dwPadPos; // 60

	CMatrix_Padded()
	{
		memset ( this, 0, sizeof ( CMatrix_Padded ) );
	}

    CMatrix_Padded ( const CMatrix& Matrix )
    {
        SetFromMatrix ( Matrix );
    }

    void ConvertToMatrix ( CMatrix& Matrix ) const
    {
        Matrix.vPos = vPos;
        Matrix.vFront = vFront;
        Matrix.vUp = vUp;
        Matrix.vRight = vRight;
    }

    void SetFromMatrix ( const CMatrix& Matrix )
    {
        vPos = Matrix.vPos;
        dwPadPos = 0;

        vFront = Matrix.vFront;
        dwPadDirection = 0;

        vUp = Matrix.vUp;
        dwPadWas = 0;

        vRight = Matrix.vRight;
        dwPadRoll = 0;
    }
};

#endif