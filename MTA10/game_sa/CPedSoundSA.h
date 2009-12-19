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

#ifndef __CGAMESA_PEDSOUND
#define __CGAMESA_PEDSOUND

#include <game/CPedSound.h>

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

class CPedSoundSAInterface
{
public:
    BYTE ucPad1[0x92];
    short m_sVoiceType;
    short m_sVoiceID;
    short m_bIsFemale;
    BYTE ucPad2[1];
    bool m_bDisabled;
};

class CPedSoundSA : public CPedSound
{
private:
    static int              m_iNumVoicesPerType[];
    static SPedVoiceName*   m_pVoiceNamesPerType[];

    CPedSoundSAInterface*   m_pInterface;

public:
                            CPedSoundSA                 ( CPedSoundSAInterface* pInterface )    { m_pInterface = pInterface; }
    CPedSoundSAInterface*   GetInterface                ( void )    { return m_pInterface; }

    short                   GetVoiceTypeID              ( void );
    short                   GetVoiceID                  ( void );
    void                    SetVoiceTypeID              ( short sVoiceType );
    void                    SetVoiceID                  ( short sVoiceID );
    
    bool                    IsSpeechDisabled            ( void );
    void                    EnablePedSpeech             ( void );
    void                    DisablePedSpeech            ( bool bStopCurrent );

    static short            GetVoiceTypeIDFromName      ( const char* szVoiceTypeName );
    static short            GetVoiceIDFromName          ( short sVoiceTypeID, const char* szVoiceName );
    static short            GetVoiceIDFromName          ( const char* szVoiceTypeName, const char* szVoiceName );

    static const char*      GetVoiceTypeNameFromID      ( short sVoiceTypeID );
    static const char*      GetVoiceNameFromID          ( short sVoiceTypeID, short sVoiceID );
};

#endif