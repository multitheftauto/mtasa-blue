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
	CVector vRoll;
	CVector vDirection;
	CVector vWas;
	CVector vPos;
};

#endif