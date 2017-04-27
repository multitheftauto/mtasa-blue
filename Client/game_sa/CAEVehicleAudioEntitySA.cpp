/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAEVehicleAudioEntitySA.cpp
*  PURPOSE:     Vehicle audio entity
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CAEVehicleAudioEntitySA::CAEVehicleAudioEntitySA ( CAEVehicleAudioEntitySAInterface * pInterface )
{
    m_pInterface = pInterface;
}

// Based on CAEVehicleAudioEntity::JustGotInVehicleAsDriver
// Loads accelerate sound bank for planes and helis.
void CAEVehicleAudioEntitySA::LoadDriverSounds ( void )
{
    CAEVehicleAudioEntitySAInterface * pVehicleAudioEntity = m_pInterface;
    char soundType = pVehicleAudioEntity->m_nSettings.m_nVehicleSoundType;

    // If it's heli or plane sound type
    if ( soundType == 4 || soundType == 5 )
    {
        // If there is accelerate sound bank defined
        if ( pVehicleAudioEntity->m_wEngineAccelerateSoundBankId != -1 )
        {
            CAEAudioHardware * pAEAudioHardware = pGame->GetAEAudioHardware ();
            // If this bank is not already loaded
            if ( !pAEAudioHardware->IsSoundBankLoaded ( pVehicleAudioEntity->m_wEngineAccelerateSoundBankId, BANKSLOT_ENGINE_RESIDENT ) )
            {
                // Load it
                pAEAudioHardware->LoadSoundBank ( pVehicleAudioEntity->m_wEngineAccelerateSoundBankId, BANKSLOT_ENGINE_RESIDENT );
            }
        }
    }
}
