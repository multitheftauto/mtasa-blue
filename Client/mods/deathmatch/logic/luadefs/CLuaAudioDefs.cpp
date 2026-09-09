/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaAudioDefs.cpp
 *  PURPOSE:     Lua audio definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>
#include "CBassAudio.h"
#include <cmath>

static bool IsValidFFTBandCount(int length, int bands)
{
    return bands >= 0 && bands <= length / 2;
}

static float* ProcessFFTData(float* data, int length, int bands)
{
    if (bands == 0 || data == nullptr)
        return data;

    float* newData = new float[bands];
    int    bandCounter = 0;
    bands--;
    for (int x = 0; x <= bands; x++)
    {
        float peak = 0.0;

        double bandRange = pow(2, x * 10.0 / bands);

        if (bandRange > (length / 2) - 1)
            bandRange = (length / 2) - 1;

        if (bandRange <= bandCounter)
            bandRange = bandCounter + 1;

        while (bandCounter < bandRange)
        {
            if (peak < data[1 + bandCounter])
            {
                newData[x] = data[1 + bandCounter];
                peak = data[1 + bandCounter];
            }
            bandCounter = bandCounter + 1;
        }
    }
    delete[] data;
    return newData;
}

void CLuaAudioDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{// Audio funcs
                                                                             {"playSoundFrontEnd", ArgumentParserWarn<false, PlaySoundFrontEnd>},
                                                                             {"setAmbientSoundEnabled", SetAmbientSoundEnabled},
                                                                             {"isAmbientSoundEnabled", IsAmbientSoundEnabled},
                                                                             {"resetAmbientSounds", ResetAmbientSounds},
                                                                             {"setWorldSoundEnabled", SetWorldSoundEnabled},
                                                                             {"isWorldSoundEnabled", IsWorldSoundEnabled},
                                                                             {"resetWorldSounds", ResetWorldSounds},
                                                                             {"playSFX", ArgumentParserWarn<false, PlaySFX>},
                                                                             {"playSFX3D", ArgumentParserWarn<false, PlaySFX3D>},
                                                                             {"getSFXStatus", ArgumentParserWarn<nullptr, GetSFXStatus>},

                                                                             // Sound effects and synth funcs
                                                                             {"playSound", ArgumentParserWarn<false, PlaySound>},
                                                                             {"playSound3D", ArgumentParserWarn<false, PlaySound3D>},
                                                                             {"stopSound", ArgumentParserWarn<false, StopSound>},
                                                                             {"setSoundPosition", ArgumentParserWarn<false, SetSoundPosition>},
                                                                             {"getSoundPosition", ArgumentParserWarn<false, GetSoundPosition>},
                                                                             {"getSoundLength", ArgumentParserWarn<false, GetSoundLength>},
                                                                             {"getSoundBufferLength", ArgumentParserWarn<nullptr, GetSoundBufferLength>},
                                                                             {"setSoundLooped", ArgumentParser<SetSoundLooped>},
                                                                             {"isSoundLooped", ArgumentParser<IsSoundLooped>},
                                                                             {"setSoundPaused", ArgumentParserWarn<false, SetSoundPaused>},
                                                                             {"isSoundPaused", ArgumentParserWarn<false, IsSoundPaused>},
                                                                             {"setSoundVolume", ArgumentParserWarn<false, SetSoundVolume>},
                                                                             {"getSoundVolume", ArgumentParserWarn<false, GetSoundVolume>},
                                                                             {"setSoundSpeed", ArgumentParserWarn<false, SetSoundSpeed>},
                                                                             {"getSoundSpeed", ArgumentParserWarn<false, GetSoundSpeed>},
                                                                             {"setSoundProperties", ArgumentParserWarn<false, SetSoundProperties>},
                                                                             {"getSoundProperties", ArgumentParserWarn<false, GetSoundProperties>},
                                                                             {"getSoundFFTData", ArgumentParserWarn<false, GetSoundFFTData>},
                                                                             {"getSoundWaveData", ArgumentParserWarn<false, GetSoundWaveData>},
                                                                             {"getSoundLevelData", ArgumentParserWarn<false, GetSoundLevelData>},
                                                                             {"getSoundBPM", ArgumentParserWarn<false, GetSoundBPM>},
                                                                             {"setSoundPanningEnabled", ArgumentParserWarn<false, SetSoundPanEnabled>},
                                                                             {"isSoundPanningEnabled", ArgumentParserWarn<false, IsSoundPanEnabled>},
                                                                             {"setSoundMinDistance", ArgumentParserWarn<false, SetSoundMinDistance>},
                                                                             {"getSoundMinDistance", ArgumentParserWarn<false, GetSoundMinDistance>},
                                                                             {"setSoundMaxDistance", ArgumentParserWarn<false, SetSoundMaxDistance>},
                                                                             {"getSoundMaxDistance", ArgumentParserWarn<false, GetSoundMaxDistance>},
                                                                             {"getSoundMetaTags", ArgumentParserWarn<false, GetSoundMetaTags>},
                                                                             {"setSoundEffectEnabled", ArgumentParserWarn<false, SetSoundEffectEnabled>},
                                                                             {"getSoundEffects", ArgumentParserWarn<false, GetSoundEffects>},
                                                                             {"setSoundEffectParameter", ArgumentParserWarn<false, SetSoundEffectParameter>},
                                                                             {"getSoundEffectParameters", ArgumentParserWarn<false, GetSoundEffectParameters>},
                                                                             {"setSoundPan", ArgumentParserWarn<false, SetSoundPan>},
                                                                             {"getSoundPan", ArgumentParserWarn<false, GetSoundPan>},

                                                                             // Radio funcs
                                                                             {"setRadioChannel", ArgumentParserWarn<false, SetRadioChannel>},
                                                                             {"getRadioChannel", ArgumentParserWarn<false, GetRadioChannel>},
                                                                             {"getRadioChannelName", ArgumentParserWarn<false, GetRadioChannelName>},

                                                                             // Dev funcs
                                                                             {"showSound", ArgumentParser<ShowSound>},
                                                                             {"isShowSoundEnabled", ArgumentParser<IsShowSoundEnabled>}};

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
    lua_classvariable(luaVM, "length", NULL, "getSoundLength");
    lua_classvariable(luaVM, "bufferLength", NULL, "getSoundBufferLength");

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
}std::variant<CClientSound*, bool> CLuaAudioDefs::PlaySound(lua_State* luaVM, const std::string path, std::optional<bool> loop,
                                                           std::optional<bool> throttle)
{
    CResource* pResource = &lua_getownerresource(luaVM);

    std::string soundPath = path;
    std::string filename;
    bool        isURL = false;
    bool        isRawData = false;

    if (CResourceManager::ParseResourcePathInput(soundPath, pResource, &filename, nullptr, true))
        soundPath = filename;
    else
    {
        if ((stricmp(soundPath.substr(0, 4).c_str(), "http") == 0 || stricmp(soundPath.substr(0, 3).c_str(), "ftp") == 0) &&
            (soundPath.length() <= 2048 || soundPath.find('\n') == std::string::npos))
            isURL = true;
        else
            isRawData = true;
    }

    // ParseResourcePathInput changes pResource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3"
    // ) Fixes #6507 - Caz
    if (pResource)
    {
        CClientSound* sound = m_pManager->GetSoundManager()->PlaySound2D(soundPath, isURL, isRawData, loop.value_or(false), throttle.value_or(true));
        if (sound)
        {
            sound->SetParent(pResource->GetResourceDynamicEntity());

            // call onClientSoundStarted
            CLuaArguments Arguments;
            Arguments.PushString("play");  // Reason
            sound->CallEvent("onClientSoundStarted", Arguments, false);

            return sound;
        }
    }

    return false;
}

