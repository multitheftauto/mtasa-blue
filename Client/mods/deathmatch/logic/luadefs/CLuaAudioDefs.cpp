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

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <memory>
#include <ranges>
#include <stdexcept>

static bool IsValidFFTBandCount(int length, int bands) noexcept
{
    // BASS provides iLength / 2 spectrum values, so additional bands cannot be populated without reading beyond the FFT data.
    return bands >= 0 && bands <= length / 2;
}

static std::unique_ptr<float[]> ProcessFFTData(std::unique_ptr<float[]> data, int length, int bands)
{
    if (bands == 0 || data == nullptr)
        return data;

    auto newData = std::make_unique<float[]>(bands);
    int  bandCounter = 0;
    bands--;

    for (int x = 0; x <= bands; x++)
    {
        float peak = 0.0;

        double bandRange = std::pow(2.0, x * 10.0 / bands);

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

    return newData;
}

static bool IsSoundURL(const std::string& soundPath) noexcept
{
    const auto ToLower = [](char c) { return std::tolower(static_cast<unsigned char>(c)); };

    return (std::ranges::starts_with(soundPath, std::string_view{"http"}, {}, ToLower, ToLower) ||
            std::ranges::starts_with(soundPath, std::string_view{"ftp"}, {}, ToLower, ToLower)) &&
           (soundPath.length() <= 2048 || soundPath.find('\n') == std::string::npos);
}

static std::string SanitizeSoundPath(std::string_view path)
{
    constexpr std::size_t MAX_LOGGED_LENGTH = 256;

    std::string       result;
    const std::size_t length = std::min(path.size(), MAX_LOGGED_LENGTH);
    result.reserve(length + (path.size() > MAX_LOGGED_LENGTH ? 3 : 0));

    for (std::size_t i = 0; i < length; i++)
    {
        const unsigned char c = static_cast<unsigned char>(path[i]);
        result += (std::isprint(c) && c != '\n' && c != '\r' && c != '\t') ? static_cast<char>(c) : '?';
    }

    if (path.size() > MAX_LOGGED_LENGTH)
        result += "...";

    return result;
}

std::variant<CClientSound*, bool> CLuaAudioDefs::PlaySound(lua_State* luaVM, const std::string path, std::optional<bool> loop, std::optional<bool> throttle)
{
    CResource* resource = &lua_getownerresource(luaVM);

    std::string soundPath = path;
    std::string filename;
    bool        isURL = false;
    bool        isRawData = false;

    if (CResourceManager::ParseResourcePathInput(soundPath, resource, &filename, nullptr, true))
    {
        if (!FileExists(filename.c_str()))
        {
            throw LuaFunctionError(SString("Unable to load sound '%s'.", SanitizeSoundPath(path).c_str()), true);
        }

        soundPath = filename;
    }
    else
    {
        if (IsSoundURL(soundPath))
            isURL = true;
        else
            isRawData = true;
    }

    // ParseResourcePathInput changes resource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3"
    // ) Fixes #6507 - Caz
    if (resource)
    {
        CClientSound* sound = m_pManager->GetSoundManager()->PlaySound2D(soundPath, isURL, isRawData, loop.value_or(false), throttle.value_or(true));
        if (sound)
        {
            sound->SetParent(resource->GetResourceDynamicEntity());

            sound->SetLuaDebugInfo(m_pScriptDebugging->GetLuaDebugInfo(luaVM));

            // call onClientSoundStarted
            CLuaArguments Arguments;
            Arguments.PushString("play");  // Reason
            sound->CallEvent("onClientSoundStarted", Arguments, false);

            return sound;
        }
    }

    return false;
}

std::variant<CClientSound*, bool> CLuaAudioDefs::PlaySound3D(lua_State* luaVM, const std::string path, CVector position, std::optional<bool> loop,
                                                             std::optional<bool> throttle)
{
    CResource* resource = &lua_getownerresource(luaVM);

    std::string soundPath = path;
    std::string filename;
    bool        isURL = false;
    bool        isRawData = false;

    if (CResourceManager::ParseResourcePathInput(soundPath, resource, &filename, nullptr, true))
    {
        if (!FileExists(filename.c_str()))
        {
            throw LuaFunctionError(SString("Unable to load sound '%s'.", SanitizeSoundPath(path).c_str()), true);
        }

        soundPath = filename;
    }
    else
    {
        if (IsSoundURL(soundPath))
            isURL = true;
        else
            isRawData = true;
    }

    // ParseResourcePathInput changes resource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3"
    // ) Fixes #6507 - Caz
    if (resource)
    {
        CClientSound* sound = m_pManager->GetSoundManager()->PlaySound3D(soundPath, isURL, isRawData, position, loop.value_or(false), throttle.value_or(true));
        if (sound)
        {
            sound->SetParent(resource->GetResourceDynamicEntity());

            sound->SetLuaDebugInfo(m_pScriptDebugging->GetLuaDebugInfo(luaVM));

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
    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
        return (*soundElement)->SetPlayPosition(position);
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
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
    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
        return (*soundElement)->GetPlayPosition();
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
            return voice->GetPlayPosition();
    }

    return false;
}

std::variant<double, bool> CLuaAudioDefs::GetSoundLength(std::variant<CClientSound*, CClientPlayer*> sound)
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
        return (*soundElement)->GetLength();
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
            return voice->GetLength();
    }

    return false;
}

