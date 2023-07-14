/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CVehicleAudioSettingsEntrySA.cpp
 *  PURPOSE:     Implementation file for vehicle audio settings entry class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVehicleAudioSettingsEntrySA.h"

CVehicleAudioSettingsEntrySA::CVehicleAudioSettingsEntrySA()
{
    memset(&m_Settings, 0, sizeof(tVehicleAudioSettings));
}

CVehicleAudioSettingsEntrySA::CVehicleAudioSettingsEntrySA(tVehicleAudioSettings* pSettingsEntry)
{
    m_Settings = *pSettingsEntry;
}

void CVehicleAudioSettingsEntrySA::Assign(const CVehicleAudioSettingsEntry* pData)
{
    const CVehicleAudioSettingsEntrySA* pEntrySA = static_cast<const CVehicleAudioSettingsEntrySA*>(pData);

    m_Settings = pEntrySA->m_Settings;
}
