/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CAEVehicleAudioEntitySA.cpp
 *  PURPOSE:     Vehicle audio entity
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
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
    DWORD dwFunc = 0x4F5700;
    DWORD dwThis = (DWORD)m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CAEVehicleAudioEntitySA::JustGotOutOfVehicleAsDriver()
{
    m_pInterface->m_bPlayerDriver = false;
    DWORD dwFunc = 0x4FCF40;
    DWORD dwThis = (DWORD)m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CAEVehicleAudioEntitySA::TurnOnRadioForVehicle()
{
    DWORD dwFunc = 0x4F5B20;
    DWORD dwThis = (DWORD)m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CAEVehicleAudioEntitySA::StopVehicleEngineSound(uchar ucSlot)
{
    DWORD          dwFunc = 0x4EF1C0;
    tVehicleSound* pVehicleSound = &m_pInterface->m_aEngineSounds[ucSlot];
    if (pVehicleSound->m_pSound)
    {
        DWORD dwThis = (DWORD)pVehicleSound->m_pSound;
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
    }
}
