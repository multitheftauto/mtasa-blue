/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CColModelSA.cpp
*  PURPOSE:		Collision model entity
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CColModelSA::CColModelSA (CColModelSAInterface * objectInterface)
{
	this->internalInterface = objectInterface;
}

CColModelSA::CColModelSA ( void )
{
	DWORD CColModel_operator_new = FUNC_CColModel_operator_new;
	DWORD CColModel_constructor = FUNC_CColModel_constructor;
	DWORD dwColModelPtr = NULL;

	_asm
	{
		push	SIZEOF_CColModel
		call	CColModel_operator_new
		add		esp, 4
		mov		dwColModelPtr, eax
	}

	if ( dwColModelPtr )
	{
		_asm
		{
			mov		ecx, dwColModelPtr
			call	CColModel_constructor
		}
	}

	this->internalInterface = (CColModelSAInterface*)dwColModelPtr;
}

CColModelSA::~CColModelSA ( void )
{
	__asm int 3;
	// not implemented
}
