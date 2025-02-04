/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CVehicleAudioSettingsEntry.cpp
 *  PURPOSE:     Implementation file for vehicle audio settings manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVehicleAudioSettingsManagerSA.h"
#include <array>

const auto (&ORIGINAL_AUDIO_SETTINGS)[VEHICLES_COUNT] = *reinterpret_cast<const tVehicleAudioSettings (*)[VEHICLES_COUNT]>(0x860AF0);
tVehicleAudioSettings const * pNextVehicleAudioSettings = nullptr;

CVehicleAudioSettingsManagerSA::CVehicleAudioSettingsManagerSA()
{
    ResetAudioSettingsData();
}

CVehicleAudioSettingsEntry* CVehicleAudioSettingsManagerSA::CreateVehicleAudioSettingsData()
{
    CVehicleAudioSettingsEntrySA* pSettings = new CVehicleAudioSettingsEntrySA();
    return pSettings;
}

CVehicleAudioSettingsEntry* CVehicleAudioSettingsManagerSA::GetVehicleModelAudioSettingsData(eVehicleTypes eModel)
{
    size_t uiSettingsID = GetVehicleModelAudioSettingsID(eModel);
    if (uiSettingsID >= VEHICLES_COUNT)
        return nullptr;

    return &m_modelEntrys[uiSettingsID];
}

bool CVehicleAudioSettingsManagerSA::ApplyAudioSettingsData(eVehicleTypes eModel, CVehicleAudioSettingsEntry* pEntry)
{
    uint uiSettingsID = GetVehicleModelAudioSettingsID(eModel);
    if (uiSettingsID >= VEHICLES_COUNT)
        return false;

    m_modelEntrys[uiSettingsID].Assign(pEntry);
    return true;
}

void CVehicleAudioSettingsManagerSA::SetNextSettings(CVehicleAudioSettingsEntry* pSettings)
{
    pNextVehicleAudioSettings = &static_cast<CVehicleAudioSettingsEntrySA*>(pSettings)->getInterface();
}

void CVehicleAudioSettingsManagerSA::ResetAudioSettingsData()
{
    for (size_t i = 0; i < VEHICLES_COUNT; i++)
        m_modelEntrys[i].Assign(ORIGINAL_AUDIO_SETTINGS[i]);
}

void CVehicleAudioSettingsManagerSA::StaticSetHooks()
{
    // Replace 
    // 8D 34 B5 F0 0A 86 00 ; lea esi, _VehicleAudioProperties.m_eVehicleSoundType[esi*4]
    // to
    // 8b 35 XX XX XX XX ; mov esi, [pNextVehicleAudioSettings]
    // 90                ; nop
    MemCpy((void*)0x4F77CA, "\x8B\x35\x00\x00\x00\x00\x90", 7);
    MemPut(0x4F77CA + 2, (std::uint32_t)&pNextVehicleAudioSettings);
}
