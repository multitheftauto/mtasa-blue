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
#include <game/Common.h>

struct tVehicleAudioSettings
{
    eVehicleSoundType m_eVehicleSoundType;
    char  unk1;
    short m_wEngineOnSoundBankId;
    short m_wEngineOffSoundBankId;
    char  m_nStereo; // 0 or 1 or 2
    char  unk2;
    float unk3;
    float unk4;
    char  m_bHornTon;
    char  unk5[3];
    float m_fHornHigh;
    char  m_nDoorSound;
    char  unk6;
    char  m_nRadioNum;
    char  m_nRadioType;
    char  m_nVehTypeForAudio;
    char  unk8[3];
    float m_fHornVolumeDelta;
};
static_assert(sizeof(tVehicleAudioSettings) == 0x24, "Invalid size for tVehicleAudioSettings");

class CVehicleAudioSettingsEntrySA : public CVehicleAudioSettingsEntry
{
public:
    CVehicleAudioSettingsEntrySA();
    CVehicleAudioSettingsEntrySA(tVehicleAudioSettings* pSettings);
    ~CVehicleAudioSettingsEntrySA() = default;

    tVehicleAudioSettings* getInterface() { return &m_Settings; };

    void Assign(const CVehicleAudioSettingsEntry* pData);

    eVehicleSoundType GetSoundType() { return m_Settings.m_eVehicleSoundType; };
    short             GetEngineOnSoundBankID() { return m_Settings.m_wEngineOnSoundBankId; };
    short             GetEngineOffSoundBankID() { return m_Settings.m_wEngineOffSoundBankId; };
    char              GetStereo() { return m_Settings.m_nStereo; };
    float             GetUnk3() { return m_Settings.unk3; };
    float             GetUnk4() { return m_Settings.unk4; };
    char              GetHornTon() { return m_Settings.m_bHornTon; };
    float             GetHornHign() { return m_Settings.m_fHornHigh; };
    char              GetUnk6() { return m_Settings.unk6; };
    char              GetDoorSound() { return m_Settings.m_nDoorSound; };
    char              GetRadioNum() { return m_Settings.m_nRadioNum; };
    char              GetRadioType() { return m_Settings.m_nRadioType; };
    char              GetVehicleTypeForAudio() { return m_Settings.m_nVehTypeForAudio; };
    float             GetHornVolumeDelta() { return m_Settings.m_fHornVolumeDelta; };

    void SetSoundType(eVehicleSoundType value) { m_Settings.m_eVehicleSoundType = value; };
    void SetEngineOnSoundBankID(short value) { m_Settings.m_wEngineOnSoundBankId = value; };
    void SetEngineOffSoundBankID(short value) { m_Settings.m_wEngineOffSoundBankId = value; };
    void SetStereo(char value) { m_Settings.m_nStereo = value; };
    void SetUnk3(float value) { m_Settings.unk3= value; };
    void SetUnk4(float value) { m_Settings.unk4= value; };
    void SetHornTon(char value) { m_Settings.m_bHornTon = value; };
    void SetHornHign(float value) { m_Settings.m_fHornHigh = value; };
    void SetUnk6(char value) { m_Settings.unk6= value; };
    void SetDoorSound(char value) { m_Settings.m_nDoorSound = value; };
    void SetRadioNum(char value) { m_Settings.m_nRadioNum = value; };
    void SetRadioType(char value) { m_Settings.m_nRadioType = value; };
    void SetVehicleTypeForAudio(char value) { m_Settings.m_nVehTypeForAudio = value; };
    void SetHornVolumeDelta(float value) { m_Settings.m_fHornVolumeDelta = value; };

private:
    tVehicleAudioSettings m_Settings;
};