std::variant<double, bool> CLuaAudioDefs::GetSoundBufferLength(CClientSound* sound)
{
    return sound->IsSoundStream() ? sound->GetBufferLength() : false;
}

bool CLuaAudioDefs::SetSoundLooped(CClientSound* sound, bool loop)
{
    return sound->SetLooped(loop);
}

bool CLuaAudioDefs::IsSoundLooped(CClientSound* sound) noexcept
{
    return sound->IsLooped();
}

bool CLuaAudioDefs::SetSoundPaused(std::variant<CClientSound*, CClientPlayer*> sound, bool paused)
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
    {
        (*soundElement)->SetPaused(paused);
        return true;
    }
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
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

bool CLuaAudioDefs::IsSoundPaused(std::variant<CClientSound*, CClientPlayer*> sound) noexcept
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
        return (*soundElement)->IsPaused();
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
            return voice->IsPaused();
    }

    return false;
}

bool CLuaAudioDefs::SetSoundVolume(std::variant<CClientSound*, CClientPlayer*> sound, float volume)
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
    {
        (*soundElement)->SetVolume(volume);
        return true;
    }
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
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

std::variant<float, bool> CLuaAudioDefs::GetSoundVolume(std::variant<CClientSound*, CClientPlayer*> sound) noexcept
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
        return (*soundElement)->GetVolume();
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
            return voice->GetVolume();
    }

    return false;
}

bool CLuaAudioDefs::SetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> sound, float speed)
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
    {
        (*soundElement)->SetPlaybackSpeed(speed);
        return true;
    }
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
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

auto CLuaAudioDefs::GetSoundProperties(CClientSound* sound) noexcept
{
    float sampleRate = 0.0f, tempo = 0.0f, pitch = 0.0f;
    bool  reversed = false;

    sound->GetFXModifications(sampleRate, tempo, pitch, reversed);

    return CLuaMultiReturn<float, float, float, bool>{sampleRate, tempo, pitch, reversed};
}

auto CLuaAudioDefs::GetSoundFFTData(std::variant<CClientSound*, CClientPlayer*> sound, int length, std::optional<int> bands)
{
    using ResultType = std::variant<std::unordered_map<int, float>, bool>;

    const int numBands = bands.value_or(0);

    if (!IsValidFFTBandCount(length, numBands))
        return ResultType{false};

    std::unique_ptr<float[]> fftData;
    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
        fftData.reset((*soundElement)->GetFFTData(length));
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice && voice->IsActive())
            fftData.reset(voice->GetFFTData(length));
    }

    if (!fftData)
        return ResultType{false};

    fftData = ProcessFFTData(std::move(fftData), length, numBands);

    const int                      size = numBands == 0 ? length / 2 : numBands - 1;
    std::unordered_map<int, float> data;

    data.reserve(size + 1);

    for (int i = 0; i <= size; i++)
        data.emplace(i, fftData[i]);

    return ResultType{data};
}

