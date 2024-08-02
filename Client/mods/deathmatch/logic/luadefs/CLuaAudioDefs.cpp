/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaAudioDefs.cpp
 *  PURPOSE:     Lua audio definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>
#include "CBassAudio.h"

void CLuaAudioDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]
    {
        // Audio funcs
        {"playSoundFrontEnd", ArgumentParser<PlaySoundFrontEnd>},
        {"setAmbientSoundEnabled", ArgumentParser<SetAmbientSoundEnabled>},
        {"isAmbientSoundEnabled", ArgumentParser<IsAmbientSoundEnabled>},
        {"resetAmbientSounds", ArgumentParser<ResetAmbientSounds>},
        {"setWorldSoundEnabled", ArgumentParser<SetWorldSoundEnabled>},
        {"isWorldSoundEnabled", ArgumentParser<IsWorldSoundEnabled>},
        {"resetWorldSounds", ArgumentParser<ResetWorldSounds>},
        {"playSFX", ArgumentParser<PlaySFX>},
        {"playSFX3D", ArgumentParser<PlaySFX3D>},
        {"getSFXStatus", ArgumentParser<GetSFXStatus>},

        // Sound effects and synth funcs
        {"playSound", ArgumentParser<PlaySound>},
        {"playSound3D", ArgumentParser<PlaySound3D>},
        {"stopSound", ArgumentParser<StopSound>},
        {"setSoundPosition", ArgumentParser<SetSoundPosition>},
        {"getSoundPosition", ArgumentParser<GetSoundPosition>},
        {"getSoundLength", ArgumentParser<GetSoundLength>},
        {"getSoundBufferLength", ArgumentParser<GetSoundBufferLength>},
        {"setSoundLooped", ArgumentParser<SetSoundLooped>},
        {"isSoundLooped", ArgumentParser<IsSoundLooped>},
        {"setSoundPaused", ArgumentParser<SetSoundPaused>},
        {"isSoundPaused", ArgumentParser<IsSoundPaused>},
        {"setSoundVolume", ArgumentParser<SetSoundVolume>},
        {"getSoundVolume", ArgumentParser<GetSoundVolume>},
        {"setSoundSpeed", ArgumentParser<SetSoundSpeed>},
        {"getSoundSpeed", ArgumentParser<GetSoundSpeed>},
        {"setSoundProperties", ArgumentParser<SetSoundProperties>},
        {"getSoundProperties", ArgumentParser<GetSoundProperties>},
        {"getSoundFFTData", ArgumentParser<GetSoundFFTData>},
        {"getSoundWaveData", ArgumentParser<GetSoundWaveData>},
        {"getSoundLevelData", ArgumentParser<GetSoundLevelData>},
        {"setSoundPanningEnabled", ArgumentParser<SetSoundPanEnabled>},
        {"isSoundPanningEnabled", ArgumentParser<IsSoundPanningEnabled>},
        {"getSoundBPM", ArgumentParser<GetSoundBPM>},
        {"setSoundMinDistance", ArgumentParser<SetSoundMinDistance>},
        {"getSoundMinDistance", ArgumentParser<GetSoundMinDistance>},
        {"setSoundMaxDistance", ArgumentParser<SetSoundMaxDistance>},
        {"getSoundMaxDistance", ArgumentParser<GetSoundMaxDistance>},
        {"getSoundMetaTags", ArgumentParser<GetSoundMetaTags>},
        {"setSoundEffectEnabled", ArgumentParser<SetSoundEffectEnabled>},
        {"getSoundEffects", ArgumentParser<GetSoundEffects>},
        {"setSoundEffectParameter", ArgumentParser<SetSoundEffectParameter>},
        {"getSoundEffectParameters", ArgumentParser<GetSoundEffectParameters>},
        {"setSoundPan", ArgumentParser<SetSoundPan>},
        {"getSoundPan", ArgumentParser<GetSoundPan>},

        // Radio funcs
        {"setRadioChannel", ArgumentParser<SetRadioChannel>},
        {"getRadioChannel", ArgumentParser<GetRadioChannel>},
        {"getRadioChannelName", ArgumentParser<GetRadioChannelName>},

        // Dev funcs
        {"showSound", ArgumentParser<ShowSound>},
        {"isShowSoundEnabled", ArgumentParser<IsShowSoundEnabled>}
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaAudioDefs::AddClass(lua_State* luaVM)
{
    // 2D
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "playSound");
    lua_classfunction(luaVM, "playFrontEnd", "playSoundFrontEnd");
    lua_classfunction(luaVM, "stop", "stopSound");

    lua_classfunction(luaVM, "isPaused", "isSoundPaused");
    lua_classfunction(luaVM, "setEffectEnabled", "setSoundEffectEnabled");
    lua_classfunction(luaVM, "setEffectParameter", "setSoundEffectParameter");
    lua_classfunction(luaVM, "setPlaybackPosition", "setSoundPosition");
    lua_classfunction(luaVM, "setSpeed", "setSoundSpeed");
    lua_classfunction(luaVM, "setVolume", "setSoundVolume");
    lua_classfunction(luaVM, "setPaused", "setSoundPaused");
    lua_classfunction(luaVM, "setLooped", "setSoundLooped");
    lua_classfunction(luaVM, "setPan", "setSoundPan");
    lua_classfunction(luaVM, "setPanningEnabled", "setSoundPanningEnabled");
    lua_classfunction(luaVM, "setProperties", "setSoundProperties");

    lua_classfunction(luaVM, "getLength", "getSoundLength");
    lua_classfunction(luaVM, "getBufferLength", "getSoundBufferLength");
    lua_classfunction(luaVM, "isLooped", "isSoundLooped");
    lua_classfunction(luaVM, "getMetaTags", "getSoundMetaTags");
    lua_classfunction(luaVM, "getBPM", "getSoundBPM");
    lua_classfunction(luaVM, "getFFTData", "getSoundFFTData");
    lua_classfunction(luaVM, "getWaveData", "getSoundWaveData");
    lua_classfunction(luaVM, "getLevelData", "getSoundLevelData");
    lua_classfunction(luaVM, "getEffects", "getSoundEffects");
    lua_classfunction(luaVM, "getEffectParameters", "getSoundEffectParameters");
    lua_classfunction(luaVM, "getPlaybackPosition", "getSoundPosition");
    lua_classfunction(luaVM, "getSpeed", "getSoundSpeed");
    lua_classfunction(luaVM, "getVolume", "getSoundVolume");
    lua_classfunction(luaVM, "getPan", "getSoundPan");
    lua_classfunction(luaVM, "isPanningEnabled", "isSoundPanningEnabled");
    lua_classfunction(luaVM, "getProperties", "getSoundProperties");

    lua_classvariable(luaVM, "playbackPosition", "setSoundPosition", "getSoundPosition");
    lua_classvariable(luaVM, "speed", "setSoundSpeed", "getSoundSpeed");
    lua_classvariable(luaVM, "volume", "setSoundVolume", "getSoundVolume");
    lua_classvariable(luaVM, "looped", "setSoundLooped", "isSoundLooped");
    lua_classvariable(luaVM, "paused", "setSoundPaused", "isSoundPaused");
    lua_classvariable(luaVM, "pan", "setSoundPan", "getSoundPan");
    lua_classvariable(luaVM, "panningEnabled", "setSoundPanningEnabled", "isSoundPanningEnabled");
    lua_classvariable(luaVM, "length", nullptr, "getSoundLength");
    lua_classvariable(luaVM, "bufferLength", nullptr, "getSoundBufferLength");

    lua_registerclass(luaVM, "Sound", "Element");

    // 3D
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "playSound3D");

    lua_classfunction(luaVM, "getMaxDistance", "getSoundMaxDistance");
    lua_classfunction(luaVM, "getMinDistance", "getSoundMinDistance");

    lua_classfunction(luaVM, "setMaxDistance", "setSoundMaxDistance");
    lua_classfunction(luaVM, "setMinDistance", "setSoundMinDistance");

    lua_classvariable(luaVM, "maxDistance", "setSoundMaxDistance", "getSoundMaxDistance");
    lua_classvariable(luaVM, "minDistance", "setSoundMinDistance", "getSoundMinDistance");

    lua_registerclass(luaVM, "Sound3D", "Sound");
}

