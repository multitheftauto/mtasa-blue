
#include "StdInc.h"

CVehicleAudioSettingsEntrySA::CVehicleAudioSettingsEntrySA()
{
    memset(&m_Settings, 0, sizeof(tVehicleAudioSettings));
}

CVehicleAudioSettingsEntrySA::CVehicleAudioSettingsEntrySA(tVehicleAudioSettings* pSettingsEntry)
{
    m_Settings = *pSettingsEntry;
}

CVehicleAudioSettingsEntrySA::~CVehicleAudioSettingsEntrySA()
{

}

void CVehicleAudioSettingsEntrySA::Assign(const CVehicleAudioSettingsEntry* pData)
{
    const CVehicleAudioSettingsEntrySA* pEntrySA = static_cast<const CVehicleAudioSettingsEntrySA*>(pData);

    m_Settings = pEntrySA->m_Settings;
}
