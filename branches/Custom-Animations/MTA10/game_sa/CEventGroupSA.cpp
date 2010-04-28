/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEventGroupSA.cpp
*  PURPOSE:     Base event group
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CEvent * CEventGroupSA::Add ( CEvent * pEvent, bool b_1 )
{
    CEventSAInterface * pReturnInterface = NULL;
    DWORD dwEventInterface = ( DWORD ) pEvent->GetInterface ();
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CEventGroup_Add;
    _asm
    {
        mov     ecx, dwThis
        push    b_1
        push    dwEventInterface
        call    dwFunc
        mov     pReturnInterface, eax
    }
    return NULL;
}

