#include <StdInc.h>

int CPedSoundSA::m_iNumVoicesPerType[] =
{
    NUM_GEN_VOICES,
    NUM_EMG_VOICES,
    NUM_PLAYER_VOICES,
    NUM_GANG_VOICES,
    NUM_GFD_VOICES
};

SPedVoiceName* CPedSoundSA::m_pVoiceNamesPerType[] =
{
    (SPedVoiceName *)VAR_CAEPedSpeechAudioEntity__VoiceNames_GEN,
    (SPedVoiceName *)VAR_CAEPedSpeechAudioEntity__VoiceNames_EMG,
    (SPedVoiceName *)VAR_CAEPedSpeechAudioEntity__VoiceNames_PLAYER,
    (SPedVoiceName *)VAR_CAEPedSpeechAudioEntity__VoiceNames_GANG,
    (SPedVoiceName *)VAR_CAEPedSpeechAudioEntity__VoiceNames_GFD
};

short CPedSoundSA::GetVoiceTypeID ( const char* szVoiceTypeName )
{
    DWORD dwFunc = (DWORD)FUNC_CAEPedSpeechAudioEntity__GetAudioPedType;
    short sVoiceTypeID;

    _asm
    {
        push    szVoiceTypeName
        call    dwFunc
        add     esp, 4
        mov     sVoiceTypeID, ax
    }
    return sVoiceTypeID;
}

short CPedSoundSA::GetVoiceID ( short sVoiceTypeID, const char* szVoiceName )
{
    DWORD dwFunc = (DWORD)FUNC_CAEPedSpeechAudioEntity__GetVoice;
    short sVoiceID;
    
    _asm
    {
        movzx eax, sVoiceTypeID
        push    eax
        push    szVoiceName
        call    dwFunc
        add		esp, 8
        mov     sVoiceID, ax
    }
    return sVoiceID;
}

short CPedSoundSA::GetVoiceID ( const char* szVoiceTypeName, const char* szVoiceName )
{
    short sVoiceTypeID = GetVoiceTypeID ( szVoiceTypeName );
    if ( sVoiceTypeID < 0 )
        return -1;

    return GetVoiceID ( sVoiceTypeID, szVoiceName );
}

const char* CPedSoundSA::GetVoiceTypeName ( short sVoiceTypeID )
{
    if ( sVoiceTypeID < 0 || sVoiceTypeID >= NUM_PED_VOICE_TYPES )
        return NULL;

    return ( (const char**)VAR_CAEPedSpeechAudioEntity__VoiceTypeNames ) [ sVoiceTypeID ];
}

const char* CPedSoundSA::GetVoiceName ( short sVoiceTypeID, short sVoiceID )
{
    if ( sVoiceTypeID < 0 || sVoiceTypeID >= NUM_PED_VOICE_TYPES )
        return NULL;

    if ( sVoiceID < 0 || sVoiceID >= m_iNumVoicesPerType[sVoiceTypeID] )
        return NULL;

    return m_pVoiceNamesPerType [ sVoiceTypeID ][ sVoiceID ].szName;
}
