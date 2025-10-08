/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CVehicleAudioSettingsManager.h
 *  PURPOSE:     Vehicle audio manager interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CVehicleAudioSettingsEntry.h"

class CVehicleAudioSettingsManager
{
public:
    virtual std::unique_ptr<CVehicleAudioSettingsEntry> CreateVehicleAudioSettingsData(uint32_t modelId) = 0;
    virtual CVehicleAudioSettingsEntry&                 GetVehicleModelAudioSettingsData(uint32_t modelId) noexcept = 0;
    
    virtual void ResetModelSettings(uint32_t modelId) noexcept = 0;
    virtual void ResetAudioSettingsData() noexcept = 0;
    virtual void SetNextSettings(CVehicleAudioSettingsEntry const* pSettings) noexcept = 0;
    virtual void SetNextSettings(uint32_t modelId) noexcept = 0;
};
