/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CCoronas.h
 *  PURPOSE:     Corona entity manager interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "enums/CoronaType.h"

class CRegisteredCorona;
class CVector;

// these are the IDs that are used internally for coronas
#define CORONA_SUN                  1
#define CORONA_MOON                 2

class CCoronas
{
public:
    virtual CRegisteredCorona* GetCorona(DWORD ID) = 0;
    virtual CRegisteredCorona* CreateCorona(DWORD Identifier, CVector* position) = 0;
    virtual CRegisteredCorona* FindFreeCorona() = 0;
    virtual CRegisteredCorona* FindCorona(DWORD Identifier) = 0;
    virtual void               DisableSunAndMoon(bool bDisabled) = 0;
    virtual void               SetCoronaReflectionsEnabled(unsigned char ucEnabled) = 0;
    virtual unsigned char      GetCoronaReflectionsEnabled() = 0;
};
