/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRopesSA.cpp
*  PURPOSE:     Rope entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

int CRopesSA::CreateRopeForSwatPed ( const CVector & vecPosition )
{
    int iReturn;
    DWORD dwFunc = FUNC_CRopes_CreateRopeForSwatPed;
    CVector * pvecPosition = const_cast < CVector * > ( &vecPosition );
    _asm
    {
        push    pvecPosition
        call    dwFunc
        add     esp, 0x4
        mov     iReturn, eax
    }
    return iReturn;
}