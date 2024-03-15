/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CKeyGenSA.h
 *  PURPOSE:     Header file for game key generator class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CKeyGen.h>

#define FUNC_CKeyGen_GetKey_len         0x53ced0
#define FUNC_CKeyGen_GetKey             0x53cf00
#define FUNC_CKeyGen_GetUppercaseKey    0x53cf30
#define FUNC_CKeyGen_AppendStringToKey  0x53cf70

class CKeyGenSA : public CKeyGen
{
public:
    std::uint32_t GetKey(const char* szString, int iLength);
    std::uint32_t GetKey(const char* szString);
    std::uint32_t GetUppercaseKey(const char* szString);
    std::uint32_t AppendStringToKey(std::uint32_t uiKey, const char* szString);
};
