/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CKeyGenSA.cpp
 *  PURPOSE:     Game key generator
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CKeyGenSA.h"

std::uint32_t CKeyGenSA::GetKey(const char* szString, int iLength)
{
    std::uint32_t uiReturn;
    DWORD        dwFunc = FUNC_CKeyGen_GetKey_len;
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

std::uint32_t CKeyGenSA::GetKey(const char* szString)
{
    std::uint32_t uiReturn;
    DWORD        dwFunc = FUNC_CKeyGen_GetKey;
    _asm
    {
        push    szString
        call    dwFunc
        add     esp, 0x4
        mov     uiReturn, eax
    }
    return uiReturn;
}

std::uint32_t CKeyGenSA::GetUppercaseKey(const char* szString)
{
    std::uint32_t uiReturn;
    DWORD        dwFunc = FUNC_CKeyGen_GetUppercaseKey;
    _asm
    {
        push    szString
        call    dwFunc
        add     esp, 0x4
        mov     uiReturn, eax
    }
    return uiReturn;
}

std::uint32_t CKeyGenSA::AppendStringToKey(std::uint32_t uiKey, const char* szString)
{
    std::uint32_t uiReturn;
    DWORD        dwFunc = FUNC_CKeyGen_AppendStringToKey;
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
