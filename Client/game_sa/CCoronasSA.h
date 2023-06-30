/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CCoronasSA.cpp
 *  PURPOSE:     Header file for corona entity manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/Common.h>
#include <game/CCoronas.h>

class CRegisteredCorona;
class CRegisteredCoronaSA;
struct RwTexture;

#define MAX_CORONAS                 64
#define MAX_CORONA_TEXTURES         9
#define ARRAY_CORONAS               0xC3E058 // also in CRegisteredCoronasSA.h

class CCoronasSA : public CCoronas
{
private:
    CRegisteredCoronaSA* Coronas[MAX_CORONAS];
    unsigned char        m_ucCoronaReflectionsEnabled;

public:
    CCoronasSA();
    ~CCoronasSA();

    CRegisteredCorona* GetCorona(DWORD ID);
    CRegisteredCorona* CreateCorona(DWORD Identifier, CVector* position);
    CRegisteredCorona* FindFreeCorona();
    CRegisteredCorona* FindCorona(DWORD Identifier);
    RwTexture*         GetTexture(eCoronaType type);

    void DisableSunAndMoon(bool bDisabled);

    void          SetCoronaReflectionsEnabled(unsigned char ucEnabled);
    unsigned char GetCoronaReflectionsEnabled();
};