bool CLuaAudioDefs::PlaySoundFrontEnd(std::uint8_t sound)
{
    // bool playSoundFrontEnd ( int sound )
    if(sound > 101)
        throw std::invalid_argument("Invalid sound ID specified. Valid sound IDs are 0 - 101.");
    
    return CStaticFunctionDefinitions::PlaySoundFrontEnd(sound);
}

bool CLuaAudioDefs::SetAmbientSoundEnabled(eAmbientSoundType type, bool enabled) noexcept
{
    // bool setAmbientSoundEnabled( string theType, bool enable )
    return CStaticFunctionDefinitions::SetAmbientSoundEnabled(type, enabled);
}

bool CLuaAudioDefs::IsAmbientSoundEnabled(eAmbientSoundType type) noexcept
{
    // bool isAmbientSoundEnabled( string theType )
    return CStaticFunctionDefinitions::IsAmbientSoundEnabled(type);
}

bool CLuaAudioDefs::ResetAmbientSounds() noexcept
{
    // bool resetAmbientSounds()
    return CStaticFunctionDefinitions::ResetAmbientSounds();
}

bool CLuaAudioDefs::SetWorldSoundEnabled(int group, std::optional<int> index, bool enable, std::optional<bool> immediate) noexcept
{
    // bool setWorldSoundEnabled ( int group, [ int index = -1, ] bool enable [, bool immediate = false ] )
    return CStaticFunctionDefinitions::SetWorldSoundEnabled(
        group, index.value_or(-1), enable, immediate.value_or(false)
    );
}

bool CLuaAudioDefs::IsWorldSoundEnabled(int group, std::optional<int> index) noexcept
{
    // bool isWorldSoundEnabled ( int group, [ int index ] )
    return CStaticFunctionDefinitions::IsWorldSoundEnabled(group, index.value_or(-1));
}

bool CLuaAudioDefs::ResetWorldSounds() noexcept
{
    // bool resetWorldSounds()
    return CStaticFunctionDefinitions::ResetWorldSounds();
}

CClientSound* CLuaAudioDefs::PlaySFX(lua_State* luaVM, eAudioLookupIndex container, int bank, int audio, std::optional<bool> loop)
{
    // sound playSFX ( string audioContainer, int bankIndex, int audioIndex [, loop = false ] )    
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        throw std::invalid_argument("Cannot detect virtual machine!");
        
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        throw std::invalid_argument("Cannot get resource!");
    
    return CStaticFunctionDefinitions::PlaySFX(pResource, container, bank, audio,
        loop.value_or(false)
    );
}

CClientSound* CLuaAudioDefs::PlaySFX3D(lua_State* luaVM, eAudioLookupIndex container, int bank, int audio, float posX, float posY, float posZ,
                                       std::optional<bool> loop)
{
    // sound playSFX3D ( string audioContainer, int bankIndex, int audioIndex, float posX, float posY, float posZ [, loop = false ] )
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        throw std::invalid_argument("Cannot detect virtual machine!");
        
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        throw std::invalid_argument("Cannot get resource!");

    return CStaticFunctionDefinitions::PlaySFX3D(pResource, container, bank, audio,
        CVector(posX, posY, posZ), loop.value_or(false)
    );
}

bool CLuaAudioDefs::GetSFXStatus(eAudioLookupIndex container) noexcept
{
    // bool getSFXStatus ( string audioContainer )
    return CStaticFunctionDefinitions::GetSFXStatus(container);
}

