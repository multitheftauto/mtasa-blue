/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CAntiCheatModule.cpp
*  PURPOSE:     Anti-cheat module class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

CAntiCheatModule::CAntiCheatModule ( CAntiCheat& AntiCheat ) : m_AntiCheat ( AntiCheat )
{
    AntiCheat.AddModule ( *this );
}


CAntiCheatModule::~CAntiCheatModule ( void )
{
    m_AntiCheat.RemoveModule ( *this );
}