std::variant<CClientSound*, bool> CLuaAudioDefs::PlaySound3D(lua_State* luaVM, const std::string path, CVector vecPosition, std::optional<bool> loop,
                                                             std::optional<bool> throttle)
{
    CResource* pResource = &lua_getownerresource(luaVM);

    std::string soundPath = path;
    std::string filename;
    bool        isURL = false;
    bool        isRawData = false;

    if (CResourceManager::ParseResourcePathInput(soundPath, pResource, &filename, nullptr, true))
        soundPath = filename;
    else
    {
        if ((stricmp(soundPath.substr(0, 4).c_str(), "http") == 0 || stricmp(soundPath.substr(0, 3).c_str(), "ftp") == 0) &&
            (soundPath.length() <= 2048 || soundPath.find('\n') == std::string::npos))
            isURL = true;
        else
            isRawData = true;
    }

    // ParseResourcePathInput changes pResource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3"
    // ) Fixes #6507 - Caz
    if (pResource)
    {
        CClientSound* sound =
            m_pManager->GetSoundManager()->PlaySound3D(soundPath, isURL, isRawData, vecPosition, loop.value_or(false), throttle.value_or(true));
        if (sound)
        {
            sound->SetParent(pResource->GetResourceDynamicEntity());

            // call onClientSoundStarted
            CLuaArguments Arguments;
            Arguments.PushString("play");  // Reason
            sound->CallEvent("onClientSoundStarted", Arguments, false);

            return sound;
        }
    }

    return false;
}

bool CLuaAudioDefs::StopSound(CClientSound* sound)
{
    CLuaArguments Arguments;
    Arguments.PushString("destroyed");
    sound->CallEvent("onClientSoundStopped", Arguments, false);
    g_pClientGame->GetElementDeleter()->Delete(sound);
    return true;
}

bool CLuaAudioDefs::SetSoundPosition(std::variant<CClientSound*, CClientPlayer*> sound, double position)
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
        return (*soundElement)->SetPlayPosition(position);
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
        {
            voice->SetPlayPosition(position);
            return true;
        }
    }
    return false;
}

std::variant<double, bool> CLuaAudioDefs::GetSoundPosition(std::variant<CClientSound*, CClientPlayer*> sound)
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
        return (*soundElement)->GetPlayPosition();
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
            return voice->GetPlayPosition();
    }
    return false;
}

std::variant<double, bool> CLuaAudioDefs::GetSoundLength(std::variant<CClientSound*, CClientPlayer*> sound)
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
        return (*soundElement)->GetLength();
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
            return voice->GetLength();
    }
    return false;
}

std::variant<double, bool> CLuaAudioDefs::GetSoundBufferLength(CClientSound* sound)
{
    if (sound->IsSoundStream())
        return sound->GetBufferLength();
    return false;
}

bool CLuaAudioDefs::SetSoundLooped(CClientSound* sound, bool loop)
{
    return sound->SetLooped(loop);
}

bool CLuaAudioDefs::IsSoundLooped(CClientSound* sound)
{
    return sound->IsLooped();
}

bool CLuaAudioDefs::SetSoundPaused(std::variant<CClientSound*, CClientPlayer*> sound, bool paused)
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
    {
        (*soundElement)->SetPaused(paused);
        return true;
    }
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
        {
            voice->SetPaused(paused);
            return true;
        }
    }
    return false;
}

bool CLuaAudioDefs::IsSoundPaused(std::variant<CClientSound*, CClientPlayer*> sound)
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
        return (*soundElement)->IsPaused();
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
            return voice->IsPaused();
    }
    return false;
}

bool CLuaAudioDefs::SetSoundVolume(std::variant<CClientSound*, CClientPlayer*> sound, float volume)
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
    {
        (*soundElement)->SetVolume(volume);
        return true;
    }
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
        {
            voice->SetVolume(volume);
            return true;
        }
    }
    return false;
}

std::variant<float, bool> CLuaAudioDefs::GetSoundVolume(std::variant<CClientSound*, CClientPlayer*> sound)
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
        return (*soundElement)->GetVolume();
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
            return voice->GetVolume();
    }
    return false;
}

bool CLuaAudioDefs::SetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> sound, float speed)
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
    {
        (*soundElement)->SetPlaybackSpeed(speed);
        return true;
    }
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
        {
            voice->SetPlaybackSpeed(speed);
            return true;
        }
    }
    return false;
}