CClientSound* CLuaAudioDefs::PlaySound(lua_State* luaVM, std::string path, std::optional<bool> loop, std::optional<bool> throttle)
{
    // element playSound ( string soundPath, [ bool looped = false, bool throttled = true ] )
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        throw std::invalid_argument("Cannot detect virtual machine!");

    CResource* pResource = luaMain->GetResource();
    if (!pResource)
        throw std::invalid_argument("Cannot get resource!");

    SString strFilename = path;
    bool bIsURL = false;
    bool bIsRawData = false;

    if (!CResourceManager::ParseResourcePathInput(path, pResource, &strFilename))
    {
        if ((!stricmp(strFilename.Left(4), "http") ||!stricmp(strFilename.Left(3), "ftp")) &&
            (strFilename.length() <= 2048 || strFilename.find('\n') == SString::npos))
            bIsURL = true;
        else
            bIsRawData = true;
    }

    // ParseResourcePathInput changes pResource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3"
    // ) Fixes #6507 - Caz
    // TODO: Fix ParseResourcePathInput
    if (!pResource)
        throw std::invalid_argument("Cannot get resource!");
        
    CClientSound* pSound = CStaticFunctionDefinitions::PlaySound(pResource, strFilename,
        bIsURL, bIsRawData, loop.value_or(false), throttle.value_or(true));

    if (!pSound)
        return nullptr;
        
    CLuaArguments Arguments;
    // Reason
    Arguments.PushString("play");
    // call onClientSoundStarted
    pSound->CallEvent("onClientSoundStarted", Arguments, false);

    return pSound;
}

CClientSound* CLuaAudioDefs::PlaySound3D(lua_State* luaVM, std::string path, float x, float y, float z, std::optional<bool> loop, std::optional<bool> throttle)
{
    // element playSound3D ( string soundPath, float x, float y, float z, [ bool looped = false, bool throttled = true ] )
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        throw std::invalid_argument("Cannot detect virtual machine!");

    CResource* pResource = luaMain->GetResource();
    if (!pResource)
        throw std::invalid_argument("Cannot get resource!");

    SString strFilename = path;
    bool bIsURL = false;
    bool bIsRawData = false;

    if (!CResourceManager::ParseResourcePathInput(path, pResource, &strFilename))
    {
        if ((!stricmp(strFilename.Left(4), "http") || !stricmp(strFilename.Left(3), "ftp")) &&
            (strFilename.length() <= 2048 || strFilename.find('\n') == SString::npos))
            bIsURL = true;
        else
            bIsRawData = true;
    }

    // ParseResourcePathInput changes pResource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3"
    // ) Fixes #6507 - Caz
    // TODO: Fix ParseResourcePathInput
    if (!pResource)
        throw std::invalid_argument("Cannot get resource!");
        
    CClientSound* pSound =
        CStaticFunctionDefinitions::PlaySound3D(pResource, strFilename,
        bIsURL, bIsRawData, CVector(x, y, z), loop.value_or(false), throttle.value_or(true));

    if (!pSound)
        return nullptr;
        
    CLuaArguments Arguments;
    // Reason
    Arguments.PushString("play");
    // call onClientSoundStarted
    pSound->CallEvent("onClientSoundStarted", Arguments, false);

    return pSound;
}

bool CLuaAudioDefs::StopSound(CClientSound* sound) noexcept
{
    // bool stopSound ( element theSound )
    return CStaticFunctionDefinitions::StopSound(*sound);
}

bool CLuaAudioDefs::SetSoundPosition(std::variant<CClientSound*, CClientPlayer*> element, double pos) noexcept
{
    // bool setSoundPosition ( element theSound, float pos )
    return CStaticFunctionDefinitions::SetSoundPosition(element, pos);
}

double CLuaAudioDefs::GetSoundPosition(std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    // float getSoundPosition ( element theSound )
    return CStaticFunctionDefinitions::GetSoundPosition(element);
}

double CLuaAudioDefs::GetSoundLength(std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    // float getSoundLength ( element theSound )
    return CStaticFunctionDefinitions::GetSoundLength(element);
}

double CLuaAudioDefs::GetSoundBufferLength(CClientSound* sound) noexcept
{
    // float getSoundBufferLength ( element theSound )
    return CStaticFunctionDefinitions::GetSoundBufferLength(*sound).value_or(0.0);
}

bool CLuaAudioDefs::SetSoundPaused(std::variant<CClientSound*, CClientPlayer*> element, bool paused) noexcept
{
    // bool setSoundPaused ( element theSound, bool paused )
    return CStaticFunctionDefinitions::SetSoundPaused(element, paused);
}

bool CLuaAudioDefs::IsSoundPaused(std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    // bool isSoundPaused ( element theSound )
    return CStaticFunctionDefinitions::IsSoundPaused(element);
}

bool CLuaAudioDefs::SetSoundVolume(std::variant<CClientSound*, CClientPlayer*> element, float volume) noexcept
{
    // bool setSoundVolume ( element theSound/thePlayer, float volume )
    return CStaticFunctionDefinitions::SetSoundVolume(element, volume);
}

float CLuaAudioDefs::GetSoundVolume(std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    // float getSoundVolume ( element theSound )
    return CStaticFunctionDefinitions::GetSoundVolume(element);
}

bool CLuaAudioDefs::SetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> element, float speed) noexcept
{
    // bool setSoundSpeed ( element theSound, float speed )
    return CStaticFunctionDefinitions::SetSoundSpeed(element, speed);
}

float CLuaAudioDefs::GetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    // float getSoundSpeed ( element theSound )
    return CStaticFunctionDefinitions::GetSoundSpeed(element);
}

bool CLuaAudioDefs::SetSoundProperties(CClientSound* sound, float sampleRate, float tempo, float pitch, std::optional<bool> reverse) noexcept
{
    // bool setSoundProperties(element sound, float fSampleRate, float fTempo, float fPitch [, bool bReverse = false ] )
    return CStaticFunctionDefinitions::SetSoundProperties(*sound, sampleRate,
        tempo, pitch, reverse.value_or(false)
    );
}

std::variant<bool, CLuaMultiReturn<float, float, float, bool>> CLuaAudioDefs::GetSoundProperties(CClientSound* sound) noexcept
{
    // float, float, float, bool getSoundProperties( element sound )
    float sampleRate;
    float tempo;
    float pitch;
    bool reversed;

    if (!CStaticFunctionDefinitions::GetSoundProperties(*sound, sampleRate, tempo, pitch, reversed))
        return false;
    
    return CLuaMultiReturn<float, float, float, bool>(sampleRate, tempo, pitch, reversed);
}

