/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CKeyGen.h
 *  PURPOSE:     Game key generator interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CKeyGen
{
public:
    virtual unsigned int GetKey(const char* szString, int iLength) = 0;
    virtual unsigned int GetKey(const char* szString) = 0;
    virtual unsigned int GetUppercaseKey(const char* szString) = 0;
    virtual unsigned int AppendStringToKey(unsigned int uiKey, const char* szString) = 0;
};
