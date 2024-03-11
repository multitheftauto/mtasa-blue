/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CHandlingManager.h
 *  PURPOSE:     Vehicle handling manager interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CVehicleAudioSettingsEntry.h"

class CVehicleAudioSettingsManager
{
public:
    virtual CVehicleAudioSettingsEntry* CreateVehicleAudioSettingsData() = 0;
    
    virtual CVehicleAudioSettingsEntry* GetVehicleModelAudioSettingsData(eVehicleTypes eModel) = 0;
    virtual void                        SetNextSettings(CVehicleAudioSettingsEntry* pSettings) = 0;
};