std::variant<bool, std::vector<float>> CLuaAudioDefs::GetSoundFFTData(std::variant<CClientSound*, CClientPlayer*> element, int samples,
                                                                      std::optional<int> bands) noexcept
{
    // table getSoundFFTData ( element sound, int samples [, int bands = 0 ] )
    int    iBands = bands.value_or(0);
    float* pData = CStaticFunctionDefinitions::GetSoundFFTData(element, samples, iBands);
    if (!pData)
        return false;
    
    std::vector<float> arr;

    for (auto i = 0; i <= (iBands ? iBands - 1 : samples / 2); i++)
    {
        arr.push_back(pData[i]);
    }

    delete[] pData;
    return arr;
}

std::variant<bool, std::vector<float>> CLuaAudioDefs::GetSoundWaveData(std::variant<CClientSound*, CClientPlayer*> element, int samples) noexcept
{
    // table getSoundWaveData ( element sound, int samples )
    float* pData = CStaticFunctionDefinitions::GetSoundWaveData(element, samples);
    if (!pData)
        return false;
    
    std::vector<float> arr;
    
    for (auto i = 0; i < samples; i++)
    {
        arr.push_back(pData[i]);
    }
    
    delete[] pData;
    return 1;
}

bool CLuaAudioDefs::SetSoundPanEnabled(CClientSound* sound, bool enable) noexcept
{
    // bool setSoundPanningEnabled ( element sound, bool enable )
    return CStaticFunctionDefinitions::SetSoundPanEnabled(*sound, enable);
}

std::variant<bool, CLuaMultiReturn<std::uint32_t, std::uint32_t>> CLuaAudioDefs::GetSoundLevelData(
    std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    // int, int getSoundLevelData ( element theSound )
    DWORD left;
    DWORD right;
    if (!CStaticFunctionDefinitions::GetSoundLevelData(element, left, right))
        return false;

    return CLuaMultiReturn<std::uint32_t, std::uint32_t>(left, right);
}

bool CLuaAudioDefs::IsSoundPanningEnabled(CClientSound* sound) noexcept
{
    // bool isSoundPanningEnabled ( element theSound )
    return CStaticFunctionDefinitions::IsSoundPanEnabled(*sound);
}

float CLuaAudioDefs::GetSoundBPM(CClientSound* sound) noexcept
{
    // int getSoundBPM ( element sound )
    return CStaticFunctionDefinitions::GetSoundBPM(*sound);
}

bool CLuaAudioDefs::SetSoundMinDistance(CClientSound* sound, float distance) noexcept
{
    // bool setSoundMinDistance ( element sound, int distance )
    return CStaticFunctionDefinitions::SetSoundMinDistance(*sound, distance);
}

float CLuaAudioDefs::GetSoundMinDistance(CClientSound* sound) noexcept
{
    // int getSoundMinDistance ( element sound )
    return CStaticFunctionDefinitions::GetSoundMinDistance(*sound);
}

bool CLuaAudioDefs::SetSoundMaxDistance(CClientSound* sound, float distance) noexcept
{
    // bool setSoundMaxDistance ( element sound, int distance )
    return CStaticFunctionDefinitions::SetSoundMaxDistance(*sound, distance);
}

float CLuaAudioDefs::GetSoundMaxDistance(CClientSound* sound) noexcept
{
    // int getSoundMaxDistance ( element sound )
    return CStaticFunctionDefinitions::GetSoundMaxDistance(*sound);
}

std::variant<bool, std::string, std::unordered_map<std::string, std::string>> CLuaAudioDefs::GetSoundMetaTags(CClientSound*              sound,
                                                                                                              std::optional<std::string> format) noexcept
{
    // table getSoundMetaTags ( element sound [, string format = "" ] )
    if (!sound)
        return false;
        
    SString strMetaTags = "";
    if (format.has_value())
    {
        if (!CStaticFunctionDefinitions::GetSoundMetaTags(*sound, format.value(), strMetaTags))
            return false;
            
        if (strMetaTags.empty())
            return false;
            
        return strMetaTags;
    }
    
    std::unordered_map<std::string, std::string> mapFormats;

    const auto& GetMetaTags = [&](const char* format, const char* name) {
        CStaticFunctionDefinitions::GetSoundMetaTags(*sound, format, strMetaTags);
        if (!strMetaTags.empty())
            mapFormats[name] = strMetaTags;
    };

    GetMetaTags("%TITL", "title");
    GetMetaTags("%ARTI", "artist");
    GetMetaTags("%ALBM", "album");
    GetMetaTags("%GNRE", "genre");
    GetMetaTags("%YEAR", "year");
    GetMetaTags("%CMNT", "comment");
    GetMetaTags("%TRCK", "track");
    GetMetaTags("%COMP", "composer");
    GetMetaTags("%COPY", "copyright");
    GetMetaTags("%SUBT", "subtitle");
    GetMetaTags("%AART", "album_artist");
    GetMetaTags("streamName", "stream_name");
    GetMetaTags("streamTitle", "stream_title");
    
    return mapFormats;
}

bool CLuaAudioDefs::SetSoundEffectEnabled(std::variant<CClientSound*, CClientPlayer*> element, std::string effectName, std::optional<bool> enable) noexcept
{
    // bool setSoundEffectEnabled ( element theSound/thePlayer, string effectName, bool bEnable )
    return CStaticFunctionDefinitions::SetSoundEffectEnabled(element, effectName, enable.value_or(false));
}

std::unordered_map<std::string, bool> CLuaAudioDefs::GetSoundEffects(std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    // table getSoundEffects ( element sound )
    std::map<std::string, int> fxEffects = m_pManager->GetSoundManager()->GetFxEffects();
    
    std::unordered_map<std::string, bool> arr;
    arr.reserve(fxEffects.size());
    
    for (const auto& [effectName, effect] : fxEffects)
    {
        if (std::holds_alternative<CClientSound*>(element)) {
            auto sound = std::get<CClientSound*>(element);
            arr[effectName] = sound->IsFxEffectEnabled(effect);
        } else if (std::holds_alternative<CClientPlayer*>(element)) {
            auto sound = std::get<CClientPlayer*>(element)->GetVoice();
            arr[effectName] = sound->IsFxEffectEnabled(effect);
        }
    }
    
    return arr;
}

