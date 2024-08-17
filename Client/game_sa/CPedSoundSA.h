/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPedSoundSA.h
 *  PURPOSE:     Header file for ped sound class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPedSound.h>
#include "CAudioEngineSA.h"
#include "CAEVehicleAudioEntitySA.h"

class CPedSAInterface;

#define FUNC_CAEPedSound__GetVoice          0x4E3CD0        // 4E3CD0 ; public: static short __cdecl CAEPedSound::GetVoice(char *,short)
#define FUNC_CAEPedSound__GetAudioPedType   0x4E3C60        // 4E3C60 ; public: static short __cdecl CAEPedSound::GetAudioPedType(char *)
#define FUNC_CAEPedSound__SetPed            0x4E68D0        // 4E69D0 ; public: void __thiscall CAEPedSound::SetPed(CPedSAInterface *)
#define FUNC_CAEPedSound__EnablePedSpeech   0x4e3f70
#define FUNC_CAEPedSound__DisablePedSpeech  0x4e56d0
#define FUNC_CAEPedSound__IsSpeedDisabled   0x4e34d0

#define VAR_CAEPedSound__VoiceTypeNames     0x8C8108        // Array of 6 pointers to strings

#define VAR_CAEPedSound__VoiceNames_GEN     0x8AE6A8        // 20 bytes per voice name
#define VAR_CAEPedSound__VoiceNames_EMG     0x8BA0D8
#define VAR_CAEPedSound__VoiceNames_PLAYER  0x8BBD40
#define VAR_CAEPedSound__VoiceNames_GANG    0x8BE1A8
#define VAR_CAEPedSound__VoiceNames_GFD     0x8C4120

#define NUM_PED_VOICE_TYPES 5

enum
{
    PED_TYPE_GEN,
    PED_TYPE_EMG,
    PED_TYPE_PLAYER,
    PED_TYPE_GANG,
    PED_TYPE_GFD
};

#define NUM_GEN_VOICES     209
#define NUM_EMG_VOICES     46
#define NUM_PLAYER_VOICES  20
#define NUM_GANG_VOICES    52
#define NUM_GFD_VOICES     18

typedef struct
{
    char szName[20];
} SPedVoiceName;

// CAEPedSpeechAudioEntity
class CPedSoundSAInterface : public CAEAudioEntity
{
public:
    std::uint8_t unk_7C[14];
    std::int8_t  unk_90;
    std::uint8_t unk_91;
    std::int16_t m_sVoiceType;
    std::int16_t m_sVoiceID;
    std::int16_t m_bIsFemale;            // 0 = male, 1 = female
    bool         m_bTalking;
    bool         m_bDisabled;            // m_bEnableSpeech
    bool         m_bEnableSpeechForScripts;
    std::uint8_t m_vocalEnableFlag;
    std::uint8_t unk_9C[4];            // From 9C to 9F
    CAESound*    m_pSound;             // CSound*
    std::int16_t m_soundId;
    std::int16_t m_bankId;
    std::int16_t m_pedSpeechSlotIndex;
    std::uint8_t unk_A4[4];
    float        m_fVoiceVolume;
    std::int16_t m_sPhraseId;
    std::int16_t unk_B2;
    std::uint8_t unk_B4[76];
};

// CAEPedAudioEntity
class CPedSoundEntitySAInterface : public CAEAudioEntity
{
public:
    std::uint8_t           unk_7C[24];            // from 7C to 8C
    CPedSAInterface*       ped;
    std::uint8_t           unk_98[16];            // from 98 to A8
    CAETwinLoopSoundEntity twinLoopSoundEntity;
    std::uint8_t           unk_150[12];            // from 150 to 15C
};

// CAEPedWeaponAudioEntity
class CPedWeaponAudioEntitySAInterface : public CAEAudioEntity
{
public:
    bool                playedMiniGunFireSound;
    bool                unk_7D;               // CAEWeaponAudioEntity::PlayMiniGunFireSounds
    std::uint8_t        unk_7E[2];            // from 7E to 7F
    std::uint8_t        chainsawSoundState;
    std::uint8_t        unk_81[3];            // from 81 to 83
    std::uint32_t       flameThrowerLastPlayedTime;
    std::uint32_t       spraycanLastPlayedTime;
    std::uint32_t       extinguisherLastPlayedTime;
    std::uint32_t       miniGunFireSoundPlayedTime;
    std::uint32_t       timeChainsaw;
    std::uint32_t       timeLastFired;
    void*               sounds;
    bool                active;
    std::uint8_t        unk_A1[3];            // from A1 to A3
    CPedSAInterface*    ped;
};

class CPedSoundSA : public CPedSound
{
private:
    static int            m_iNumVoicesPerType[];
    static SPedVoiceName* m_pVoiceNamesPerType[];

    CPedSoundSAInterface* m_pInterface;

public:
    CPedSoundSA(CPedSoundSAInterface* pInterface) { m_pInterface = pInterface; }
    CPedSoundSAInterface* GetInterface() { return m_pInterface; }

    short GetVoiceTypeID();
    short GetVoiceID();
    void  SetVoiceTypeID(short sVoiceType);
    void  SetVoiceID(short sVoiceID);

    bool IsSpeechDisabled();
    void EnablePedSpeech();
    void DisablePedSpeech(bool bStopCurrent);

    static short GetVoiceTypeIDFromName(const char* szVoiceTypeName);
    static short GetVoiceIDFromName(short sVoiceTypeID, const char* szVoiceName);
    static short GetVoiceIDFromName(const char* szVoiceTypeName, const char* szVoiceName);

    static const char* GetVoiceTypeNameFromID(short sVoiceTypeID);
    static const char* GetVoiceNameFromID(short sVoiceTypeID, short sVoiceID);
};
