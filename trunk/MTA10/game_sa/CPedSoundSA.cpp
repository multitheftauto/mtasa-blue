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
    (SPedVoiceName *)VAR_CAEPedSound__VoiceNames_GEN,
    (SPedVoiceName *)VAR_CAEPedSound__VoiceNames_EMG,
    (SPedVoiceName *)VAR_CAEPedSound__VoiceNames_PLAYER,
    (SPedVoiceName *)VAR_CAEPedSound__VoiceNames_GANG,
    (SPedVoiceName *)VAR_CAEPedSound__VoiceNames_GFD
};

CPedSoundSA::CPedSoundSA ( )
{
    m_pPedSoundInterface = NULL;
}

CPedSoundSA::CPedSoundSA ( CPedSoundSAInterface* pPedSoundInterface )
{
    m_pPedSoundInterface = pPedSoundInterface;
}

short CPedSoundSA::GetVoiceTypeID ( )
{
    if ( GetPedSoundInterface () )
        return GetPedSoundInterface ()->m_sVoiceType;
    else
        return -1;
}

short CPedSoundSA::GetVoiceID ( )
{
    if ( GetPedSoundInterface () )
        return GetPedSoundInterface ()->m_sVoiceID;
    else
        return -1;
}

void CPedSoundSA::SetVoiceTypeID ( short sVoiceType )
{
    if ( GetPedSoundInterface () )
        GetPedSoundInterface ()->m_sVoiceType = sVoiceType;
}

void CPedSoundSA::SetVoiceID ( short sVoiceID )
{
    if ( GetPedSoundInterface () )
        GetPedSoundInterface ()->m_sVoiceID = sVoiceID;
}

short CPedSoundSA::GetVoiceTypeIDFromName ( const char* szVoiceTypeName )
{
    DWORD dwFunc = (DWORD)FUNC_CAEPedSound__GetAudioPedType;
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

short CPedSoundSA::GetVoiceIDFromName ( short sVoiceTypeID, const char* szVoiceName )
{
    DWORD dwFunc = (DWORD)FUNC_CAEPedSound__GetVoice;
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

short CPedSoundSA::GetVoiceIDFromName ( const char* szVoiceTypeName, const char* szVoiceName )
{
    short sVoiceTypeID = GetVoiceTypeIDFromName ( szVoiceTypeName );
    if ( sVoiceTypeID < 0 )
        return -1;

    return GetVoiceIDFromName ( sVoiceTypeID, szVoiceName );
}

const char* CPedSoundSA::GetVoiceTypeNameFromID ( short sVoiceTypeID )
{
    if ( sVoiceTypeID < 0 || sVoiceTypeID >= NUM_PED_VOICE_TYPES )
        return NULL;

    return ( (const char**)VAR_CAEPedSound__VoiceTypeNames ) [ sVoiceTypeID ];
}

const char* CPedSoundSA::GetVoiceNameFromID ( short sVoiceTypeID, short sVoiceID )
{
    if ( sVoiceTypeID < 0 || sVoiceTypeID >= NUM_PED_VOICE_TYPES )
        return NULL;

    if ( sVoiceID < 0 || sVoiceID >= m_iNumVoicesPerType[sVoiceTypeID] )
        return NULL;

    return m_pVoiceNamesPerType [ sVoiceTypeID ][ sVoiceID ].szName;
}