bool CLuaAudioDefs::SetSoundEffectParameter(CClientSound* sound, eSoundEffectType effectType, std::string effectParam, CLuaArgument value)
{
    // bool setSoundEffectParameter ( element sound, string effectName, string effectParam, var paramValue )
    if (!sound || !sound->IsFxEffectEnabled(effectType))
        throw std::invalid_argument("Effect's parameters can't be set unless it's enabled");

    using namespace eSoundEffectParams;

    const auto SetParamWithErrorLog = [&](auto effectParam, auto& params)
    {
        // Try setting parameter
        if (sound->SetFxEffectParameters(effectType, &params))
            return true;

        // Unsuccessful, log error. (Hard error on usage mistakes)
        // 
        // Previous info about the `luaL_error` was invalid.
        // If the user was able to inject stuff into parameters via
        // `luaL_error` then he will be able to inject stuff
        // via `lua_error`. Why?
        // Because of this line (`SString(...)`).
        // Manually pushing values into the lua stack
        // and then calling `lua_error` is pointless.
        throw std::invalid_argument(
            SString("BASS Error %i, after setting parameter %s -> %s. (Message: %s)",
                CBassAudio::ErrorGetCode(), EnumToString(effectType).c_str(),
                EnumToString(effectParam).c_str(), CBassAudio::ErrorGetMessage()
            )
        );
    };

    switch (effectType)
    {
        case BASS_FX_DX8_CHORUS:
        {
            BASS_DX8_CHORUS params;
            sound->GetFxEffectParameters(effectType, &params);

            Chorus eEffectParameter;
            StringToEnum(effectParam, eEffectParameter);
            switch (eEffectParameter)
            {
                case Chorus::WET_DRY_MIX:
                {
                    params.fWetDryMix = static_cast<float>(value.GetNumber());
                    break;
                }
                case Chorus::DEPTH:
                {
                    params.fDepth = static_cast<float>(value.GetNumber());
                    break;
                }
                case Chorus::FEEDBACK:
                {
                    params.fFeedback = static_cast<float>(value.GetNumber());
                    break;
                }
                case Chorus::FREQUENCY:
                {
                    params.fFrequency = static_cast<float>(value.GetNumber());
                    break;
                }
                case Chorus::WAVEFORM:
                {
                    params.lWaveform = static_cast<DWORD>(value.GetNumber());
                    break;
                }
                case Chorus::DELAY:
                {
                    params.fDelay = static_cast<float>(value.GetNumber());
                    break;
                }
                case Chorus::PHASE:
                {
                    params.lPhase = static_cast<DWORD>(value.GetNumber());
                    break;
                }
            }

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_COMPRESSOR:
        {
            BASS_DX8_COMPRESSOR params;
            sound->GetFxEffectParameters(effectType, &params);

            Compressor eEffectParameter;
            StringToEnum(effectParam, eEffectParameter);
            switch (eEffectParameter)
            {
                case Compressor::GAIN:
                {
                    params.fGain = static_cast<float>(value.GetNumber());
                    break;
                }
                case Compressor::ATTACK:
                {
                    params.fAttack = static_cast<float>(value.GetNumber());
                    break;
                }
                case Compressor::RELEASE:
                {
                    params.fRelease = static_cast<float>(value.GetNumber());
                    break;
                }
                case Compressor::THRESHOLD:
                {
                    params.fThreshold = static_cast<float>(value.GetNumber());
                    break;
                }
                case Compressor::RATIO:
                {
                    params.fRatio = static_cast<float>(value.GetNumber());
                    break;
                }
                case Compressor::PREDELAY:
                {
                    params.fPredelay = static_cast<float>(value.GetNumber());
                    break;
                }
            }

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_DISTORTION:
        {
            BASS_DX8_DISTORTION params;
            sound->GetFxEffectParameters(effectType, &params);

            Distortion eEffectParameter;
            StringToEnum(effectParam, eEffectParameter);
            switch (eEffectParameter)
            {
                case Distortion::GAIN:
                {
                    params.fGain = static_cast<float>(value.GetNumber());
                    break;
                }
                case Distortion::EDGE:
                {
                    params.fEdge = static_cast<float>(value.GetNumber());
                    break;
                }
                case Distortion::POST_EQ_CENTER_FREQUENCY:
                {
                    params.fPostEQCenterFrequency = static_cast<float>(value.GetNumber());
                    break;
                }
                case Distortion::POST_EQ_BANDWIDTH:
                {
                    params.fPostEQBandwidth = static_cast<float>(value.GetNumber());
                    break;
                }
                case Distortion::PRE_LOWPASS_CUTOFF:
                {
                    params.fPreLowpassCutoff = static_cast<float>(value.GetNumber());
                    break;
                }
            }

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_ECHO:
        {
            BASS_DX8_ECHO params;
            sound->GetFxEffectParameters(effectType, &params);

            Echo eEffectParameter;
            StringToEnum(effectParam, eEffectParameter);
            switch (eEffectParameter)
            {
                case Echo::WET_DRY_MIX:
                {
                    params.fWetDryMix = static_cast<float>(value.GetNumber());
                    break;
                }
                case Echo::FEEDBACK:
                {
                    params.fFeedback = static_cast<float>(value.GetNumber());
                    break;
                }
                case Echo::LEFT_DELAY:
                {
                    params.fLeftDelay = static_cast<float>(value.GetNumber());
                    break;
                }
                case Echo::RIGHT_DELAY:
                {
                    params.fRightDelay = static_cast<float>(value.GetNumber());
                    break;
                }
                case Echo::PAN_DELAY:
                {
                    params.lPanDelay = value.GetBoolean();
                    break;
                }
            }

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_FLANGER:
        {
            BASS_DX8_FLANGER params;
            sound->GetFxEffectParameters(effectType, &params);

            Flanger eEffectParameter;
            StringToEnum(effectParam, eEffectParameter);
            switch (eEffectParameter)
            {
                case Flanger::WET_DRY_MIX:
                {
                    params.fWetDryMix = static_cast<float>(value.GetNumber());
                    break;
                }
                case Flanger::DEPTH:
                {
                    params.fDepth = static_cast<float>(value.GetNumber());
                    break;
                }
                case Flanger::FEEDBACK:
                {
                    params.fFeedback = static_cast<float>(value.GetNumber());
                    break;
                }
                case Flanger::FREQUENCY:
                {
                    params.fFrequency = static_cast<float>(value.GetNumber());
                    break;
                }
                case Flanger::WAVEFORM:
                {
                    params.lWaveform = static_cast<DWORD>(value.GetNumber());
                    break;
                }
                case Flanger::DELAY:
                {
                    params.fDelay = static_cast<float>(value.GetNumber());
                    break;
                }
                case Flanger::PHASE:
                {
                    params.lPhase = static_cast<DWORD>(value.GetNumber());
                    break;
                }
            }

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_GARGLE:
        {
            BASS_DX8_GARGLE params;
            sound->GetFxEffectParameters(effectType, &params);

            Gargle eEffectParameter;
            StringToEnum(effectParam, eEffectParameter);
            switch (eEffectParameter)
            {
                case Gargle::RATE_HZ:
                {
                    params.dwRateHz = static_cast<DWORD>(value.GetNumber());
                    break;
                }
                case Gargle::WAVE_SHAPE:
                {
                    params.dwWaveShape = static_cast<DWORD>(value.GetNumber());
                    break;
                }
            }

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_I3DL2REVERB:
        {
            BASS_DX8_I3DL2REVERB params;
            sound->GetFxEffectParameters(effectType, &params);

            I3DL2Reverb eEffectParameter;
            StringToEnum(effectParam, eEffectParameter);
            switch (eEffectParameter)
            {
                case I3DL2Reverb::ROOM:
                {
                    params.lRoom = static_cast<int>(value.GetNumber());
                    break;
                }
                case I3DL2Reverb::ROOM_HF:
                {
                    params.lRoomHF = static_cast<int>(value.GetNumber());
                    break;
                }
                case I3DL2Reverb::ROOM_ROLLOFF_FACTOR:
                {
                    params.flRoomRolloffFactor = static_cast<float>(value.GetNumber());
                    break;
                }
                case I3DL2Reverb::DECAY_TIME:
                {
                    params.flDecayTime = static_cast<float>(value.GetNumber());
                    break;
                }
                case I3DL2Reverb::DECAY_HF_RATIO:
                {
                    params.flDecayHFRatio = static_cast<float>(value.GetNumber());
                    break;
                }
                case I3DL2Reverb::REFLECTIONS:
                {
                    params.lReflections = static_cast<int>(value.GetNumber());
                    break;
                }
                case I3DL2Reverb::REFLECTIONS_DELAY:
                {
                    params.flReflectionsDelay = static_cast<float>(value.GetNumber());
                    break;
                }
                case I3DL2Reverb::REVERB:
                {
                    params.lReverb = static_cast<int>(value.GetNumber());
                    break;
                }
                case I3DL2Reverb::REVERB_DELAY:
                {
                    params.flReverbDelay = static_cast<float>(value.GetNumber());
                    break;
                }
                case I3DL2Reverb::DIFFUSION:
                {
                    params.flDiffusion = static_cast<float>(value.GetNumber());
                    break;
                }
                case I3DL2Reverb::DENSITY:
                {
                    params.flDensity = static_cast<float>(value.GetNumber());
                    break;
                }
                case I3DL2Reverb::HF_REFERENCE:
                {
                    params.flHFReference = static_cast<float>(value.GetNumber());
                    break;
                }
            }

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_PARAMEQ:
        {
            BASS_DX8_PARAMEQ params;
            sound->GetFxEffectParameters(effectType, &params);

            ParamEq eEffectParameter;
            StringToEnum(effectParam, eEffectParameter);
            switch (eEffectParameter)
            {
                case ParamEq::CENTER:
                {
                    params.fCenter = static_cast<float>(value.GetNumber());
                    break;
                }
                case ParamEq::BANDWIDTH:
                {
                    params.fBandwidth = static_cast<float>(value.GetNumber());
                    break;
                }
                case ParamEq::GAIN:
                {
                    params.fGain = static_cast<float>(value.GetNumber());
                    break;
                }
            }

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_REVERB:
        {
            BASS_DX8_REVERB params;
            sound->GetFxEffectParameters(effectType, &params);

            Reverb eEffectParameter;
            StringToEnum(effectParam, eEffectParameter);
            switch (eEffectParameter)
            {
                case Reverb::IN_GAIN:
                {
                    params.fInGain = static_cast<float>(value.GetNumber());
                    break;
                }
                case Reverb::REVERB_MIX:
                {
                    params.fReverbMix = static_cast<float>(value.GetNumber());
                    break;
                }
                case Reverb::REVERB_TIME:
                {
                    params.fReverbTime = static_cast<float>(value.GetNumber());
                    break;
                }
                case Reverb::HIGH_FREQ_RT_RATIO:
                {
                    params.fHighFreqRTRatio = static_cast<float>(value.GetNumber());
                    break;
                }
            }

            return SetParamWithErrorLog(eEffectParameter, params);
        }
    };
    throw std::invalid_argument("Cannot set effect parameter!");
}

std::variant<bool, std::unordered_map<std::string, CLuaArgument>> CLuaAudioDefs::GetSoundEffectParameters(CClientSound*    sound,
                                                                                                          eSoundEffectType effectType) noexcept
{
    // table getSoundEffectParameters ( sound sound, string effectName )
    if (!sound || !sound->IsFxEffectEnabled(effectType))
        return false;

    std::unordered_map<std::string, CLuaArgument> arr;

    using namespace eSoundEffectParams;

    CLuaArgument arg;

    switch (effectType)
    {
        case BASS_FX_DX8_CHORUS:
        {
            BASS_DX8_CHORUS fxChorusParams;
            if (!sound->GetFxEffectParameters(effectType, &fxChorusParams))
                break;

            arg.ReadNumber(fxChorusParams.fWetDryMix);
            arr[EnumToString(Chorus::WET_DRY_MIX)] = arg;
            arg.ReadNumber(fxChorusParams.fDepth);
            arr[EnumToString(Chorus::DEPTH)] = arg;
            arg.ReadNumber(fxChorusParams.fFeedback);
            arr[EnumToString(Chorus::FEEDBACK)] = arg;
            arg.ReadNumber(fxChorusParams.fFrequency);
            arr[EnumToString(Chorus::FREQUENCY)] = arg;
            arg.ReadNumber(fxChorusParams.lWaveform);
            arr[EnumToString(Chorus::WAVEFORM)] = arg;
            arg.ReadNumber(fxChorusParams.fDelay);
            arr[EnumToString(Chorus::DELAY)] = arg;
            arg.ReadNumber(fxChorusParams.lPhase);
            arr[EnumToString(Chorus::PHASE)] = arg;
            break;
        }
        case BASS_FX_DX8_COMPRESSOR:
        {
            BASS_DX8_COMPRESSOR fxCompressorParams;
            if (!sound->GetFxEffectParameters(effectType, &fxCompressorParams))
                break;

            arg.ReadNumber(fxCompressorParams.fGain);
            arr[EnumToString(Compressor::GAIN)] = arg;
            arg.ReadNumber(fxCompressorParams.fAttack);
            arr[EnumToString(Compressor::ATTACK)] = arg;
            arg.ReadNumber(fxCompressorParams.fRelease);
            arr[EnumToString(Compressor::RELEASE)] = arg;
            arg.ReadNumber(fxCompressorParams.fThreshold);
            arr[EnumToString(Compressor::THRESHOLD)] = arg;
            arg.ReadNumber(fxCompressorParams.fRatio);
            arr[EnumToString(Compressor::RATIO)] = arg;
            arg.ReadNumber(fxCompressorParams.fPredelay);
            arr[EnumToString(Compressor::PREDELAY)] = arg;
            break;
        }
        case BASS_FX_DX8_DISTORTION:
        {
            BASS_DX8_DISTORTION fxDistortionParams;
            if (!sound->GetFxEffectParameters(effectType, &fxDistortionParams))
                break;

            arg.ReadNumber(fxDistortionParams.fGain);
            arr[EnumToString(Distortion::GAIN)] = arg;
            arg.ReadNumber(fxDistortionParams.fEdge);
            arr[EnumToString(Distortion::EDGE)] = arg;
            arg.ReadNumber(fxDistortionParams.fPostEQCenterFrequency);
            arr[EnumToString(Distortion::POST_EQ_CENTER_FREQUENCY)] = arg;
            arg.ReadNumber(fxDistortionParams.fPostEQBandwidth);
            arr[EnumToString(Distortion::POST_EQ_BANDWIDTH)] = arg;
            arg.ReadNumber(fxDistortionParams.fPreLowpassCutoff);
            arr[EnumToString(Distortion::PRE_LOWPASS_CUTOFF)] = arg;
            break;
        }
        case BASS_FX_DX8_ECHO:
        {
            BASS_DX8_ECHO fxEchoParams;
            if (!sound->GetFxEffectParameters(effectType, &fxEchoParams))
                break;

            arg.ReadNumber(fxEchoParams.fWetDryMix);
            arr[EnumToString(Echo::WET_DRY_MIX)] = arg;
            arg.ReadNumber(fxEchoParams.fFeedback);
            arr[EnumToString(Echo::FEEDBACK)] = arg;
            arg.ReadNumber(fxEchoParams.fLeftDelay);
            arr[EnumToString(Echo::LEFT_DELAY)] = arg;
            arg.ReadNumber(fxEchoParams.fRightDelay);
            arr[EnumToString(Echo::RIGHT_DELAY)] = arg;
            arg.ReadBool(fxEchoParams.lPanDelay);
            arr[EnumToString(Echo::PAN_DELAY)] = arg;
            break;
        }
        case BASS_FX_DX8_FLANGER:
        {
            BASS_DX8_FLANGER fxFlangerParams;
            if (!sound->GetFxEffectParameters(effectType, &fxFlangerParams))
                break;

            arg.ReadNumber(fxFlangerParams.fWetDryMix);
            arr[EnumToString(Flanger::WET_DRY_MIX)] = arg;
            arg.ReadNumber(fxFlangerParams.fDepth);
            arr[EnumToString(Flanger::DEPTH)] = arg;
            arg.ReadNumber(fxFlangerParams.fFeedback);
            arr[EnumToString(Flanger::FEEDBACK)] = arg;
            arg.ReadNumber(fxFlangerParams.fFrequency);
            arr[EnumToString(Flanger::FREQUENCY)] = arg;
            arg.ReadNumber(fxFlangerParams.lWaveform);
            arr[EnumToString(Flanger::WAVEFORM)] = arg;
            arg.ReadNumber(fxFlangerParams.fDelay);
            arr[EnumToString(Flanger::DELAY)] = arg;
            arg.ReadNumber(fxFlangerParams.lPhase);
            arr[EnumToString(Flanger::PHASE)] = arg;
            break;
        }
        case BASS_FX_DX8_GARGLE:
        {
            BASS_DX8_GARGLE fxGargleParams;
            if (!sound->GetFxEffectParameters(effectType, &fxGargleParams))
                break;

            arg.ReadNumber(fxGargleParams.dwRateHz);
            arr[EnumToString(Gargle::RATE_HZ)] = arg;
            arg.ReadNumber(fxGargleParams.dwWaveShape);
            arr[EnumToString(Gargle::WAVE_SHAPE)] = arg;
            break;
        }
        case BASS_FX_DX8_I3DL2REVERB:
        {
            BASS_DX8_I3DL2REVERB fxI3DL2ReverbParams;
            if (!sound->GetFxEffectParameters(effectType, &fxI3DL2ReverbParams))
                break;

            arg.ReadNumber(fxI3DL2ReverbParams.lRoom);
            arr[EnumToString(I3DL2Reverb::ROOM)] = arg;
            arg.ReadNumber(fxI3DL2ReverbParams.lRoomHF);
            arr[EnumToString(I3DL2Reverb::ROOM_HF)] = arg;
            arg.ReadNumber(fxI3DL2ReverbParams.flRoomRolloffFactor);
            arr[EnumToString(I3DL2Reverb::ROOM_ROLLOFF_FACTOR)] = arg;
            arg.ReadNumber(fxI3DL2ReverbParams.flDecayTime);
            arr[EnumToString(I3DL2Reverb::DECAY_TIME)] = arg;
            arg.ReadNumber(fxI3DL2ReverbParams.flDecayHFRatio);
            arr[EnumToString(I3DL2Reverb::DECAY_HF_RATIO)] = arg;
            arg.ReadNumber(fxI3DL2ReverbParams.lReflections);
            arr[EnumToString(I3DL2Reverb::REFLECTIONS)] = arg;
            arg.ReadNumber(fxI3DL2ReverbParams.flReflectionsDelay);
            arr[EnumToString(I3DL2Reverb::REFLECTIONS_DELAY)] = arg;
            arg.ReadNumber(fxI3DL2ReverbParams.lReverb);
            arr[EnumToString(I3DL2Reverb::REVERB)] = arg;
            arg.ReadNumber(fxI3DL2ReverbParams.flReverbDelay);
            arr[EnumToString(I3DL2Reverb::REVERB_DELAY)] = arg;
            arg.ReadNumber(fxI3DL2ReverbParams.flDiffusion);
            arr[EnumToString(I3DL2Reverb::DIFFUSION)] = arg;
            arg.ReadNumber(fxI3DL2ReverbParams.flDensity);
            arr[EnumToString(I3DL2Reverb::DENSITY)] = arg;
            arg.ReadNumber(fxI3DL2ReverbParams.flHFReference);
            arr[EnumToString(I3DL2Reverb::HF_REFERENCE)] = arg;
            break;
        }
        case BASS_FX_DX8_PARAMEQ:
        {
            BASS_DX8_PARAMEQ fxParameqParams;
            if (!sound->GetFxEffectParameters(effectType, &fxParameqParams))
                break;

            arg.ReadNumber(fxParameqParams.fCenter);
            arr[EnumToString(ParamEq::CENTER)] = arg;
            arg.ReadNumber(fxParameqParams.fBandwidth);
            arr[EnumToString(ParamEq::BANDWIDTH)] = arg;
            arg.ReadNumber(fxParameqParams.fGain);
            arr[EnumToString(ParamEq::GAIN)] = arg;
            break;
        }
        case BASS_FX_DX8_REVERB:
        {
            BASS_DX8_REVERB fxReverbParams;
            if (!sound->GetFxEffectParameters(effectType, &fxReverbParams))
                break;

            arg.ReadNumber(fxReverbParams.fInGain);
            arr[EnumToString(Reverb::IN_GAIN)] = arg;
            arg.ReadNumber(fxReverbParams.fReverbMix);
            arr[EnumToString(Reverb::REVERB_MIX)] = arg;
            arg.ReadNumber(fxReverbParams.fReverbTime);
            arr[EnumToString(Reverb::REVERB_TIME)] = arg;
            arg.ReadNumber(fxReverbParams.fHighFreqRTRatio);
            arr[EnumToString(Reverb::HIGH_FREQ_RT_RATIO)] = arg;
            break;
        }
    }
    return arr;
}

bool CLuaAudioDefs::SetSoundPan(std::variant<CClientSound*, CClientPlayer*> element, float pan) noexcept
{
    // bool setSoundPan ( element theSound, float pan )
    return CStaticFunctionDefinitions::SetSoundPan(element, pan);
}

float CLuaAudioDefs::GetSoundPan(std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    // float getSoundPan ( element theSound )
    return CStaticFunctionDefinitions::GetSoundPan(element);
}

bool CLuaAudioDefs::SetSoundLooped(CClientSound* pSound, bool bLoop) noexcept
{
    // bool setSoundLooped ( element theSound, bool loop )
    return pSound->SetLooped(bLoop);
}

bool CLuaAudioDefs::IsSoundLooped(CClientSound* pSound) noexcept
{
    // bool isSoundLooped ( element theSound )
    return pSound->IsLooped();
}

bool CLuaAudioDefs::SetRadioChannel(std::uint8_t channel) noexcept
{
    // bool setRadioChannel ( int ID )
    return CStaticFunctionDefinitions::SetRadioChannel(channel);
}

std::uint8_t CLuaAudioDefs::GetRadioChannel() noexcept
{
    // int getRadioChannel ( )
    return CStaticFunctionDefinitions::GetRadioChannel();
}

std::variant<bool, std::string> CLuaAudioDefs::GetRadioChannelName(int channel) noexcept
{
    // string getRadioChannelName ( int id )
    static constexpr SFixedArray<const char*, 13> szRadioStations = {{
        "Radio off", "Playback FM", "K-Rose", "K-DST", "Bounce FM", "SF-UR", "Radio Los Santos",
        "Radio X", "CSR 103.9", "K-Jah West", "Master Sounds 98.3", "WCTR", "User Track Player"
    }};

    // TODO: Add .size() method to SFixedArray and call that method here
    if (channel < 0 || channel > NUMELMS(szRadioStations))
        return false;

    return szRadioStations[channel];
}

bool CLuaAudioDefs::ShowSound(bool state) noexcept
{
    // bool showSound ( bool state )
    if (!g_pClientGame->GetDevelopmentMode())
        return false;

    g_pClientGame->SetShowSound(state);
    return true;
}

bool CLuaAudioDefs::IsShowSoundEnabled() noexcept
{
    // bool isShowSoundEnabled ( )
    return g_pClientGame->GetShowSound();
}
