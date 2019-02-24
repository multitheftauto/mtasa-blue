/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CAntiCheatModule.cpp
 *  PURPOSE:     Anti-cheat module class
 *
 *****************************************************************************/

#include <StdInc.h>

CAntiCheatModule::CAntiCheatModule(CAntiCheat& AntiCheat) : m_AntiCheat(AntiCheat)
{
    AntiCheat.AddModule(*this);
}

CAntiCheatModule::~CAntiCheatModule()
{
    m_AntiCheat.RemoveModule(*this);
}