bool CLuaAudioDefs::SetSoundProperties(CClientSound* sound, float sampleRate, float tempo, float pitch, std::optional<bool> reversed)
{
    if (!sound->IsSoundStream())
    {
        sound->ApplyFXModifications(sampleRate, tempo, pitch, reversed.value_or(false));
        return true;
    }

    return false;
}

auto CLuaAudioDefs::GetSoundProperties(CClientSound* sound)
{
    float sampleRate = 0.0f, tempo = 0.0f, pitch = 0.0f;
    bool  reversed = false;
    sound->GetFXModifications(sampleRate, tempo, pitch, reversed);
    return CLuaMultiReturn<float, float, float, bool>{sampleRate, tempo, pitch, reversed};
}

auto CLuaAudioDefs::GetSoundFFTData(std::variant<CClientSound*, CClientPlayer*> sound, int length, std::optional<int> bands)
{
    using ResultType = std::variant<std::unordered_map<int, float>, bool>;

    if (!IsValidFFTBandCount(length, bands.value_or(0)))
        return ResultType{false};

    float* fftData = nullptr;
    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
        fftData = (*soundElement)->GetFFTData(length);
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice && voice->IsActive())
            fftData = voice->GetFFTData(length);
    }

    if (!fftData)
        return ResultType{false};

    fftData = ProcessFFTData(fftData, length, bands.value_or(0));

    const int                      size = bands.value_or(0) == 0 ? length / 2 : bands.value_or(0) - 1;
    std::unordered_map<int, float> data;
    for (int i = 0; i <= size; i++)
        data.emplace(i, fftData[i]);

    // Deallocate our data array here after it's used.
    delete[] fftData;
    return ResultType{data};
}

auto CLuaAudioDefs::GetSoundWaveData(std::variant<CClientSound*, CClientPlayer*> sound, int length)
{
    using ResultType = std::variant<std::unordered_map<int, float>, bool>;

    float* waveData = nullptr;
    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
        waveData = (*soundElement)->GetWaveData(length);
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice && voice->IsActive())
            waveData = voice->GetWaveData(length);
    }
    else
        return ResultType{false};

    if (!waveData)
        return ResultType{false};

    std::unordered_map<int, float> data;
    for (int i = 0; i < length; i++)
        data.emplace(i, waveData[i]);

    // Deallocate our data array here after it's used.
    delete[] waveData;
    return ResultType{data};
}

auto CLuaAudioDefs::GetSoundLevelData(std::variant<CClientSound*, CClientPlayer*> sound)
{
    using ResultType = std::variant<CLuaMultiReturn<std::uint32_t, std::uint32_t>, bool>;

    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
    {
        std::uint32_t levelData = (*soundElement)->GetLevelData();
        if (levelData != 0)
            return ResultType{CLuaMultiReturn<std::uint32_t, std::uint32_t>{levelData & 0xFFFF, levelData >> 16}};
    }
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice && voice->IsActive())
        {
            std::uint32_t levelData = voice->GetLevelData();
            if (levelData != 0)
                return ResultType{CLuaMultiReturn<std::uint32_t, std::uint32_t>{levelData & 0xFFFF, levelData >> 16}};
        }
    }
    return ResultType{false};
}

std::variant<float, bool> CLuaAudioDefs::GetSoundBPM(CClientSound* sound)
{
    float bpm = sound->GetSoundBPM();
    if (bpm != 0.0f)
        return bpm;
    return false;
}

bool CLuaAudioDefs::SetSoundPanEnabled(CClientSound* sound, bool enabled)
{
    return sound->SetPanEnabled(enabled);
}

bool CLuaAudioDefs::IsSoundPanEnabled(CClientSound* sound)
{
    return sound->IsPanEnabled();
}

std::variant<float, bool> CLuaAudioDefs::GetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> sound)
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
        return (*soundElement)->GetPlaybackSpeed();
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
            return voice->GetPlaybackSpeed();
    }
    return false;
}

bool CLuaAudioDefs::SetSoundMinDistance(CClientSound* sound, float distance)
{
    sound->SetMinDistance(distance);
    return true;
}

std::variant<float, bool> CLuaAudioDefs::GetSoundMinDistance(CClientSound* sound)
{
    return sound->GetMinDistance();
}

bool CLuaAudioDefs::SetSoundMaxDistance(CClientSound* sound, float distance)
{
    sound->SetMaxDistance(distance);
    return true;
}

std::variant<float, bool> CLuaAudioDefs::GetSoundMaxDistance(CClientSound* sound)
{
    return sound->GetMaxDistance();
}

auto CLuaAudioDefs::GetSoundMetaTags(CClientSound* sound, std::optional<std::string> format)
{
    using ResultType = std::variant<std::string, std::unordered_map<std::string, std::string>, bool>;

    if (format.has_value() && !format.value().empty())
    {
        std::string metaTags = sound->GetMetaTags(SString(format.value()));
        if (!metaTags.empty())
            return ResultType{metaTags};
        return ResultType{false};
    }

    std::unordered_map<std::string, std::string> tags;
    const auto                                   AddTag = [&](const char* tagFormat, const char* key)
    {
        std::string metaTags = sound->GetMetaTags(tagFormat);
        if (!metaTags.empty())
            tags.emplace(key, metaTags);
    };
    AddTag("%TITL", "title");
    AddTag("%ARTI", "artist");
    AddTag("%ALBM", "album");
    AddTag("%GNRE", "genre");
    AddTag("%YEAR", "year");
    AddTag("%CMNT", "comment");
    AddTag("%TRCK", "track");
    AddTag("%COMP", "composer");
    AddTag("%COPY", "copyright");
    AddTag("%SUBT", "subtitle");
    AddTag("%AART", "album_artist");
    AddTag("streamName", "stream_name");
    AddTag("streamTitle", "stream_title");
    return ResultType{tags};
}

