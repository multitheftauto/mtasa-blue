class CPedSoundSA;

#ifndef __CGAMESA_PEDSOUND
#define __CGAMESA_PEDSOUND

#define FUNC_CAEPedSpeechAudioEntity__GetVoice          0x4e3cd0        // 4E3CD0 ; public: static short __cdecl CAEPedSpeechAudioEntity::GetVoice(char *,short)
#define FUNC_CAEPedSpeechAudioEntity__GetAudioPedType   0x4e3c60        // 4E3C60 ; public: static short __cdecl CAEPedSpeechAudioEntity::GetAudioPedType(char *)

#define VAR_CAEPedSpeechAudioEntity__VoiceTypeNames     0x8C8108        // Array of 6 pointers to strings

#define VAR_CAEPedSpeechAudioEntity__VoiceNames_GEN     0x8AE6A8        // 20 bytes per voice name
#define VAR_CAEPedSpeechAudioEntity__VoiceNames_EMG     0x8BA0D8
#define VAR_CAEPedSpeechAudioEntity__VoiceNames_PLAYER  0x8BBD40
#define VAR_CAEPedSpeechAudioEntity__VoiceNames_GANG    0x8BE1A8
#define VAR_CAEPedSpeechAudioEntity__VoiceNames_GFD     0x8C4120

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
};

class CPedSoundSA
{
private:
    static int            m_iNumVoicesPerType[];
    static SPedVoiceName* m_pVoiceNamesPerType[];

public:
    static short          GetVoiceTypeID          ( const char* szVoiceTypeName );
    static short          GetVoiceID              ( short sVoiceTypeID, const char* szVoiceName );
    static short          GetVoiceID              ( const char* szVoiceTypeName, const char* szVoiceName );

    static const char*    GetVoiceTypeName        ( short sVoiceTypeID );
    static const char*    GetVoiceName            ( short sVoiceTypeID, short sVoiceID );
};

#endif