/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CKeyGenSA.cpp
 *  PURPOSE:     Game key generator
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CKeyGenSA.h"

uint CKeyGenSA::GetKey(const char* szString, int iLength)
{
    uint  uiReturn = 0;
    DWORD dwFunc = 0x53CED0;
    _asm
    {
        push    iLength
        push    szString
        call    dwFunc
        add     esp, 0x8
        mov     uiReturn, eax
    }
    return uiReturn;
}

uint CKeyGenSA::GetKey(const char* szString)
{
    uint  uiReturn = 0;
    DWORD dwFunc = 0x53CF00;
    _asm
    {
        push    szString
        call    dwFunc
        add     esp, 0x4
        mov     uiReturn, eax
    }
    return uiReturn;
}

uint CKeyGenSA::GetUppercaseKey(const char* szString)
{
    uint  uiReturn = 0;
    DWORD dwFunc = 0x53CF30;
    _asm
    {
        push    szString
        call    dwFunc
        add     esp, 0x4
        mov     uiReturn, eax
    }
    return uiReturn;
}

uint CKeyGenSA::AppendStringToKey(uint uiKey, const char* szString)
{
    uint  uiReturn = 0;
    DWORD dwFunc = 0x53CF70;
    _asm
    {
        push    szString
        push    uiKey
        call    dwFunc
        add     esp, 0x8
        mov     uiReturn, eax
    }
    return uiReturn;
}
