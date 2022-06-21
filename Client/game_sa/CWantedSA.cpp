/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWantedSA.cpp
 *  PURPOSE:     Wanted level management
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CWantedSA::CWantedSA()
{
    // TODO: Call GTA's new operator for CWanted. Lack of proper initialization might be causing crashes.

    internalInterface = new CWantedSAInterface;
    MemSetFast(internalInterface, 0, sizeof(CWantedSAInterface));

    m_bDontDelete = false;
}

CWantedSA::CWantedSA(CWantedSAInterface* wantedInterface)
{
    internalInterface = wantedInterface;
    m_bDontDelete = true;
}

CWantedSA::~CWantedSA()
{
    if (!m_bDontDelete)
    {
        // TODO: Call GTA's delete operator for CWanted. Lack of proper destruction might be causing crashes.

        delete internalInterface;
    }
}

void CWantedSA::SetMaximumWantedLevel(DWORD dwWantedLevel)
{
    // CWanted::SetMaximumWantedLevel
    ((void(__cdecl*)(int))FUNC_SetMaximumWantedLevel)(dwWantedLevel);
}

void CWantedSA::SetWantedLevel(DWORD dwWantedLevel)
{
    // CWanted::SetWantedLevel
    ((void(__thiscall*)(CWantedSAInterface*, int))FUNC_SetWantedLevel)(this->GetInterface(), dwWantedLevel);
}
void CWantedSA::SetWantedLevelNoFlash(DWORD dwWantedLevel)
{
    DWORD dwLastTimeChanged = internalInterface->m_LastTimeWantedLevelChanged;
    SetWantedLevel(dwWantedLevel);
    internalInterface->m_LastTimeWantedLevelChanged = dwLastTimeChanged;
}

void CWantedSA::SetWantedLevelNoDrop(DWORD dwWantedLevel)
{
    // CWanted::SetWantedLevelNoDrop
    ((void(__thiscall*)(CWantedSAInterface*, int))FUNC_SetWantedLevelNoDrop)(this->GetInterface(), dwWantedLevel);
}
