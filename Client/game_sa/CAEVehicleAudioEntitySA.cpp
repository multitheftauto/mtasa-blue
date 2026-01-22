/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAEVehicleAudioEntitySA.cpp
 *  PURPOSE:     Vehicle audio entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAEVehicleAudioEntitySA.h"

CAEVehicleAudioEntitySA::CAEVehicleAudioEntitySA(CAEVehicleAudioEntitySAInterface* pInterface)
{
    m_pInterface = pInterface;
}

void CAEVehicleAudioEntitySA::JustGotInVehicleAsDriver()
{
    m_pInterface->m_bPlayerDriver = true;
    DWORD dwFunc = FUNC_CAEVehicleAudioEntity__JustGotInVehicleAsDriver;
    DWORD dwThis = (DWORD)m_pInterface;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
    // clang-format on
}

void CAEVehicleAudioEntitySA::JustGotOutOfVehicleAsDriver()
{
    m_pInterface->m_bPlayerDriver = false;
    DWORD dwFunc = FUNC_CAEVehicleAudioEntity__JustGotOutOfVehicleAsDriver;
    DWORD dwThis = (DWORD)m_pInterface;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
    // clang-format on
}

void CAEVehicleAudioEntitySA::TurnOnRadioForVehicle()
{
    DWORD dwFunc = FUNC_CAEVehicleAudioEntity__TurnOnRadioForVehicle;
    DWORD dwThis = (DWORD)m_pInterface;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
    // clang-format on
}

void CAEVehicleAudioEntitySA::StopVehicleEngineSound(unsigned char ucSlot)
{
    DWORD          dwFunc = FUNC_CAESound__Stop;
    tVehicleSound* pVehicleSound = &m_pInterface->m_aEngineSounds[ucSlot];
    if (pVehicleSound->m_pSound)
    {
        DWORD dwThis = (DWORD)pVehicleSound->m_pSound;
        // clang-format off
        __asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
        // clang-format on
    }
}
