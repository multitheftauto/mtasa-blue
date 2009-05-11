/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CColModelSA.cpp
*  PURPOSE:		Collision model entity
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*               arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CColModelSA::CColModelSA ( void )
{
    DWORD pInterface = (DWORD)&m_Interface;
    DWORD CColModel_constructor = FUNC_CColModel_constructor;
    _asm
    {
        mov ecx, pInterface
        call CColModel_constructor
    }
}

CColModelSA::~CColModelSA ( void )
{
    DWORD pInterface = (DWORD)&m_Interface;
    DWORD CColModel_destructor = FUNC_CColModel_destructor;
    _asm
    {
        mov ecx, pInterface
        call CColModel_destructor
    }
}
