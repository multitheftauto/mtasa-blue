/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/Audio/CAudioZonesSA.cpp
*  PURPOSE:     Source file for audio zones class
*
*  Multi Theft Auto is available from https://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"
#include "CAudioZonesSA.h"

// 0xB6DC6C CAudioZones::m_aActiveBoxes
// 0xB6DC94 CAudioZones::m_aActiveSpheres
// 0xB6DCBC CAudioZones::m_NumActiveBoxes
// 0xB6DCC0 CAudioZones::m_NumActiveSpheres
// 0xB6DCC4 CAudioZones::m_NumBoxes
// 0xB6DCC8 CAudioZones::m_NumSpheres
// 0xB6EBB0 CAudioZones::m_aSpheres

#define VAR_CAudioZonesSA__m_aBoxes 0xB6DCD0

CAudioZoneBoxSAInterface ( * CAudioZonesSA::m_aBoxes ) [100] = reinterpret_cast < decltype ( CAudioZonesSA::m_aBoxes ) > ( VAR_CAudioZonesSA__m_aBoxes );

void CAudioZonesSA::Init ( void )
{
    // 0x5081A0 void __cdecl CAudioZones::Init(void)
}

void CAudioZonesSA::RegisterAudioBox ( const char * szZoneName, int16_t wBank, bool bEnabled, float minX, float minY, float minZ, float maxX, float maxY, float maxZ )
{
    // 0x508240 void __cdecl CAudioZones::RegisterAudioBox(const char *, int16_t, bool, float, float, float, float, float, float)
}

void CAudioZonesSA::RegisterAudioSphere ( const char * szZoneName, int16_t wBank, bool bEnabled, float centerX, float centerY, float centerZ )
{
    // 0x5081C0 void __cdecl CAudioZones::RegisterAudioSphere(const char *, in16_t, bool, float, float, float, float)
}

void CAudioZonesSA::SwitchAudioZone ( const char * szZoneName, bool bEnabled )
{
    // 0x508320 void __cdecl CAudioZones::SwitchAudioZone(const char *, bool)    
}

void CAudioZonesSA::Update ( bool bForced )
{
    // 0x5083C0 CAudioZones::Update(bool) 
}
