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

CAEVehicleAudioEntitySA::CAEVehicleAudioEntitySA(CAEVehicleAudioEntitySAInterface* pInterface)
{
    m_pInterface = pInterface;
}

void CAEVehicleAudioEntitySA::JustGotInVehicleAsDriver()
{
    m_pInterface->m_bPlayerDriver = true;

    // CAEVehicleAudioEntity::JustGotInVehicleAsDriver
    ((void(__thiscall*)(CAEVehicleAudioEntitySAInterface*))FUNC_CAEVehicleAudioEntity__JustGotInVehicleAsDriver)(m_pInterface);
}

void CAEVehicleAudioEntitySA::JustGotOutOfVehicleAsDriver()
{
    m_pInterface->m_bPlayerDriver = false;

    // CAEVehicleAudioEntity::JustGotInVehicleAsDriver
    ((void(__thiscall*)(CAEVehicleAudioEntitySAInterface*))FUNC_CAEVehicleAudioEntity__JustGotOutOfVehicleAsDriver)(m_pInterface);
}

void CAEVehicleAudioEntitySA::TurnOnRadioForVehicle()
{
    // CAEVehicleAudioEntity::TurnOnRadioForVehicle
    ((void(__thiscall*)(CAEVehicleAudioEntitySAInterface*))FUNC_CAEVehicleAudioEntity__TurnOnRadioForVehicle)(m_pInterface);
}

void CAEVehicleAudioEntitySA::StopVehicleEngineSound(unsigned char ucSlot)
{
    tVehicleSound* pVehicleSound = &m_pInterface->m_aEngineSounds[ucSlot];
    if (pVehicleSound->m_pSound)
    {
        // CAESound::Stop
        ((void(__thiscall*)(CAESound*))FUNC_CAESound__Stop)(pVehicleSound->m_pSound);
    }
}
