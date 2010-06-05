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

short CPedSoundSA::GetVoiceTypeID ( )
{
    return m_pInterface->m_sVoiceType;
}

short CPedSoundSA::GetVoiceID ( )
{
    return m_pInterface->m_sVoiceID;    
}

void CPedSoundSA::SetVoiceTypeID ( short sVoiceType )
{
    m_pInterface->m_sVoiceType = sVoiceType;
}

void CPedSoundSA::SetVoiceID ( short sVoiceID )
{
    m_pInterface->m_sVoiceID = sVoiceID;
}

bool CPedSoundSA::IsSpeechDisabled ( void )
{
    bool bReturn;
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CAEPedSound__IsSpeedDisabled;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}


void CPedSoundSA::EnablePedSpeech ( void )
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CAEPedSound__EnablePedSpeech;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


void CPedSoundSA::DisablePedSpeech ( bool bStopCurrent )
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CAEPedSound__DisablePedSpeech;
    _asm
    {
        mov     ecx, dwThis
        push    bStopCurrent
        call    dwFunc
    }
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
