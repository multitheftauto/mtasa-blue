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
    uchar             GetEngineOnSoundBankID() { return m_Settings.m_wEngineOnSoundBankId; };
    uchar             GetEngineOffSoundBankID() { return m_Settings.m_wEngineOffSoundBankId; };
    uchar             GetStereo() { return m_Settings.m_nStereo; };
    float             GetUnk3() { return m_Settings.unk3; };
    float             GetUnk4() { return m_Settings.unk4; };
    uchar             GetHornTon() { return m_Settings.m_bHornTon; };
    float             GetHornHign() { return m_Settings.m_fHornHigh; };
    uchar             GetUnk6() { return m_Settings.unk6; };
    uchar             GetDoorSound() { return m_Settings.m_nDoorSound; };
    uchar             GetRadioNum() { return m_Settings.m_nRadioNum; };
    uchar             GetRadioType() { return m_Settings.m_nRadioType; };
    uchar             GetVehicleTypeForAudio() { return m_Settings.m_nVehTypeForAudio; };
    float             GetHornVolumeDelta() { return m_Settings.m_fHornVolumeDelta; };

    void SetSoundType(eVehicleSoundType value) { m_Settings.m_eVehicleSoundType = value; };
    void SetEngineOnSoundBankID(uchar value) { m_Settings.m_wEngineOnSoundBankId = value; };
    void SetEngineOffSoundBankID(uchar value) { m_Settings.m_wEngineOffSoundBankId = value; };
    void SetStereo(uchar value) { m_Settings.m_nStereo = value; };
    void SetUnk3(float value) { m_Settings.unk3= value; };
    void SetUnk4(float value) { m_Settings.unk4= value; };
    void SetHornTon(uchar value) { m_Settings.m_bHornTon = value; };
    void SetHornHign(float value) { m_Settings.m_fHornHigh = value; };
    void SetUnk6(uchar value) { m_Settings.unk6= value; };
    void SetDoorSound(uchar value) { m_Settings.m_nDoorSound = value; };
    void SetRadioNum(uchar value) { m_Settings.m_nRadioNum = value; };
    void SetRadioType(uchar value) { m_Settings.m_nRadioType = value; };
    void SetVehicleTypeForAudio(uchar value) { m_Settings.m_nVehTypeForAudio = value; };
    void SetHornVolumeDelta(float value) { m_Settings.m_fHornVolumeDelta = value; };

private:
    tVehicleAudioSettings m_Settings;
};
