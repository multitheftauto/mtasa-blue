/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CFontSA.cpp
*  PURPOSE:     Font handling and printing
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CFontSA::PrintString ( float fX, float fY, char * szString )
{
    DWORD dwFunc = FUNC_PrintString;
    _asm
    {
        push    szString
        push    fY
        push    fX
        call    dwFunc
        add     esp, 12
    }
}
