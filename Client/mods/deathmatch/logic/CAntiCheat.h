/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CAntiCheat.h
 *  PURPOSE:     Anti-cheat handling class
 *
 *****************************************************************************/

#pragma once

#include "CAntiCheatModule.h"
#include <list>

class CAntiCheat
{
public:
    CAntiCheat();
    ~CAntiCheat();

    void AddModule(CAntiCheatModule& Module);
    void RemoveModule(CAntiCheatModule& Module);
    void ClearModules();

    bool           PerformChecks();
    static SString GetInfo(const SString& acInfo, const SString& sdInfo);

private:
    std::list<CAntiCheatModule*>           m_Entries;
    std::list<CAntiCheatModule*>::iterator m_LastEntry;
};
