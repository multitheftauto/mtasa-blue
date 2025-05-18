/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CVehicleAudioSettingsEntrySA.h
 *  PURPOSE:     Header file for vehicle audio settings entry class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "game/CVehicleAudioSettingsEntry.h"
#include "enums/VehicleSoundType.h"

// SA interface
struct tVehicleAudioSettings
{
    VehicleSoundType m_eVehicleSoundType;
    int16             m_nEngineOnSoundBankId;
    int16             m_nEngineOffSoundBankId;
    int8              m_nBassSetting;            // 0 or 1 or 2
    float             m_fBassEq;
    float             field_C;
    int8              m_nHornToneSoundInBank;
    float             m_fHornHigh;
    char              m_nDoorSound;
    char              m_EngineUpgrade;
    char              m_nRadioID;
    char              m_nRadioType;
    char              m_nVehTypeForAudio;
    float             m_fHornVolumeDelta;
};
static_assert(sizeof(tVehicleAudioSettings) == 0x24, "Invalid size for tVehicleAudioSettings");

class CVehicleAudioSettingsEntrySA final : public CVehicleAudioSettingsEntry
{
public:
    CVehicleAudioSettingsEntrySA(): m_Settings{} {};
    CVehicleAudioSettingsEntrySA(tVehicleAudioSettings* pSettings) { m_Settings = *pSettings; };
    ~CVehicleAudioSettingsEntrySA() = default;

    const tVehicleAudioSettings& GetInterface() const noexcept { return m_Settings; };

    void Assign(const tVehicleAudioSettings& settings) noexcept { m_Settings = settings; }
    void Assign(const CVehicleAudioSettingsEntry& settings) noexcept { m_Settings = static_cast<const CVehicleAudioSettingsEntrySA&>(settings).GetInterface(); };

    VehicleSoundType  GetSoundType() const noexcept override { return m_Settings.m_eVehicleSoundType; };
    short             GetEngineOnSoundBankID() const noexcept override { return m_Settings.m_nEngineOnSoundBankId; };
    short             GetEngineOffSoundBankID() const noexcept override { return m_Settings.m_nEngineOffSoundBankId; };
    char              GetBassSetting() const noexcept override { return m_Settings.m_nBassSetting; };
    float             GetBassEq() const noexcept override { return m_Settings.m_fBassEq; };
    float             GetFieldC() const noexcept override { return m_Settings.field_C; };
    char              GetHornTon() const noexcept override { return m_Settings.m_nHornToneSoundInBank; };
    float             GetHornHign() const noexcept override { return m_Settings.m_fHornHigh; };
    char              GetEngineUpgrade() const noexcept override { return m_Settings.m_EngineUpgrade; };
    char              GetDoorSound() const noexcept override { return m_Settings.m_nDoorSound; };
    char              GetRadioNum() const noexcept override { return m_Settings.m_nRadioID; };
    char              GetRadioType() const noexcept override { return m_Settings.m_nRadioType; };
    char              GetVehicleTypeForAudio() const noexcept override { return m_Settings.m_nVehTypeForAudio; };
    float             GetHornVolumeDelta() const noexcept override { return m_Settings.m_fHornVolumeDelta; };

    void SetSoundType(VehicleSoundType value) noexcept override { m_Settings.m_eVehicleSoundType = value; };
    void SetEngineOnSoundBankID(short value) noexcept override { m_Settings.m_nEngineOnSoundBankId = value; };
    void SetEngineOffSoundBankID(short value) noexcept override { m_Settings.m_nEngineOffSoundBankId = value; };
    void SetBassSetting(char value) noexcept override { m_Settings.m_nBassSetting = value; };
    void SetBassEq(float value) noexcept override { m_Settings.m_fBassEq = value; };
    void SetFieldC(float value) noexcept override { m_Settings.field_C = value; };
    void SetHornTon(char value) noexcept override { m_Settings.m_nHornToneSoundInBank = value; };
    void SetHornHign(float value) noexcept override { m_Settings.m_fHornHigh = value; };
    void SetEngineUpgrade(char value) noexcept override { m_Settings.m_EngineUpgrade = value; };
    void SetDoorSound(char value) noexcept override { m_Settings.m_nDoorSound = value; };
    void SetRadioNum(char value) noexcept override { m_Settings.m_nRadioID = value; };
    void SetRadioType(char value) noexcept override { m_Settings.m_nRadioType = value; };
    void SetVehicleTypeForAudio(char value) noexcept override { m_Settings.m_nVehTypeForAudio = value; };
    void SetHornVolumeDelta(float value) noexcept override { m_Settings.m_fHornVolumeDelta = value; };

private:
    tVehicleAudioSettings m_Settings;
};
