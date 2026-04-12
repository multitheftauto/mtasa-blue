/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CKeyGenSA.cpp
 *  PURPOSE:     Game key generator
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CKeyGenSA.h"

unsigned int CKeyGenSA::GetKey(const char* szString, int iLength)
{
    unsigned int uiReturn;
    DWORD        dwFunc = FUNC_CKeyGen_GetKey_len;
    // clang-format off
    using func_t = decltype(uiReturn) (__cdecl*)(decltype(szString), decltype(iLength));
uiReturn =     reinterpret_cast<func_t>(dwFunc)(szString, iLength);
    // clang-format on
    return uiReturn;
}

unsigned int CKeyGenSA::GetKey(const char* szString)
{
    unsigned int uiReturn;
    DWORD        dwFunc = FUNC_CKeyGen_GetKey;
    // clang-format off
    using func_t = decltype(uiReturn) (__cdecl*)(decltype(szString));
uiReturn =     reinterpret_cast<func_t>(dwFunc)(szString);
    // clang-format on
    return uiReturn;
}

unsigned int CKeyGenSA::GetUppercaseKey(const char* szString)
{
    unsigned int uiReturn;
    DWORD        dwFunc = FUNC_CKeyGen_GetUppercaseKey;
    // clang-format off
    using func_t = decltype(uiReturn) (__cdecl*)(decltype(szString));
uiReturn =     reinterpret_cast<func_t>(dwFunc)(szString);
    // clang-format on
    return uiReturn;
}

unsigned int CKeyGenSA::AppendStringToKey(unsigned int uiKey, const char* szString)
{
    unsigned int uiReturn;
    DWORD        dwFunc = FUNC_CKeyGen_AppendStringToKey;
    // clang-format off
    using func_t = decltype(uiReturn) (__cdecl*)(decltype(uiKey), decltype(szString));
uiReturn =     reinterpret_cast<func_t>(dwFunc)(uiKey, szString);
    // clang-format on
    return uiReturn;
}
