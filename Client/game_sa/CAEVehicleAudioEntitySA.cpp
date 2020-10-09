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

//static auto s_aEngineDummySlots = (CAEVehicleAudioEntitySA::DummyEngineSlot*)(VAR_CAEVehicleAudioEntity__s_aDummyEngineSlots);

CAEVehicleAudioEntitySA::CAEVehicleAudioEntitySA(CAEVehicleAudioEntitySAInterface* pInterface)
{
    m_pInterface = pInterface;
}

void CAEVehicleAudioEntitySA::JustGotInVehicleAsDriver()
{
    m_pInterface->m_bPlayerDriver = true;
    DWORD dwFunc = FUNC_CAEVehicleAudioEntity__JustGotInVehicleAsDriver;
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
    DWORD dwFunc = FUNC_CAEVehicleAudioEntity__JustGotOutOfVehicleAsDriver;
    DWORD dwThis = (DWORD)m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CAEVehicleAudioEntitySA::TurnOnRadioForVehicle()
{
    DWORD dwFunc = FUNC_CAEVehicleAudioEntity__TurnOnRadioForVehicle;
    DWORD dwThis = (DWORD)m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CAEVehicleAudioEntitySA::StopVehicleEngineSound(unsigned char ucSlot)
{
    tVehicleSound* pVehicleSound = &m_pInterface->m_aEngineSounds[ucSlot];
    if (pVehicleSound->m_pSound)
        ((void(__cdecl*)(CAESound*))FUNC_CAESound__Stop)(pVehicleSound->m_pSound);
}

int CAEVehicleAudioEntitySA::StoppedUsingBankSlot(int bankId)
{
    return ((int(__cdecl*)(int))FUNC_CAEVehicleAudioEntity__StoppedUsingBankSlot)(bankId);
}

short CAEVehicleAudioEntitySA::RequestBankSlot(unsigned short bankId)
{
    return ((short(__cdecl*)(short))FUNC_CAEVehicleAudioEntity__RequestBankSlot)(bankId);
}

short CAEVehicleAudioEntitySA::DemandBankSlot(unsigned short bankId)
{
    return ((short(__cdecl*)(short))FUNC_CAEVehicleAudioEntity__DemandBankSlot)(bankId);
}

void CAEVehicleAudioEntitySA::CancelVehicleEngineSound(short engineSoundStateID)
{
    return ((void(__thiscall*)(void*, short))FUNC_CAEVehicleAudioEntity__CancelVehicleEngineSound)((void*)m_pInterface, engineSoundStateID);
}

void CAEVehicleAudioEntitySA::MakeSureEngineSoundsAreLoaded()
{
    // TODO: Check if this is the right way to do this
    //       (aka should we write a function to load the sound bank, and all that, or this hack is just fine)
    ((void(__thiscall*)(void*))FUNC_CAEVehicleAudioEntity__JustGotInVehicleAsDriver)((void*)m_pInterface);
}

unsigned int CAEVehicleAudioEntitySA::SetEngineAccelerationSoundBank(unsigned int bankId) // Seems to crash with id 83
{
    if (bankId == m_pInterface->m_wEngineOnSoundBankId)
        return m_pInterface->m_wEngineOnSoundBankId;

    // TODO: Add check for bankid (maybe use CAEAudioHardware or something)
    auto oldValue = m_pInterface->m_wEngineOnSoundBankId;
    m_pInterface->m_wEngineOnSoundBankId = bankId;
    m_pInterface->m_nSettings.m_wEngineOnSoundBankId = bankId;

    MakeSureEngineSoundsAreLoaded();

    return oldValue;
}

unsigned int CAEVehicleAudioEntitySA::SetEngineDeaccelerationSoundBank(unsigned int bankId)
{
    if (bankId == m_pInterface->m_wEngineOffSoundBankId)
        return m_pInterface->m_wEngineOffSoundBankId;

    // TODO: Add check for bankid (maybe use CAEAudioHardware or something)
    const short previousBank = m_pInterface->m_wEngineOffSoundBankId;
    const short previousSlot = m_pInterface->m_wEngineSlotInBank;

    m_pInterface->m_wEngineOffSoundBankId = bankId;
    m_pInterface->m_nSettings.m_wEngineOffSoundBankId = bankId;

    StoppedUsingBankSlot(m_pInterface->m_wEngineSlotInBank);
    m_pInterface->m_wEngineSlotInBank = RequestBankSlot(bankId); // Loads the sound bank (if it wasnt)

    if (m_pInterface->m_wEngineSlotInBank == -1) // failed to load, revert to the previous one
    {
        m_pInterface->m_wEngineOffSoundBankId = previousBank;
        m_pInterface->m_nSettings.m_wEngineOffSoundBankId = previousBank;
        m_pInterface->m_wEngineSlotInBank = previousSlot;
    }
    else
    {
        if (!pGame->GetAEAudioHardware()->IsSoundBankLoaded(bankId, m_pInterface->m_wEngineSlotInBank))
            if (!pGame->GetAEAudioHardware()->LoadSoundBank(bankId, m_pInterface->m_wEngineSlotInBank))
                throw std::invalid_argument("sound bank didnt get loaded");
    }

    return previousBank;
}