bool CLuaAudioDefs::SetSoundEffectEnabled(std::variant<CClientSound*, CClientPlayer*> sound, const std::string effectName, std::optional<bool> enable)
{
    int fxEffect = m_pManager->GetSoundManager()->GetFxEffectFromName(effectName);

    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
        return fxEffect >= 0 && (*soundElement)->SetFxEffect(fxEffect, enable.value_or(false));
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        return voice && fxEffect >= 0 && voice->SetFxEffect(fxEffect, enable.value_or(false));
    }
    return false;
}

auto CLuaAudioDefs::GetSoundEffects(std::variant<CClientSound*, CClientPlayer*> sound)
{
    using ResultType = std::variant<std::unordered_map<std::string, bool>, bool>;

    std::unordered_map<std::string, bool> result;
    const std::map<std::string, int>      iFxEffects = m_pManager->GetSoundManager()->GetFxEffects();

    if (auto* pSound = std::get_if<CClientSound*>(&sound); pSound && *pSound)
    {
        for (const auto& [name, iFxEffect] : iFxEffects)
            result.emplace(name, (*pSound)->IsFxEffectEnabled(iFxEffect));
        return ResultType{result};
    }
    else if (auto* pPlayer = std::get_if<CClientPlayer*>(&sound); pPlayer && *pPlayer)
    {
        CClientPlayerVoice* pPlayerVoice = (*pPlayer)->GetVoice();
        if (!pPlayerVoice)
            return ResultType{false};
        for (const auto& [name, iFxEffect] : iFxEffects)
            result.emplace(name, pPlayerVoice->IsFxEffectEnabled(iFxEffect));
        return ResultType{result};
    }
    return ResultType{false};
}

// This wrapper eliminates the need in additional methods inside CClientPlayer.
// It doesn't look right to put them there.
struct SPlayerVoiceWrapper
{
    CClientPlayer* pPlayer{};

    bool IsFxEffectEnabled(std::uint32_t uiFxEffect)
    {
        CClientPlayerVoice* pVoice = pPlayer->GetVoice();
        return pVoice ? pVoice->IsFxEffectEnabled(uiFxEffect) : false;
    }

    bool SetFxEffectParameters(std::uint32_t uiFxEffect, void* params)
    {
        CClientPlayerVoice* pVoice = pPlayer->GetVoice();
        return pVoice ? pVoice->SetFxEffectParameters(uiFxEffect, params) : false;
    }
    bool GetFxEffectParameters(std::uint32_t uiFxEffect, void* params)
    {
        CClientPlayerVoice* pVoice = pPlayer->GetVoice();
        return pVoice ? pVoice->GetFxEffectParameters(uiFxEffect, params) : false;
    }
};

