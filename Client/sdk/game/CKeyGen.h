/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CKeyGen.h
 *  PURPOSE:     Game key generator interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CKeyGen
{
public:
    virtual std::uint32_t GetKey(const char* szString, int iLength) = 0;
    virtual std::uint32_t GetKey(const char* szString) = 0;
    virtual std::uint32_t GetUppercaseKey(const char* szString) = 0;
    virtual std::uint32_t AppendStringToKey(std::uint32_t uiKey, const char* szString) = 0;
};
