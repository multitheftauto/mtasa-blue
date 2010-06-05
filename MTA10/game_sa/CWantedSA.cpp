/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWantedSA.cpp
*  PURPOSE:     Wanted level management
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CWantedSA::CWantedSA ( void )
{
    // TODO: Call GTA's new operator for CWanted. Lack of proper initialization might be causing crashes.

    internalInterface = new CWantedSAInterface;
    memset ( internalInterface, 0, sizeof ( CWantedSAInterface ) );

    m_bDontDelete = false;
}

CWantedSA::CWantedSA ( CWantedSAInterface* wantedInterface )
{
    internalInterface = wantedInterface;
    m_bDontDelete = true;
}

CWantedSA::~CWantedSA ( void )
{
    if ( !m_bDontDelete )
    {
        // TODO: Call GTA's delete operator for CWanted. Lack of proper destruction might be causing crashes.

        delete internalInterface;
    }
}

void CWantedSA::SetMaximumWantedLevel ( DWORD dwWantedLevel )
{
	DWORD dwFunc = FUNC_SetMaximumWantedLevel;
	_asm
	{
		push	dwWantedLevel
		call	dwFunc
		add		esp, 4
	}
}

void CWantedSA::SetWantedLevel ( DWORD dwWantedLevel )
{
	DWORD dwThis = (DWORD)this->GetInterface();
	DWORD dwFunc = FUNC_SetWantedLevel;
	_asm
	{
		mov		ecx, dwThis
		push	dwWantedLevel
		call	dwFunc
	}
}

void CWantedSA::SetWantedLevelNoDrop ( DWORD dwWantedLevel )
{
	DWORD dwThis = (DWORD)this->GetInterface();
	DWORD dwFunc = FUNC_SetWantedLevelNoDrop;
	_asm
	{
		mov		ecx, dwThis
		push	dwWantedLevel
		call	dwFunc
	}
}