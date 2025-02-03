/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CVehicleAudioSettingsManagerSA.h
 *  PURPOSE:     Header file for vehicle audio settings manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CVehicleAudioSettingsManager.h>
#include "CAEVehicleAudioEntitySA.h"
#include <array>

constexpr size_t VEHICLES_COUNT = 212;

class CVehicleAudioSettingsManagerSA final : public CVehicleAudioSettingsManager
{
public:
    CVehicleAudioSettingsManagerSA();

    CVehicleAudioSettingsEntry* CreateVehicleAudioSettingsData();
    CVehicleAudioSettingsEntry* GetVehicleModelAudioSettingsData(eVehicleTypes eModel);

    bool ApplyAudioSettingsData(eVehicleTypes eModel, CVehicleAudioSettingsEntry* pEntry);
    void SetNextSettings(CVehicleAudioSettingsEntry* pSettings);

    static void StaticSetHooks();

private:
    void   ResetAudioSettingsData();
    size_t GetVehicleModelAudioSettingsID(eVehicleTypes eModel) const noexcept { return eModel - 400; };

private:
    // Array with the model audio settings entries
    std::array<CVehicleAudioSettingsEntrySA, VEHICLES_COUNT> m_modelEntrys;
};
