/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPedSoundSA.h
 *  PURPOSE:     Header file for ped sound class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPedSound.h>
#include "CAudioEngineSA.h"
#include "CAEVehicleAudioEntitySA.h"
#include "CAEWeaponAudioEntitySA.h"

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

enum ePedVoiceType : std::uint16_t
{
    PED_TYPE_GEN,
    PED_TYPE_EMG,
    PED_TYPE_PLAYER,
    PED_TYPE_GANG,
    PED_TYPE_GFD,
    PED_TYPE_SPC
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
    CAESound*     sounds[5];
    bool          isInitialised;
    ePedVoiceType m_sVoiceType;
    std::uint16_t m_sVoiceID;
    std::uint16_t m_bIsFemale;
    bool          m_bTalking;
    bool          m_bDisabled;
    bool          m_bDisabledSpeechForScripts;
    bool          m_bIsFrontend;
    bool          m_bIsForcedAudible;
    CAESound*     m_sound;
    std::int16_t  m_soundId;
    std::int16_t  m_bankId;
    std::int16_t  m_pedSpeechSlotIndex;
    float         m_fVoiceVolume;
    std::int16_t  m_sPhraseId;
    std::uint32_t m_nextTimeCanSay[19];
};
static_assert(sizeof(CPedSoundSAInterface) == 0x100, "Invalid size for CPedSoundSAInterface");

// CAEPedAudioEntity
class CPedSoundEntitySAInterface : public CAEAudioEntity
{
public:
    bool canAddEvent;

    std::uint8_t  field_7D;
    std::int16_t  sfxId;
    std::uint32_t timeInMS;

    float volume1;
    float volume2;
    float volume3;
    float jetpackSoundSpeedMult;

    CPedSAInterface* ped;

    bool      jetpackSoundPlaying;
    CAESound* jetpackSound1;
    CAESound* jetpackSound2;
    CAESound* jetpackSound3;

    CAETwinLoopSoundEntity twinLoopSoundEntity;
    CAESound*              field_150;

    std::uint8_t field_154[4];
    std::uint8_t field_158[4];
};
static_assert(sizeof(CPedSoundEntitySAInterface) == 0x15C, "Invalid size for CPedSoundEntitySAInterface");

// CAEPedWeaponAudioEntity
class CPedWeaponAudioEntitySAInterface : public CAEWeaponAudioEntitySAInterface
{
public:
    bool m_bIsInitialised;
    CPedSAInterface* m_ped;
};
static_assert(sizeof(CPedWeaponAudioEntitySAInterface) == 0xA8, "Invalid size for CPedWeaponAudioEntitySAInterface");

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
