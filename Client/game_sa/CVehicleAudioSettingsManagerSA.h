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
#include "CVehicleAudioSettingsEntrySA.h"
#include "CAEVehicleAudioEntitySA.h"
#include <array>

constexpr size_t VEHICLES_COUNT = 212;

class CVehicleAudioSettingsManagerSA final : public CVehicleAudioSettingsManager
{
public:
    CVehicleAudioSettingsManagerSA();

    std::unique_ptr<CVehicleAudioSettingsEntry> CreateVehicleAudioSettingsData(uint32_t modelId) override;
    CVehicleAudioSettingsEntry& GetVehicleModelAudioSettingsData(uint32_t modelId) noexcept override;

    void ResetModelSettings(uint32_t modelId) noexcept override;
    void ResetAudioSettingsData() noexcept override;

    void SetNextSettings(CVehicleAudioSettingsEntry const* pSettings) noexcept override;
    void SetNextSettings(uint32_t modelId) noexcept override;

    static void StaticSetHooks() noexcept;

private:
    size_t GetVehicleModelAudioSettingsID(uint32_t modelId) const noexcept { return modelId - 400; };

private:
    // Array with the model audio settings entries
    std::array<CVehicleAudioSettingsEntrySA, VEHICLES_COUNT> m_modelEntrys;
};
