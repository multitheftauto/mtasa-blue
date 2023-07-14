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

#define VEHICLES_COUNT 212

class CVehicleAudioSettingsManagerSA;

class CVehicleAudioSettingsManagerSA : public CVehicleAudioSettingsManager
{
public:
    CVehicleAudioSettingsManagerSA();
    ~CVehicleAudioSettingsManagerSA();

    CVehicleAudioSettingsEntry* CreateVehicleAudioSettingsData();

    bool ApplyAudioSettingsData(eVehicleTypes eModel, CVehicleAudioSettingsEntry* pEntry);

    CVehicleAudioSettingsEntry* GetVehicleModelAudioSettingsData(eVehicleTypes eModel);

    void SetNextSettings(CVehicleAudioSettingsEntry* pSettings);

private:
    void InitializeDefaultVehicleAudioSettings();
    uint GetVehicleModelAudioSettingsID(eVehicleTypes eModel);

    // Original audio settings
    SFixedArray<tVehicleAudioSettings, VEHICLES_COUNT> m_OriginalVehicleAudioSettings;

    // Array with the model audio settings entries
    SFixedArray<CVehicleAudioSettingsEntrySA*, VEHICLES_COUNT> m_pModelEntries;
};
