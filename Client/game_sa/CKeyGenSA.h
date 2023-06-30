/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CKeyGenSA.h
 *  PURPOSE:     Header file for game key generator class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CKeyGen.h>

class CKeyGenSA : public CKeyGen
{
public:
    uint GetKey(const char* szString, int iLength);
    uint GetKey(const char* szString);
    uint GetUppercaseKey(const char* szString);
    uint AppendStringToKey(uint uiKey, const char* szString);
};