bool CLuaAudioDefs::SetSoundEffectParameter(std::variant<CClientSound*, CClientPlayer*> sound, SoundEffectType::Enum eEffectType,
                                            std::string strEffectParameter, std::variant<float, bool> value)
{
    //  bool setSoundEffectParameter ( sound/player sound, string effectName, string effectParameter, var effectParameterValue  )
    CClientSound*       pSound = nullptr;
    SPlayerVoiceWrapper playerVoice;
    if (auto* pSoundPtr = std::get_if<CClientSound*>(&sound); pSoundPtr && *pSoundPtr)
        pSound = *pSoundPtr;
    else if (auto* pPlayerPtr = std::get_if<CClientPlayer*>(&sound); pPlayerPtr && *pPlayerPtr)
        playerVoice.pPlayer = *pPlayerPtr;
    else
        return false;

    // Call `SetFxEffectParameters` and log errors if any
    const auto SetParamWithErrorLog = [&eEffectType](auto* pSound, auto effectParam, auto& params)
    {
        // Try setting parameter
        if (pSound->SetFxEffectParameters((uint)eEffectType, &params))
            return true;

        // Unsuccessful, log error. (Hard error on usage mistakes)
        // `luaL_error` with a format string straight out crashes, so we have to do it this way..
        const SString msg("BASS Error %i, after setting parameter %s -> %s. (Message: %s)", CBassAudio::ErrorGetCode(), EnumToString(eEffectType).c_str(),
                          EnumToString(effectParam).c_str(), CBassAudio::ErrorGetMessage());

        // Do not use `luaL_error` here and pass in `msg` as the format string,
        // user could inject parameters into the format string, and that would be bad :D
        // The below code is based on the code from `luaL_error`
        throw std::invalid_argument(msg.c_str());
    };

    const auto ReadFloatValue = [&value](auto& out)
    {
        if (auto* pValue = std::get_if<float>(&value))
        {
            out = *pValue;
            return true;
        }
        return false;
    };

    const auto ProcessSoundParams = [&](auto* pSound)
    {
        if (!pSound->IsFxEffectEnabled((std::uint32_t)eEffectType))
            throw LuaFunctionError("Effect's parameters can't be set unless it's enabled");

        using namespace SoundEffectParams;
        switch (eEffectType)
        {
            case SoundEffectType::FX_DX8_CHORUS:
            {
                BASS_DX8_CHORUS params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Chorus eEffectParameter;
                if (!StringToEnum(strEffectParameter, eEffectParameter))
                    return false;
                switch (eEffectParameter)
                {
                    case Chorus::WET_DRY_MIX:
                    {
                        if (!ReadFloatValue(params.fWetDryMix))
                            return false;
                        break;
                    }
                    case Chorus::DEPTH:
                    {
                        if (!ReadFloatValue(params.fDepth))
                            return false;
                        break;
                    }
                    case Chorus::FEEDBACK:
                    {
                        if (!ReadFloatValue(params.fFeedback))
                            return false;
                        break;
                    }
                    case Chorus::FREQUENCY:
                    {
                        if (!ReadFloatValue(params.fFrequency))
                            return false;
                        break;
                    }
                    case Chorus::WAVEFORM:
                    {
                        if (!ReadFloatValue(params.lWaveform))
                            return false;
                        break;
                    }
                    case Chorus::DELAY:
                    {
                        if (!ReadFloatValue(params.fDelay))
                            return false;
                        break;
                    }
                    case Chorus::PHASE:
                    {
                        if (!ReadFloatValue(params.lPhase))
                            return false;
                        break;
                    }
                }

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_COMPRESSOR:
            {
                BASS_DX8_COMPRESSOR params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Compressor eEffectParameter;
                if (!StringToEnum(strEffectParameter, eEffectParameter))
                    return false;
                switch (eEffectParameter)
                {
                    case Compressor::GAIN:
                    {
                        if (!ReadFloatValue(params.fGain))
                            return false;
                        break;
                    }
                    case Compressor::ATTACK:
                    {
                        if (!ReadFloatValue(params.fAttack))
                            return false;
                        break;
                    }
                    case Compressor::RELEASE:
                    {
                        if (!ReadFloatValue(params.fRelease))
                            return false;
                        break;
                    }
                    case Compressor::THRESHOLD:
                    {
                        if (!ReadFloatValue(params.fThreshold))
                            return false;
                        break;
                    }
                    case Compressor::RATIO:
                    {
                        if (!ReadFloatValue(params.fRatio))
                            return false;
                        break;
                    }
                    case Compressor::PREDELAY:
                    {
                        if (!ReadFloatValue(params.fPredelay))
                            return false;
                        break;
                    }
                }

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_DISTORTION:
            {
                BASS_DX8_DISTORTION params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Distortion eEffectParameter;
                if (!StringToEnum(strEffectParameter, eEffectParameter))
                    return false;
                switch (eEffectParameter)
                {
                    case Distortion::GAIN:
                    {
                        if (!ReadFloatValue(params.fGain))
                            return false;
                        break;
                    }
                    case Distortion::EDGE:
                    {
                        if (!ReadFloatValue(params.fEdge))
                            return false;
                        break;
                    }
                    case Distortion::POST_EQ_CENTER_FREQUENCY:
                    {
                        if (!ReadFloatValue(params.fPostEQCenterFrequency))
                            return false;
                        break;
                    }
                    case Distortion::POST_EQ_BANDWIDTH:
                    {
                        if (!ReadFloatValue(params.fPostEQBandwidth))
                            return false;
                        break;
                    }
                    case Distortion::PRE_LOWPASS_CUTOFF:
                    {
                        if (!ReadFloatValue(params.fPreLowpassCutoff))
                            return false;
                        break;
                    }
                }

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_ECHO:
            {
                BASS_DX8_ECHO params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Echo eEffectParameter;
                if (!StringToEnum(strEffectParameter, eEffectParameter))
                    return false;
                switch (eEffectParameter)
                {
                    case Echo::WET_DRY_MIX:
                    {
                        if (!ReadFloatValue(params.fWetDryMix))
                            return false;
                        break;
                    }
                    case Echo::FEEDBACK:
                    {
                        if (!ReadFloatValue(params.fFeedback))
                            return false;
                        break;
                    }
                    case Echo::LEFT_DELAY:
                    {
                        if (!ReadFloatValue(params.fLeftDelay))
                            return false;
                        break;
                    }
                    case Echo::RIGHT_DELAY:
                    {
                        if (!ReadFloatValue(params.fRightDelay))
                            return false;
                        break;
                    }
                    case Echo::PAN_DELAY:
                    {
                        if (auto* pValue = std::get_if<bool>(&value))
                            params.lPanDelay = *pValue;
                        else
                            return false;
                        break;
                    }
                }

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_FLANGER:
            {
                BASS_DX8_FLANGER params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Flanger eEffectParameter;
                if (!StringToEnum(strEffectParameter, eEffectParameter))
                    return false;
                switch (eEffectParameter)
                {
                    case Flanger::WET_DRY_MIX:
                    {
                        if (!ReadFloatValue(params.fWetDryMix))
                            return false;
                        break;
                    }
                    case Flanger::DEPTH:
                    {
                        if (!ReadFloatValue(params.fDepth))
                            return false;
                        break;
                    }
                    case Flanger::FEEDBACK:
                    {
                        if (!ReadFloatValue(params.fFeedback))
                            return false;
                        break;
                    }
                    case Flanger::FREQUENCY:
                    {
                        if (!ReadFloatValue(params.fFrequency))
                            return false;
                        break;
                    }
                    case Flanger::WAVEFORM:
                    {
                        if (!ReadFloatValue(params.lWaveform))
                            return false;
                        break;
                    }
                    case Flanger::DELAY:
                    {
                        if (!ReadFloatValue(params.fDelay))
                            return false;
                        break;
                    }
                    case Flanger::PHASE:
                    {
                        if (!ReadFloatValue(params.lPhase))
                            return false;
                        break;
                    }
                }

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_GARGLE:
            {
                BASS_DX8_GARGLE params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Gargle eEffectParameter;
                if (!StringToEnum(strEffectParameter, eEffectParameter))
                    return false;
                switch (eEffectParameter)
                {
                    case Gargle::RATE_HZ:
                    {
                        if (!ReadFloatValue(params.dwRateHz))
                            return false;
                        break;
                    }
                    case Gargle::WAVE_SHAPE:
                    {
                        if (!ReadFloatValue(params.dwWaveShape))
                            return false;
                        break;
                    }
                }

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_I3DL2REVERB:
            {
                BASS_DX8_I3DL2REVERB params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                I3DL2Reverb eEffectParameter;
                if (!StringToEnum(strEffectParameter, eEffectParameter))
                    return false;
                switch (eEffectParameter)
                {
                    case I3DL2Reverb::ROOM:
                    {
                        if (!ReadFloatValue(params.lRoom))
                            return false;
                        break;
                    }
                    case I3DL2Reverb::ROOM_HF:
                    {
                        if (!ReadFloatValue(params.lRoomHF))
                            return false;
                        break;
                    }
                    case I3DL2Reverb::ROOM_ROLLOFF_FACTOR:
                    {
                        if (!ReadFloatValue(params.flRoomRolloffFactor))
                            return false;
                        break;
                    }
                    case I3DL2Reverb::DECAY_TIME:
                    {
                        if (!ReadFloatValue(params.flDecayTime))
                            return false;
                        break;
                    }
                    case I3DL2Reverb::DECAY_HF_RATIO:
                    {
                        if (!ReadFloatValue(params.flDecayHFRatio))
                            return false;
                        break;
                    }
                    case I3DL2Reverb::REFLECTIONS:
                    {
                        if (!ReadFloatValue(params.lReflections))
                            return false;
                        break;
                    }
                    case I3DL2Reverb::REFLECTIONS_DELAY:
                    {
                        if (!ReadFloatValue(params.flReflectionsDelay))
                            return false;
                        break;
                    }
                    case I3DL2Reverb::REVERB:
                    {
                        if (!ReadFloatValue(params.lReverb))
                            return false;
                        break;
                    }
                    case I3DL2Reverb::REVERB_DELAY:
                    {
                        if (!ReadFloatValue(params.flReverbDelay))
                            return false;
                        break;
                    }
                    case I3DL2Reverb::DIFFUSION:
                    {
                        if (!ReadFloatValue(params.flDiffusion))
                            return false;
                        break;
                    }
                    case I3DL2Reverb::DENSITY:
                    {
                        if (!ReadFloatValue(params.flDensity))
                            return false;
                        break;
                    }
                    case I3DL2Reverb::HF_REFERENCE:
                    {
                        if (!ReadFloatValue(params.flHFReference))
                            return false;
                        break;
                    }
                }

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_PARAMEQ:
            {
                BASS_DX8_PARAMEQ params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                ParamEq eEffectParameter;
                if (!StringToEnum(strEffectParameter, eEffectParameter))
                    return false;
                switch (eEffectParameter)
                {
                    case ParamEq::CENTER:
                    {
                        if (!ReadFloatValue(params.fCenter))
                            return false;
                        break;
                    }
                    case ParamEq::BANDWIDTH:
                    {
                        if (!ReadFloatValue(params.fBandwidth))
                            return false;
                        break;
                    }
                    case ParamEq::GAIN:
                    {
                        if (!ReadFloatValue(params.fGain))
                            return false;
                        break;
                    }
                }

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_REVERB:
            {
                BASS_DX8_REVERB params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Reverb eEffectParameter;
                if (!StringToEnum(strEffectParameter, eEffectParameter))
                    return false;
                switch (eEffectParameter)
                {
                    case Reverb::IN_GAIN:
                    {
                        if (!ReadFloatValue(params.fInGain))
                            return false;
                        break;
                    }
                    case Reverb::REVERB_MIX:
                    {
                        if (!ReadFloatValue(params.fReverbMix))
                            return false;
                        break;
                    }
                    case Reverb::REVERB_TIME:
                    {
                        if (!ReadFloatValue(params.fReverbTime))
                            return false;
                        break;
                    }
                    case Reverb::HIGH_FREQ_RT_RATIO:
                    {
                        if (!ReadFloatValue(params.fHighFreqRTRatio))
                            return false;
                        break;
                    }
                }

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
        }

        return false;
    };

    if (pSound)
        return ProcessSoundParams(pSound);
    else
        return ProcessSoundParams(&playerVoice);
}

auto CLuaAudioDefs::GetSoundEffectParameters(std::variant<CClientSound*, CClientPlayer*> sound, SoundEffectType::Enum eEffectType)
{
    //  table getSoundEffectParameters ( sound/player sound, string effectName )
    using ResultType = std::variant<std::unordered_map<std::string, std::variant<float, int, bool>>, bool>;

    CClientSound*       pSound = nullptr;
    SPlayerVoiceWrapper playerVoice;
    if (auto* pSoundPtr = std::get_if<CClientSound*>(&sound); pSoundPtr && *pSoundPtr)
        pSound = *pSoundPtr;
    else if (auto* pPlayerPtr = std::get_if<CClientPlayer*>(&sound); pPlayerPtr && *pPlayerPtr)
        playerVoice.pPlayer = *pPlayerPtr;
    else
        return ResultType{false};

    const auto ProcessSoundParams = [&](auto* pSound) -> std::variant<std::unordered_map<std::string, std::variant<float, int, bool>>, bool>
    {
        if (!pSound->IsFxEffectEnabled((std::uint32_t)eEffectType))
            throw LuaFunctionError("Effect's parameters can't be set unless it's enabled");

        using namespace SoundEffectParams;
        switch (eEffectType)
        {
            case SoundEffectType::FX_DX8_CHORUS:
            {
                BASS_DX8_CHORUS fxChorusParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxChorusParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(Chorus::WET_DRY_MIX), fxChorusParams.fWetDryMix},  {EnumToString(Chorus::DEPTH), fxChorusParams.fDepth},
                        {EnumToString(Chorus::FEEDBACK), fxChorusParams.fFeedback},      {EnumToString(Chorus::FREQUENCY), fxChorusParams.fFrequency},
                        {EnumToString(Chorus::WAVEFORM), (int)fxChorusParams.lWaveform}, {EnumToString(Chorus::DELAY), fxChorusParams.fDelay},
                        {EnumToString(Chorus::PHASE), (int)fxChorusParams.lPhase},
                    };
                }
                break;
            }
            case SoundEffectType::FX_DX8_COMPRESSOR:
            {
                BASS_DX8_COMPRESSOR fxCompressorParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxCompressorParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(Compressor::GAIN), fxCompressorParams.fGain},       {EnumToString(Compressor::ATTACK), fxCompressorParams.fAttack},
                        {EnumToString(Compressor::RELEASE), fxCompressorParams.fRelease}, {EnumToString(Compressor::THRESHOLD), fxCompressorParams.fThreshold},
                        {EnumToString(Compressor::RATIO), fxCompressorParams.fRatio},     {EnumToString(Compressor::PREDELAY), fxCompressorParams.fPredelay},
                    };
                }
                break;
            }
            case SoundEffectType::FX_DX8_DISTORTION:
            {
                BASS_DX8_DISTORTION fxDistortionParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxDistortionParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(Distortion::GAIN), fxDistortionParams.fGain},
                        {EnumToString(Distortion::EDGE), fxDistortionParams.fEdge},
                        {EnumToString(Distortion::POST_EQ_CENTER_FREQUENCY), fxDistortionParams.fPostEQCenterFrequency},
                        {EnumToString(Distortion::POST_EQ_BANDWIDTH), fxDistortionParams.fPostEQBandwidth},
                        {EnumToString(Distortion::PRE_LOWPASS_CUTOFF), fxDistortionParams.fPreLowpassCutoff},
                    };
                }
                break;
            }
            case SoundEffectType::FX_DX8_ECHO:
            {
                BASS_DX8_ECHO fxEchoParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxEchoParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(Echo::WET_DRY_MIX), fxEchoParams.fWetDryMix},    {EnumToString(Echo::FEEDBACK), fxEchoParams.fFeedback},
                        {EnumToString(Echo::LEFT_DELAY), fxEchoParams.fLeftDelay},     {EnumToString(Echo::RIGHT_DELAY), fxEchoParams.fRightDelay},
                        {EnumToString(Echo::PAN_DELAY), (bool)fxEchoParams.lPanDelay},
                    };
                }
                break;
            }
            case SoundEffectType::FX_DX8_FLANGER:
            {
                BASS_DX8_FLANGER fxFlangerParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxFlangerParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(Flanger::WET_DRY_MIX), fxFlangerParams.fWetDryMix},  {EnumToString(Flanger::DEPTH), fxFlangerParams.fDepth},
                        {EnumToString(Flanger::FEEDBACK), fxFlangerParams.fFeedback},      {EnumToString(Flanger::FREQUENCY), fxFlangerParams.fFrequency},
                        {EnumToString(Flanger::WAVEFORM), (int)fxFlangerParams.lWaveform}, {EnumToString(Flanger::DELAY), fxFlangerParams.fDelay},
                        {EnumToString(Flanger::PHASE), (int)fxFlangerParams.lPhase},
                    };
                }
                break;
            }
            case SoundEffectType::FX_DX8_GARGLE:
            {
                BASS_DX8_GARGLE fxGargleParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxGargleParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(Gargle::RATE_HZ), (int)fxGargleParams.dwRateHz},
                        {EnumToString(Gargle::WAVE_SHAPE), (int)fxGargleParams.dwWaveShape},
                    };
                }
                break;
            }
            case SoundEffectType::FX_DX8_I3DL2REVERB:
            {
                BASS_DX8_I3DL2REVERB fxI3DL2ReverbParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxI3DL2ReverbParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(I3DL2Reverb::ROOM), (int)fxI3DL2ReverbParams.lRoom},
                        {EnumToString(I3DL2Reverb::ROOM_HF), (int)fxI3DL2ReverbParams.lRoomHF},
                        {EnumToString(I3DL2Reverb::ROOM_ROLLOFF_FACTOR), fxI3DL2ReverbParams.flRoomRolloffFactor},
                        {EnumToString(I3DL2Reverb::DECAY_TIME), fxI3DL2ReverbParams.flDecayTime},
                        {EnumToString(I3DL2Reverb::DECAY_HF_RATIO), fxI3DL2ReverbParams.flDecayHFRatio},
                        {EnumToString(I3DL2Reverb::REFLECTIONS), (int)fxI3DL2ReverbParams.lReflections},
                        {EnumToString(I3DL2Reverb::REFLECTIONS_DELAY), fxI3DL2ReverbParams.flReflectionsDelay},
                        {EnumToString(I3DL2Reverb::REVERB), (int)fxI3DL2ReverbParams.lReverb},
                        {EnumToString(I3DL2Reverb::REVERB_DELAY), fxI3DL2ReverbParams.flReverbDelay},
                        {EnumToString(I3DL2Reverb::DIFFUSION), fxI3DL2ReverbParams.flDiffusion},
                        {EnumToString(I3DL2Reverb::DENSITY), fxI3DL2ReverbParams.flDensity},
                        {EnumToString(I3DL2Reverb::HF_REFERENCE), fxI3DL2ReverbParams.flHFReference},
                    };
                }
                break;
            }
            case SoundEffectType::FX_DX8_PARAMEQ:
            {
                BASS_DX8_PARAMEQ fxParameqParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxParameqParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(ParamEq::CENTER), fxParameqParams.fCenter},
                        {EnumToString(ParamEq::BANDWIDTH), fxParameqParams.fBandwidth},
                        {EnumToString(ParamEq::GAIN), fxParameqParams.fGain},
                    };
                }
                break;
            }
            case SoundEffectType::FX_DX8_REVERB:
            {
                BASS_DX8_REVERB fxReverbParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxReverbParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(Reverb::IN_GAIN), fxReverbParams.fInGain},
                        {EnumToString(Reverb::REVERB_MIX), fxReverbParams.fReverbMix},
                        {EnumToString(Reverb::REVERB_TIME), fxReverbParams.fReverbTime},
                        {EnumToString(Reverb::HIGH_FREQ_RT_RATIO), fxReverbParams.fHighFreqRTRatio},
                    };
                }
                break;
            }
        }

        return false;
    };

    if (pSound)
        return ProcessSoundParams(pSound);
    else
        return ProcessSoundParams(&playerVoice);
}