auto CLuaAudioDefs::GetSoundWaveData(std::variant<CClientSound*, CClientPlayer*> sound, int length)
{
    using ResultType = std::variant<std::unordered_map<int, float>, bool>;

    float* waveData = nullptr;

    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
        waveData = (*soundElement)->GetWaveData(length);
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice && voice->IsActive())
            waveData = voice->GetWaveData(length);
    }

    if (!waveData)
        return ResultType{false};

    std::unordered_map<int, float> data;
    data.reserve(length);

    for (int i = 0; i < length; i++)
        data.emplace(i, waveData[i]);

    // Deallocate our data array here after it's used.
    delete[] waveData;
    return ResultType{data};
}

auto CLuaAudioDefs::GetSoundLevelData(std::variant<CClientSound*, CClientPlayer*> sound)
{
    using ResultType = std::variant<CLuaMultiReturn<std::uint32_t, std::uint32_t>, bool>;

    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
    {
        std::uint32_t levelData = (*soundElement)->GetLevelData();
        if (levelData != 0)
            return ResultType{CLuaMultiReturn<std::uint32_t, std::uint32_t>{LOWORD(levelData), HIWORD(levelData)}};
    }
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice && voice->IsActive())
        {
            std::uint32_t levelData = voice->GetLevelData();
            if (levelData != 0)
                return ResultType{CLuaMultiReturn<std::uint32_t, std::uint32_t>{LOWORD(levelData), HIWORD(levelData)}};
        }
    }

    return ResultType{false};
}

std::variant<float, bool> CLuaAudioDefs::GetSoundBPM(CClientSound* sound)
{
    float bpm = sound->GetSoundBPM();
    return bpm != 0.0f ? bpm : false;
}

bool CLuaAudioDefs::SetSoundPanEnabled(CClientSound* sound, bool enabled) noexcept
{
    return sound->SetPanEnabled(enabled);
}

bool CLuaAudioDefs::IsSoundPanEnabled(CClientSound* sound) noexcept
{
    return sound->IsPanEnabled();
}

std::variant<float, bool> CLuaAudioDefs::GetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> sound) noexcept
{
    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
        return (*soundElement)->GetPlaybackSpeed();
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
    {
        CClientPlayerVoice* voice = (*player)->GetVoice();
        if (voice)
            return voice->GetPlaybackSpeed();
    }

    return false;
}

bool CLuaAudioDefs::SetSoundMinDistance(CClientSound* sound, float distance) noexcept
{
    sound->SetMinDistance(distance);
    return true;
}

std::variant<float, bool> CLuaAudioDefs::GetSoundMinDistance(CClientSound* sound) noexcept
{
    return sound->GetMinDistance();
}

bool CLuaAudioDefs::SetSoundMaxDistance(CClientSound* sound, float distance)
{
    sound->SetMaxDistance(distance);
    return true;
}

std::variant<float, bool> CLuaAudioDefs::GetSoundMaxDistance(CClientSound* sound) noexcept
{
    return sound->GetMaxDistance();
}

