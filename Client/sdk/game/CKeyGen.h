/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/sdk/game/CKeyGen.h
 *  PURPOSE:     Game key generator interface
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CKeyGen
{
public:
    virtual uint GetKey(const char* szString, int iLength) = 0;
    virtual uint GetKey(const char* szString) = 0;
    virtual uint GetUppercaseKey(const char* szString) = 0;
    virtual uint AppendStringToKey(uint uiKey, const char* szString) = 0;
};
