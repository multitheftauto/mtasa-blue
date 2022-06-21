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

unsigned int CKeyGenSA::GetKey(const char* szString, int iLength)
{
    // CKeyGen::GetKey
    return ((unsigned int(__cdecl*)(const char*, int))FUNC_CKeyGen_GetKey_len)(szString, iLength);
}

unsigned int CKeyGenSA::GetKey(const char* szString)
{
    // CKeyGen::GetKey
    return ((unsigned int(__cdecl*)(const char*))FUNC_CKeyGen_GetKey)(szString);
}

unsigned int CKeyGenSA::GetUppercaseKey(const char* szString)
{
    // CKeyGen::GetUppercaseKey
    return ((unsigned int(__cdecl*)(const char*))FUNC_CKeyGen_GetUppercaseKey)(szString);
}

unsigned int CKeyGenSA::AppendStringToKey(unsigned int uiKey, const char* szString)
{
    // CKeyGen::AppendStringToKey
    return ((unsigned int(__cdecl*)(unsigned int, const char*))FUNC_CKeyGen_AppendStringToKey)(uiKey, szString);
}
