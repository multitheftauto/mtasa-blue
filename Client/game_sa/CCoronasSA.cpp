/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCoronasSA.cpp
 *  PURPOSE:     Corona entity manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCoronasSA.h"
#include "CRegisteredCoronaSA.h"

CCoronasSA::CCoronasSA()
{
    for (int i = 0; i < MAX_CORONAS; i++)
    {
        Coronas[i] = new CRegisteredCoronaSA((CRegisteredCoronaSAInterface*)(ARRAY_CORONAS + i * sizeof(CRegisteredCoronaSAInterface)));
    }
}

CCoronasSA::~CCoronasSA()
{
    for (int i = 0; i < MAX_CORONAS; i++)
    {
        delete Coronas[i];
    }
}

CRegisteredCorona* CCoronasSA::GetCorona(DWORD ID)
{
    return (CRegisteredCorona*)Coronas[ID];
}

CRegisteredCorona* CCoronasSA::CreateCorona(DWORD Identifier, CVector* position)
{
    CRegisteredCoronaSA* corona;
    corona = (CRegisteredCoronaSA*)FindCorona(Identifier);

    if (!corona)
        corona = (CRegisteredCoronaSA*)FindFreeCorona();

    if (corona)
    {
        RwTexture* texture = GetTexture((eCoronaType)CORONATYPE_SHINYSTAR);
        if (texture)
        {
            corona->Init(Identifier);
            corona->SetPosition(position);
            corona->SetTexture(texture);
            return (CRegisteredCorona*)corona;
        }
    }

    return (CRegisteredCorona*)NULL;
}

CRegisteredCorona* CCoronasSA::FindFreeCorona()
{
    for (int i = 2; i < MAX_CORONAS; i++)
    {
        if (Coronas[i]->GetIdentifier() == 0)
        {
            return Coronas[i];
        }
    }
    return (CRegisteredCorona*)NULL;
}

CRegisteredCorona* CCoronasSA::FindCorona(DWORD Identifier)
{
    for (int i = 0; i < MAX_CORONAS; i++)
    {
        if (Coronas[i]->GetIdentifier() == Identifier)
        {
            return Coronas[i];
        }
    }
    return (CRegisteredCorona*)NULL;
}

RwTexture* CCoronasSA::GetTexture(eCoronaType type)
{
    if (type < MAX_CORONA_TEXTURES)
        return (RwTexture*)(*(DWORD*)(ARRAY_CORONA_TEXTURES + type * sizeof(DWORD)));
    else
        return NULL;
}

void CCoronasSA::DisableSunAndMoon(bool bDisabled)
{
    static BYTE byteOriginal = 0;
    if (bDisabled && !byteOriginal)
    {
        byteOriginal = *(BYTE*)FUNC_DoSunAndMoon;
        MemPut<BYTE>(FUNC_DoSunAndMoon, 0xC3);
    }
    else if (!bDisabled && byteOriginal)
    {
        MemPut<BYTE>(FUNC_DoSunAndMoon, byteOriginal);
        byteOriginal = 0;
    }
}

/*
    Enable or disable corona rain reflections.
    ucEnabled:
     0 - disabled
     1 - enabled
     2 - force enabled (render even if there is no rain)
*/
void CCoronasSA::SetCoronaReflectionsEnabled(unsigned char ucEnabled)
{
    m_ucCoronaReflectionsEnabled = ucEnabled;

    if (ucEnabled == 0)
    {
        // Disable corona rain reflections
        // Return out CCoronas::RenderReflections()
        MemPut<BYTE>(0x6FB630, 0xC3);
    }
    else
    {
        // Enable corona rain reflections
        // Re-enable CCoronas::RenderReflections()
        MemPut<BYTE>(0x6FB630, 0xD9);
    }

    if (ucEnabled == 2)
    {
        // Force enable corona reflections (render even if there is no rain)
        // Disable fWetGripScale check
        MemPut<BYTE>(0x6FB645, 0xEB);

        // Patch "fld fWetGripScale" to "fld fOne"
        MemCpy((void*)0x6FB906, "\x24\x86\x85\x00", 4);
    }
    else
    {
        // Restore patched code
        MemPut<BYTE>(0x6FB645, 0x7A);
        MemCpy((void*)0x6FB906, "\x08\x13\xC8\x00", 4);
    }
}

unsigned char CCoronasSA::GetCoronaReflectionsEnabled()
{
    return m_ucCoronaReflectionsEnabled;
}
