#include <StdInc.h>

int CPedSoundSA::m_iNumVoicesPerType[] = {NUM_GEN_VOICES, NUM_EMG_VOICES, NUM_PLAYER_VOICES, NUM_GANG_VOICES, NUM_GFD_VOICES};

SPedVoiceName* CPedSoundSA::m_pVoiceNamesPerType[] = {(SPedVoiceName*)VAR_CAEPedSound__VoiceNames_GEN, (SPedVoiceName*)VAR_CAEPedSound__VoiceNames_EMG,
                                                      (SPedVoiceName*)VAR_CAEPedSound__VoiceNames_PLAYER, (SPedVoiceName*)VAR_CAEPedSound__VoiceNames_GANG,
                                                      (SPedVoiceName*)VAR_CAEPedSound__VoiceNames_GFD};

short CPedSoundSA::GetVoiceTypeID()
{
    return m_pInterface->m_sVoiceType;
}

short CPedSoundSA::GetVoiceID()
{
    return m_pInterface->m_sVoiceID;
}

void CPedSoundSA::SetVoiceTypeID(short sVoiceType)
{
    m_pInterface->m_sVoiceType = sVoiceType;
}

void CPedSoundSA::SetVoiceID(short sVoiceID)
{
    m_pInterface->m_sVoiceID = sVoiceID;
}

bool CPedSoundSA::IsSpeechDisabled()
{
    // CPedSound::IsSpeechDisabled
    return ((bool(__thiscall*)(CPedSoundSAInterface*))FUNC_CAEPedSound__IsSpeedDisabled)(m_pInterface);
}

void CPedSoundSA::EnablePedSpeech()
{
    // CPedSound::EnablePedSpeech
    ((void(__thiscall*)(CPedSoundSAInterface*))FUNC_CAEPedSound__EnablePedSpeech)(m_pInterface);
}

void CPedSoundSA::DisablePedSpeech(bool bStopCurrent)
{
    // CPedSound::DisablePedSpeech
    ((void(__thiscall*)(CPedSoundSAInterface*, bool))FUNC_CAEPedSound__DisablePedSpeech)(m_pInterface, bStopCurrent);
}

short CPedSoundSA::GetVoiceTypeIDFromName(const char* szVoiceTypeName)
{
    // CAEPedSpeechAudioEntity::GetAudioPedType
    return ((short(__cdecl*)(const char*))FUNC_CAEPedSound__GetAudioPedType)(szVoiceTypeName);
}

short CPedSoundSA::GetVoiceIDFromName(short sVoiceTypeID, const char* szVoiceName)
{
    // CAEPedSpeechAudioEntity::GetVoice
    return ((short(__cdecl*)(const char*, short))FUNC_CAEPedSound__GetVoice)(szVoiceName, sVoiceTypeID);
}

short CPedSoundSA::GetVoiceIDFromName(const char* szVoiceTypeName, const char* szVoiceName)
{
    short sVoiceTypeID = GetVoiceTypeIDFromName(szVoiceTypeName);
    if (sVoiceTypeID < 0)
        return -1;

    return GetVoiceIDFromName(sVoiceTypeID, szVoiceName);
}

const char* CPedSoundSA::GetVoiceTypeNameFromID(short sVoiceTypeID)
{
    if (sVoiceTypeID < 0 || sVoiceTypeID >= NUM_PED_VOICE_TYPES)
        return NULL;

    return ((const char**)VAR_CAEPedSound__VoiceTypeNames)[sVoiceTypeID];
}

const char* CPedSoundSA::GetVoiceNameFromID(short sVoiceTypeID, short sVoiceID)
{
    if (sVoiceTypeID < 0 || sVoiceTypeID >= NUM_PED_VOICE_TYPES)
        return NULL;

    if (sVoiceID < 0 || sVoiceID >= m_iNumVoicesPerType[sVoiceTypeID])
        return NULL;

    return m_pVoiceNamesPerType[sVoiceTypeID][sVoiceID].szName;
}