auto CLuaAudioDefs::GetSoundMetaTags(CClientSound* sound, std::optional<std::string> format)
{
    using ResultType = std::variant<std::string, std::unordered_map<std::string, std::string>, bool>;

    if (format.has_value() && !format.value().empty())
    {
        std::string metaTags = sound->GetMetaTags(format.value());
        if (!metaTags.empty())
            return ResultType{metaTags};

        return ResultType{false};
    }

    std::unordered_map<std::string, std::string> tags;
    const auto                                   AddTag = [&](const char* tagFormat, const char* key)
    {
        std::string metaTags = sound->GetMetaTags(tagFormat);
        if (!metaTags.empty())
            tags.emplace(key, std::move(metaTags));
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

    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
        return fxEffect >= 0 && (*soundElement)->SetFxEffect(fxEffect, enable.value_or(false));
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
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
    const std::map<std::string, int>&     fxEffects = m_pManager->GetSoundManager()->GetFxEffects();

    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
    {
        for (const auto& [name, fxEffect] : fxEffects)
            result.emplace(name, (*soundElement)->IsFxEffectEnabled(fxEffect));

        return ResultType{result};
    }
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
    {
        CClientPlayerVoice* playerVoice = (*player)->GetVoice();

        if (!playerVoice)
            return ResultType{false};

        for (const auto& [name, fxEffect] : fxEffects)
            result.emplace(name, playerVoice->IsFxEffectEnabled(fxEffect));

        return ResultType{result};
    }

    return ResultType{false};
}

// This wrapper eliminates the need in additional methods inside CClientPlayer.
// It doesn't look right to put them there.
struct PlayerVoiceWrapper
{
    CClientPlayer* player{};

    bool IsFxEffectEnabled(std::uint32_t fxEffect)
    {
        CClientPlayerVoice* voice = player->GetVoice();
        return voice ? voice->IsFxEffectEnabled(fxEffect) : false;
    }

    bool SetFxEffectParameters(std::uint32_t fxEffect, void* params)
    {
        CClientPlayerVoice* voice = player->GetVoice();
        return voice ? voice->SetFxEffectParameters(fxEffect, params) : false;
    }

    bool GetFxEffectParameters(std::uint32_t fxEffect, void* params)
    {
        CClientPlayerVoice* voice = player->GetVoice();
        return voice ? voice->GetFxEffectParameters(fxEffect, params) : false;
    }
};

bool CLuaAudioDefs::SetSoundEffectParameter(std::variant<CClientSound*, CClientPlayer*> sound, SoundEffectType::Enum effectType, std::string effectParameter,
                                            std::variant<float, bool> value)
{
    //  bool setSoundEffectParameter ( sound/player sound, string effectName, string effectParameter, var effectParameterValue  )
    CClientSound*      soundElement = nullptr;
    PlayerVoiceWrapper playerVoice;

    if (auto* soundPtr = std::get_if<CClientSound*>(&sound))
        soundElement = *soundPtr;
    else if (auto* playerPtr = std::get_if<CClientPlayer*>(&sound))
        playerVoice.player = *playerPtr;

    // Call `SetFxEffectParameters` and log errors if any
    const auto SetParamWithErrorLog = [&effectType](auto* soundElement, auto effectParam, auto& params)
    {
        // Try setting parameter
        if (soundElement->SetFxEffectParameters((uint)effectType, &params))
            return true;

        // Unsuccessful, log error. (Hard error on usage mistakes)
        // `luaL_error` with a format string straight out crashes, so we have to do it this way..
        const SString msg("BASS Error %i, after setting parameter %s -> %s. (Message: %s)", CBassAudio::ErrorGetCode(), EnumToString(effectType).c_str(),
                          EnumToString(effectParam).c_str(), CBassAudio::ErrorGetMessage());

        // Do not use `luaL_error` here and pass in `msg` as the format string,
        // user could inject parameters into the format string, and that would be bad :D
        // The below code is based on the code from `luaL_error`
        throw std::invalid_argument(msg.c_str());
    };

    const auto ReadFloatValue = [&value](auto& out)
    {
        if (auto* valuePtr = std::get_if<float>(&value))
        {
            out = *valuePtr;
            return true;
        }

        return false;
    };

    const auto ProcessSoundParams = [&](auto* soundElement)
    {
        if (!soundElement->IsFxEffectEnabled((std::uint32_t)effectType))
            throw LuaFunctionError("Effect's parameters can't be set unless it's enabled");

        using namespace SoundEffectParams;
        switch (effectType)
        {
            case SoundEffectType::FX_DX8_CHORUS:
            {
                BASS_DX8_CHORUS params;
                soundElement->GetFxEffectParameters((std::uint32_t)effectType, &params);

                Chorus effectParameterType;
                if (!StringToEnum(effectParameter, effectParameterType))
                    return false;
                switch (effectParameterType)
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

                return SetParamWithErrorLog(soundElement, effectParameterType, params);
            }
            case SoundEffectType::FX_DX8_COMPRESSOR:
            {
                BASS_DX8_COMPRESSOR params;
                soundElement->GetFxEffectParameters((std::uint32_t)effectType, &params);

                Compressor effectParameterType;
                if (!StringToEnum(effectParameter, effectParameterType))
                    return false;
                switch (effectParameterType)
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

                return SetParamWithErrorLog(soundElement, effectParameterType, params);
            }
            case SoundEffectType::FX_DX8_DISTORTION:
            {
                BASS_DX8_DISTORTION params;
                soundElement->GetFxEffectParameters((std::uint32_t)effectType, &params);

                Distortion effectParameterType;
                if (!StringToEnum(effectParameter, effectParameterType))
                    return false;
                switch (effectParameterType)
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

                return SetParamWithErrorLog(soundElement, effectParameterType, params);
            }
            case SoundEffectType::FX_DX8_ECHO:
            {
                BASS_DX8_ECHO params;
                soundElement->GetFxEffectParameters((std::uint32_t)effectType, &params);

                Echo effectParameterType;
                if (!StringToEnum(effectParameter, effectParameterType))
                    return false;
                switch (effectParameterType)
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
                        if (auto* valuePtr = std::get_if<bool>(&value))
                            params.lPanDelay = *valuePtr;
                        else
                            return false;
                        break;
                    }
                }

                return SetParamWithErrorLog(soundElement, effectParameterType, params);
            }
            case SoundEffectType::FX_DX8_FLANGER:
            {
                BASS_DX8_FLANGER params;
                soundElement->GetFxEffectParameters((std::uint32_t)effectType, &params);

                Flanger effectParameterType;
                if (!StringToEnum(effectParameter, effectParameterType))
                    return false;
                switch (effectParameterType)
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

                return SetParamWithErrorLog(soundElement, effectParameterType, params);
            }
            case SoundEffectType::FX_DX8_GARGLE:
            {
                BASS_DX8_GARGLE params;
                soundElement->GetFxEffectParameters((std::uint32_t)effectType, &params);

                Gargle effectParameterType;
                if (!StringToEnum(effectParameter, effectParameterType))
                    return false;
                switch (effectParameterType)
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

                return SetParamWithErrorLog(soundElement, effectParameterType, params);
            }
            case SoundEffectType::FX_DX8_I3DL2REVERB:
            {
                BASS_DX8_I3DL2REVERB params;
                soundElement->GetFxEffectParameters((std::uint32_t)effectType, &params);

                I3DL2Reverb effectParameterType;
                if (!StringToEnum(effectParameter, effectParameterType))
                    return false;
                switch (effectParameterType)
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

                return SetParamWithErrorLog(soundElement, effectParameterType, params);
            }
            case SoundEffectType::FX_DX8_PARAMEQ:
            {
                BASS_DX8_PARAMEQ params;
                soundElement->GetFxEffectParameters((std::uint32_t)effectType, &params);

                ParamEq effectParameterType;
                if (!StringToEnum(effectParameter, effectParameterType))
                    return false;
                switch (effectParameterType)
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

                return SetParamWithErrorLog(soundElement, effectParameterType, params);
            }
            case SoundEffectType::FX_DX8_REVERB:
            {
                BASS_DX8_REVERB params;
                soundElement->GetFxEffectParameters((std::uint32_t)effectType, &params);

                Reverb effectParameterType;
                if (!StringToEnum(effectParameter, effectParameterType))
                    return false;
                switch (effectParameterType)
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

                return SetParamWithErrorLog(soundElement, effectParameterType, params);
            }
        }

        return false;
    };

    if (soundElement)
        return ProcessSoundParams(soundElement);
    else
        return ProcessSoundParams(&playerVoice);
}

