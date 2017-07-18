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
    if ( soundType == VEHICLE_AUDIO_HELI || soundType == VEHICLE_AUDIO_PLANE || soundType == VEHICLE_AUDIO_SEAPLANE )
    {
        // If there is accelerate sound bank defined
        if ( pVehicleAudioEntity->m_wEngineAccelerateSoundBankId != -1 )
        {
            // We want to hear new sounds ASAP, so we need to stop current ones (which may be from other bankslot, dummy sounds)
            unsigned char ucSlot = 0;
            while ( ucSlot < 12 )
            {
                StopVehicleEngineSound ( ucSlot++ );
            };

            CAEAudioHardware * pAEAudioHardware = pGame->GetAEAudioHardware ();
            // If this bank is not already loaded
            if ( !pAEAudioHardware->IsSoundBankLoaded ( pVehicleAudioEntity->m_wEngineAccelerateSoundBankId, BANKSLOT_ENGINE_RESIDENT ) )
            {
                // Cancel sounds which uses same bankslot
                DWORD dwFunc = FUNC_CAESoundManager__CancelSoundsInBankSlot;
                DWORD dwThis = VAR_pAESoundManager;
                DWORD dwBankSlot = BANKSLOT_ENGINE_RESIDENT;
                bool bUnk = false;
                _asm
                {
                    push    bUnk
                    push    dwBankSlot
                    mov     ecx, dwThis
                    call    dwFunc
                }

                // Load it
                pAEAudioHardware->LoadSoundBank ( pVehicleAudioEntity->m_wEngineAccelerateSoundBankId, BANKSLOT_ENGINE_RESIDENT );
            }
        }
    }
}

void CAEVehicleAudioEntitySA::StopVehicleEngineSound ( unsigned char ucSlot )
{
    DWORD dwFunc = FUNC_CAESound__Stop;
    tVehicleSound * pVehicleSound = &m_pInterface->m_aEngineSounds [ ucSlot ];
    if ( pVehicleSound->m_pSound )
    {
        DWORD dwThis = (DWORD) pVehicleSound->m_pSound;
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
    }
}