bool CLuaAudioDefs::PlaySoundFrontEnd(unsigned char sound)
{
    if (sound > 101)
        throw std::invalid_argument("Invalid sound ID specified. Valid sound IDs are 0 - 101.");

    g_pGame->GetAudioEngine()->PlayFrontEndSound(sound);
    return true;
}

int CLuaAudioDefs::SetAmbientSoundEnabled(lua_State* luaVM)
{
    eAmbientSoundType eType;
    bool              bEnabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(eType, AMBIENT_SOUND_GENERAL);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetAmbientSoundEnabled(eType, bEnabled))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::IsAmbientSoundEnabled(lua_State* luaVM)
{
    eAmbientSoundType eType;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(eType);

    if (!argStream.HasErrors())
    {
        bool bResultEnabled;
        if (CStaticFunctionDefinitions::IsAmbientSoundEnabled(eType, bResultEnabled))
        {
            lua_pushboolean(luaVM, bResultEnabled);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::ResetAmbientSounds(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetAmbientSounds())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::SetWorldSoundEnabled(lua_State* luaVM)
{
    //  setWorldSoundEnabled ( int group, [int index, ], bool enable [, bool immediate = false ] )
    int  group;
    int  index = -1;
    bool bEnabled;
    bool bImmediate;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(group);
    if (!argStream.NextIsBool())
        argStream.ReadNumber(index);
    argStream.ReadBool(bEnabled);
    argStream.ReadBool(bImmediate, false);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetWorldSoundEnabled(group, index, bEnabled, bImmediate))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::IsWorldSoundEnabled(lua_State* luaVM)
{
    //  bool isWorldSoundEnabled ( int group, [int index] )
    int group;
    int index;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(group);
    argStream.ReadNumber(index, -1);

    if (!argStream.HasErrors())
    {
        bool bResultEnabled;
        if (CStaticFunctionDefinitions::IsWorldSoundEnabled(group, index, bResultEnabled))
        {
            lua_pushboolean(luaVM, bResultEnabled);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::ResetWorldSounds(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetWorldSounds())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    return 1;
}

std::variant<CClientSound*, bool> CLuaAudioDefs::PlaySFX(lua_State* luaVM, eAudioLookupIndex containerIndex, std::variant<int, eRadioStreamIndex> bank,
                                                         int audioIndex, std::optional<bool> loop)
{
    //  sound playSFX ( string audioContainer, int bankIndex, int audioIndex [, loop = false ] )
    int bankIndex;
    if (auto* bankValue = std::get_if<int>(&bank))
        bankIndex = *bankValue;
    else if (containerIndex == AUDIO_LOOKUP_RADIO)
        bankIndex = static_cast<int>(std::get<eRadioStreamIndex>(bank));
    else
        return false;

    CResource* pResource = &lua_getownerresource(luaVM);

    CClientSound* sound = m_pManager->GetSoundManager()->PlayGTASFX(containerIndex, bankIndex, audioIndex, loop.value_or(false));
    if (sound)
    {
        sound->SetParent(pResource->GetResourceDynamicEntity());
        return sound;
    }

    return false;
}

std::variant<CClientSound*, bool> CLuaAudioDefs::PlaySFX3D(lua_State* luaVM, eAudioLookupIndex containerIndex, std::variant<int, eRadioStreamIndex> bank,
                                                           int audioIndex, CVector vecPosition, std::optional<bool> loop)
{
    //  sound playSFX3D ( string audioContainer, int bankIndex, int audioIndex, float posX, float posY, float posZ [, loop = false ] )
    int bankIndex;
    if (auto* bankValue = std::get_if<int>(&bank))
        bankIndex = *bankValue;
    else if (containerIndex == AUDIO_LOOKUP_RADIO)
        bankIndex = static_cast<int>(std::get<eRadioStreamIndex>(bank));
    else
        return false;

    CResource* pResource = &lua_getownerresource(luaVM);

    CClientSound* sound = m_pManager->GetSoundManager()->PlayGTASFX3D(containerIndex, bankIndex, audioIndex, vecPosition, loop.value_or(false));
    if (sound)
    {
        sound->SetParent(pResource->GetResourceDynamicEntity());
        return sound;
    }

    return false;
}

auto CLuaAudioDefs::GetSFXStatus(eAudioLookupIndex containerIndex)
{
    //  bool getSFXStatus ( string audioContainer )
    return m_pManager->GetSoundManager()->GetSFXStatus(containerIndex);
}

bool CLuaAudioDefs::SetSoundPan(std::variant<CClientSound*, CClientPlayer*> sound, float pan)
{
    //  setSoundPan ( sound theSound, float pan )
    //  setSoundPan ( player thePlayer, float pan )
    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
        return (*soundElement)->SetPan(pan);
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        return voice && voice->SetPan(pan);
    }
    return false;
}

std::variant<float, bool> CLuaAudioDefs::GetSoundPan(std::variant<CClientSound*, CClientPlayer*> sound)
{
    //  getSoundPan ( element theSound )
    //  getSoundPan ( player thePlayer )
    if (auto* soundElement = std::get_if<CClientSound*>(&sound); soundElement && *soundElement)
    {
        float pan = 0.0f;
        if ((*soundElement)->GetPan(pan))
            return pan;
    }
    else if (auto* player = std::get_if<CClientPlayer*>(&sound); player && *player)
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
        {
            float pan = 0.0f;
            if (voice->GetPan(pan))
                return pan;
        }
    }
    return false;
}

// Radio
bool CLuaAudioDefs::SetRadioChannel(unsigned char channel)
{
    return m_pPlayerManager->GetLocalPlayer()->SetCurrentRadioChannel(channel);
}

auto CLuaAudioDefs::GetRadioChannel()
{
    return m_pPlayerManager->GetLocalPlayer()->GetCurrentRadioChannel();
}

std::variant<const char*, bool> CLuaAudioDefs::GetRadioChannelName(int channel)
{
    static const SFixedArray<const char*, 13> szRadioStations = {{"Radio off", "Playback FM", "K-Rose", "K-DST", "Bounce FM", "SF-UR", "Radio Los Santos",
                                                                  "Radio X", "CSR 103.9", "K-Jah West", "Master Sounds 98.3", "WCTR", "User Track Player"}};

    if (channel >= 0 && channel < NUMELMS(szRadioStations))
        return szRadioStations[channel];
    return false;
}

bool CLuaAudioDefs::ShowSound(bool state)
{
    if (!g_pClientGame->GetDevelopmentMode())
        return false;

    g_pClientGame->SetShowSound(state);
    return true;
}

bool CLuaAudioDefs::IsShowSoundEnabled()
{
    return g_pClientGame->GetShowSound();
}