auto CLuaAudioDefs::GetSoundEffectParameters(std::variant<CClientSound*, CClientPlayer*> sound, SoundEffectType::Enum effectType)
{
    //  table getSoundEffectParameters ( sound/player sound, string effectName )
    using ResultType = std::variant<std::unordered_map<std::string, std::variant<float, int, bool>>, bool>;

    CClientSound*      soundElement = nullptr;
    PlayerVoiceWrapper playerVoice;
    if (auto* soundPtr = std::get_if<CClientSound*>(&sound))
        soundElement = *soundPtr;
    else if (auto* playerPtr = std::get_if<CClientPlayer*>(&sound))
        playerVoice.player = *playerPtr;

    const auto ProcessSoundParams = [&](auto* soundElement) -> std::variant<std::unordered_map<std::string, std::variant<float, int, bool>>, bool>
    {
        if (!soundElement->IsFxEffectEnabled((std::uint32_t)effectType))
            throw LuaFunctionError("Effect's parameters can't be set unless it's enabled");

        using namespace SoundEffectParams;
        switch (effectType)
        {
            case SoundEffectType::FX_DX8_CHORUS:
            {
                BASS_DX8_CHORUS fxChorusParams;
                if (soundElement->GetFxEffectParameters((uint)effectType, &fxChorusParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(Chorus::WET_DRY_MIX), fxChorusParams.fWetDryMix},
                        {EnumToString(Chorus::DEPTH), fxChorusParams.fDepth},
                        {EnumToString(Chorus::FEEDBACK), fxChorusParams.fFeedback},
                        {EnumToString(Chorus::FREQUENCY), fxChorusParams.fFrequency},
                        {EnumToString(Chorus::WAVEFORM), static_cast<int>(fxChorusParams.lWaveform)},
                        {EnumToString(Chorus::DELAY), fxChorusParams.fDelay},
                        {EnumToString(Chorus::PHASE), static_cast<int>(fxChorusParams.lPhase)},
                    };
                }
                break;
            }
            case SoundEffectType::FX_DX8_COMPRESSOR:
            {
                BASS_DX8_COMPRESSOR fxCompressorParams;
                if (soundElement->GetFxEffectParameters((uint)effectType, &fxCompressorParams))
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
                if (soundElement->GetFxEffectParameters((uint)effectType, &fxDistortionParams))
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
                if (soundElement->GetFxEffectParameters((uint)effectType, &fxEchoParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(Echo::WET_DRY_MIX), fxEchoParams.fWetDryMix},
                        {EnumToString(Echo::FEEDBACK), fxEchoParams.fFeedback},
                        {EnumToString(Echo::LEFT_DELAY), fxEchoParams.fLeftDelay},
                        {EnumToString(Echo::RIGHT_DELAY), fxEchoParams.fRightDelay},
                        {EnumToString(Echo::PAN_DELAY), static_cast<bool>(fxEchoParams.lPanDelay)},
                    };
                }
                break;
            }
            case SoundEffectType::FX_DX8_FLANGER:
            {
                BASS_DX8_FLANGER fxFlangerParams;
                if (soundElement->GetFxEffectParameters((uint)effectType, &fxFlangerParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(Flanger::WET_DRY_MIX), fxFlangerParams.fWetDryMix},
                        {EnumToString(Flanger::DEPTH), fxFlangerParams.fDepth},
                        {EnumToString(Flanger::FEEDBACK), fxFlangerParams.fFeedback},
                        {EnumToString(Flanger::FREQUENCY), fxFlangerParams.fFrequency},
                        {EnumToString(Flanger::WAVEFORM), static_cast<int>(fxFlangerParams.lWaveform)},
                        {EnumToString(Flanger::DELAY), fxFlangerParams.fDelay},
                        {EnumToString(Flanger::PHASE), static_cast<int>(fxFlangerParams.lPhase)},
                    };
                }
                break;
            }
            case SoundEffectType::FX_DX8_GARGLE:
            {
                BASS_DX8_GARGLE fxGargleParams;
                if (soundElement->GetFxEffectParameters((uint)effectType, &fxGargleParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(Gargle::RATE_HZ), static_cast<int>(fxGargleParams.dwRateHz)},
                        {EnumToString(Gargle::WAVE_SHAPE), static_cast<int>(fxGargleParams.dwWaveShape)},
                    };
                }
                break;
            }
            case SoundEffectType::FX_DX8_I3DL2REVERB:
            {
                BASS_DX8_I3DL2REVERB fxI3DL2ReverbParams;
                if (soundElement->GetFxEffectParameters((uint)effectType, &fxI3DL2ReverbParams))
                {
                    return std::unordered_map<std::string, std::variant<float, int, bool>>{
                        {EnumToString(I3DL2Reverb::ROOM), static_cast<int>(fxI3DL2ReverbParams.lRoom)},
                        {EnumToString(I3DL2Reverb::ROOM_HF), static_cast<int>(fxI3DL2ReverbParams.lRoomHF)},
                        {EnumToString(I3DL2Reverb::ROOM_ROLLOFF_FACTOR), fxI3DL2ReverbParams.flRoomRolloffFactor},
                        {EnumToString(I3DL2Reverb::DECAY_TIME), fxI3DL2ReverbParams.flDecayTime},
                        {EnumToString(I3DL2Reverb::DECAY_HF_RATIO), fxI3DL2ReverbParams.flDecayHFRatio},
                        {EnumToString(I3DL2Reverb::REFLECTIONS), static_cast<int>(fxI3DL2ReverbParams.lReflections)},
                        {EnumToString(I3DL2Reverb::REFLECTIONS_DELAY), fxI3DL2ReverbParams.flReflectionsDelay},
                        {EnumToString(I3DL2Reverb::REVERB), static_cast<int>(fxI3DL2ReverbParams.lReverb)},
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
                if (soundElement->GetFxEffectParameters((uint)effectType, &fxParameqParams))
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
                if (soundElement->GetFxEffectParameters((uint)effectType, &fxReverbParams))
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

    if (soundElement)
        return ProcessSoundParams(soundElement);
    else
        return ProcessSoundParams(&playerVoice);
}

bool CLuaAudioDefs::PlaySoundFrontEnd(std::uint8_t sound)
{
    if (sound > 101)
        throw std::invalid_argument("Invalid sound ID specified. Valid sound IDs are 0 - 101.");

    g_pGame->GetAudioEngine()->PlayFrontEndSound(sound);

    return true;
}

bool CLuaAudioDefs::SetAmbientSoundEnabled(std::optional<eAmbientSoundType> type, bool enabled)
{
    g_pGame->GetAudioEngine()->SetAmbientSoundEnabled(type.value_or(AMBIENT_SOUND_GENERAL), enabled);
    return true;
}

bool CLuaAudioDefs::IsAmbientSoundEnabled(eAmbientSoundType type)
{
    return g_pGame->GetAudioEngine()->IsAmbientSoundEnabled(type);
}

bool CLuaAudioDefs::ResetAmbientSounds()
{
    g_pGame->GetAudioEngine()->ResetAmbientSounds();
    return true;
}

bool CLuaAudioDefs::SetWorldSoundEnabled(int group, std::variant<int, bool> indexOrEnabled, std::optional<bool> enabled, std::optional<bool> immediate)
{
    if (auto* index = std::get_if<int>(&indexOrEnabled))
    {
        g_pGame->GetAudioEngine()->SetWorldSoundEnabled(group, *index, enabled.value_or(false), immediate.value_or(false));
        return true;
    }

    // The second argument is the enabled state, so the third argument is the immediate flag
    g_pGame->GetAudioEngine()->SetWorldSoundEnabled(group, -1, std::get<bool>(indexOrEnabled), enabled.value_or(false));

    return true;
}

bool CLuaAudioDefs::IsWorldSoundEnabled(int group, std::optional<int> index)
{
    return g_pGame->GetAudioEngine()->IsWorldSoundEnabled(group, index.value_or(-1));
}

bool CLuaAudioDefs::ResetWorldSounds()
{
    g_pGame->GetAudioEngine()->ResetWorldSounds();
    return true;
}

std::variant<CClientSound*, bool> CLuaAudioDefs::PlaySFX(lua_State* luaVM, eAudioLookupIndex containerIndex, std::variant<int, eRadioStreamIndex> bank,
                                                         int audioIndex, std::optional<bool> loop)
{
    //  sound playSFX ( string audioContainer, int bankIndex, int audioIndex [, loop = false ] )
    int bankIndex;
    if (containerIndex == AUDIO_LOOKUP_RADIO)
    {
        if (auto* radioIndex = std::get_if<eRadioStreamIndex>(&bank))
            bankIndex = static_cast<int>(*radioIndex);
        else
            return false;
    }
    else if (auto* bankValue = std::get_if<int>(&bank))
        bankIndex = *bankValue;
    else
        return false;

    CResource* resource = &lua_getownerresource(luaVM);

    CClientSound* sound = m_pManager->GetSoundManager()->PlayGTASFX(containerIndex, bankIndex, audioIndex, loop.value_or(false));
    if (sound)
    {
        sound->SetParent(resource->GetResourceDynamicEntity());
        return sound;
    }

    return false;
}

std::variant<CClientSound*, bool> CLuaAudioDefs::PlaySFX3D(lua_State* luaVM, eAudioLookupIndex containerIndex, std::variant<int, eRadioStreamIndex> bank,
                                                           int audioIndex, CVector position, std::optional<bool> loop)
{
    //  sound playSFX3D ( string audioContainer, int bankIndex, int audioIndex, float posX, float posY, float posZ [, loop = false ] )
    int bankIndex;

    if (containerIndex == AUDIO_LOOKUP_RADIO)
    {
        if (auto* radioIndex = std::get_if<eRadioStreamIndex>(&bank))
            bankIndex = static_cast<int>(*radioIndex);
        else
            return false;
    }
    else if (auto* bankValue = std::get_if<int>(&bank))
        bankIndex = *bankValue;
    else
        return false;

    CResource* resource = &lua_getownerresource(luaVM);

    CClientSound* sound = m_pManager->GetSoundManager()->PlayGTASFX3D(containerIndex, bankIndex, audioIndex, position, loop.value_or(false));
    if (sound)
    {
        sound->SetParent(resource->GetResourceDynamicEntity());
        return sound;
    }

    return false;
}

auto CLuaAudioDefs::GetSFXStatus(eAudioLookupIndex containerIndex) noexcept
{
    //  bool getSFXStatus ( string audioContainer )
    return m_pManager->GetSoundManager()->GetSFXStatus(containerIndex);
}

bool CLuaAudioDefs::SetSoundPan(std::variant<CClientSound*, CClientPlayer*> sound, float pan)
{
    //  setSoundPan ( sound theSound, float pan )
    //  setSoundPan ( player thePlayer, float pan )
    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
        return (*soundElement)->SetPan(pan);
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
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
    if (auto* soundElement = std::get_if<CClientSound*>(&sound))
    {
        float pan = 0.0f;
        if ((*soundElement)->GetPan(pan))
            return pan;
    }
    else if (auto* player = std::get_if<CClientPlayer*>(&sound))
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

auto CLuaAudioDefs::GetRadioChannel() noexcept
{
    return m_pPlayerManager->GetLocalPlayer()->GetCurrentRadioChannel();
}

std::variant<const char*, bool> CLuaAudioDefs::GetRadioChannelName(std::uint32_t channel) noexcept
{
    static constexpr std::array<const char*, 13> radioStations = {{"Radio off", "Playback FM", "K-Rose", "K-DST", "Bounce FM", "SF-UR", "Radio Los Santos",
                                                                   "Radio X", "CSR 103.9", "K-Jah West", "Master Sounds 98.3", "WCTR", "User Track Player"}};

    if (static_cast<std::size_t>(channel) < radioStations.size())
        return radioStations[channel];

    return false;
}

bool CLuaAudioDefs::ShowSound(bool state) noexcept
{
    if (!g_pClientGame->GetDevelopmentMode())
        return false;

    g_pClientGame->SetShowSound(state);
    return true;
}

bool CLuaAudioDefs::IsShowSoundEnabled() noexcept
{
    return g_pClientGame->GetShowSound();
}

void CLuaAudioDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{// Audio funcs
                                                                             {"playSoundFrontEnd", ArgumentParserWarn<false, PlaySoundFrontEnd>},
                                                                             {"setAmbientSoundEnabled", ArgumentParserWarn<false, SetAmbientSoundEnabled>},
                                                                             {"isAmbientSoundEnabled", ArgumentParserWarn<false, IsAmbientSoundEnabled>},
                                                                             {"resetAmbientSounds", ArgumentParserWarn<false, ResetAmbientSounds>},
                                                                             {"setWorldSoundEnabled", ArgumentParserWarn<false, SetWorldSoundEnabled>},
                                                                             {"isWorldSoundEnabled", ArgumentParserWarn<false, IsWorldSoundEnabled>},
                                                                             {"resetWorldSounds", ArgumentParserWarn<false, ResetWorldSounds>},
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
