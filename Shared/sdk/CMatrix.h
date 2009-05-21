/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/CMatrix.h
*  PURPOSE:		4x3 GTA matrix class
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
        vRoll       = CVector ( 1.0f, 0.0f, 0.0f );
        vDirection  = CVector ( 0.0f, 1.0f, 0.0f );
        vWas        = CVector ( 0.0f, 0.0f, 1.0f );
        vPos        = CVector ( 0.0f, 0.0f, 0.0f );
    }

	CVector vRoll;
	CVector vDirection;
	CVector vWas;
	CVector vPos;
};

#endif