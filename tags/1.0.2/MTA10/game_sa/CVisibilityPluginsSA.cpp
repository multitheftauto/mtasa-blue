/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVisibilityPluginsSA.cpp
*  PURPOSE:     RenderWare entity visibility plugin
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CVisibilityPluginsSA::SetClumpAlpha ( RpClump * pClump, int iAlpha )
{
    DWORD dwFunc = FUNC_CVisiblityPlugins_SetClumpAlpha;
    _asm
    {
        push    iAlpha
        push    pClump
        call    dwFunc
        add     esp, 0x8
    }
}