/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaAudioDefs.h
 *  PURPOSE:     Lua audio definitions class header
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <game/CAudioContainer.h>
#include <game/CAudioEngine.h>

class CLuaAudioDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Audio funcs
    static bool PlaySoundFrontEnd(std::uint8_t sound);
    static bool SetAmbientSoundEnabled(std::optional<eAmbientSoundType> type, bool enabled);
    static bool IsAmbientSoundEnabled(eAmbientSoundType type);
    static bool ResetAmbientSounds();
    static bool SetWorldSoundEnabled(int group, std::variant<int, bool> indexOrEnabled, std::optional<bool> enabled, std::optional<bool> immediate);
    static bool IsWorldSoundEnabled(int group, std::optional<int> index);
    static bool ResetWorldSounds();
    static std::variant<CClientSound*, bool> PlaySFX(lua_State* luaVM, eAudioLookupIndex containerIndex, std::variant<int, eRadioStreamIndex> bank,
                                                     int audioIndex, std::optional<bool> loop);
    static std::variant<CClientSound*, bool> PlaySFX3D(lua_State* luaVM, eAudioLookupIndex containerIndex, std::variant<int, eRadioStreamIndex> bank,
                                                       int audioIndex, CVector position, std::optional<bool> loop);
    static auto                              GetSFXStatus(eAudioLookupIndex containerIndex) noexcept;

    // Sound effects and synth functions
    static std::variant<CClientSound*, bool> PlaySound(lua_State* luaVM, const std::string path, std::optional<bool> loop, std::optional<bool> throttle);
    static std::variant<CClientSound*, bool> PlaySound3D(lua_State* luaVM, const std::string path, CVector position, std::optional<bool> loop,
                                                         std::optional<bool> throttle);
    static bool                              StopSound(CClientSound* sound);
    static bool                              SetSoundPosition(std::variant<CClientSound*, CClientPlayer*> sound, double position);
    static std::variant<double, bool>        GetSoundPosition(std::variant<CClientSound*, CClientPlayer*> sound);
    static std::variant<double, bool>        GetSoundLength(std::variant<CClientSound*, CClientPlayer*> sound);
    static std::variant<double, bool>        GetSoundBufferLength(CClientSound* sound);
    static bool                              SetSoundLooped(CClientSound* sound, bool loop);
    static bool                              IsSoundLooped(CClientSound* sound) noexcept;
    static bool                              SetSoundPaused(std::variant<CClientSound*, CClientPlayer*> sound, bool paused);
    static bool                              IsSoundPaused(std::variant<CClientSound*, CClientPlayer*> sound) noexcept;
    static bool                              SetSoundVolume(std::variant<CClientSound*, CClientPlayer*> sound, float volume);
    static std::variant<float, bool>         GetSoundVolume(std::variant<CClientSound*, CClientPlayer*> sound) noexcept;
    static bool                              SetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> sound, float speed);
    static std::variant<float, bool>         GetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> sound) noexcept;
    static bool                              SetSoundProperties(CClientSound* sound, float sampleRate, float tempo, float pitch, std::optional<bool> reversed);
    static auto                              GetSoundProperties(CClientSound* sound) noexcept;
    static auto                              GetSoundFFTData(std::variant<CClientSound*, CClientPlayer*> sound, int length, std::optional<int> bands);
    static auto                              GetSoundWaveData(std::variant<CClientSound*, CClientPlayer*> sound, int length);
    static auto                              GetSoundLevelData(std::variant<CClientSound*, CClientPlayer*> sound);
    static std::variant<float, bool>         GetSoundBPM(CClientSound* sound);
    static bool                              SetSoundPanEnabled(CClientSound* sound, bool enabled) noexcept;
    static bool                              IsSoundPanEnabled(CClientSound* sound) noexcept;
    static bool                              SetSoundMinDistance(CClientSound* sound, float distance) noexcept;
    static std::variant<float, bool>         GetSoundMinDistance(CClientSound* sound) noexcept;
    static bool                              SetSoundMaxDistance(CClientSound* sound, float distance);
    static std::variant<float, bool>         GetSoundMaxDistance(CClientSound* sound) noexcept;
    static auto                              GetSoundMetaTags(CClientSound* sound, std::optional<std::string> format);
    static bool SetSoundEffectEnabled(std::variant<CClientSound*, CClientPlayer*> sound, const std::string effectName, std::optional<bool> enable);
    static auto GetSoundEffects(std::variant<CClientSound*, CClientPlayer*> sound);
    static bool SetSoundEffectParameter(std::variant<CClientSound*, CClientPlayer*> sound, SoundEffectType::Enum effectType, std::string effectParameter,
                                        std::variant<float, bool> value);
    static auto GetSoundEffectParameters(std::variant<CClientSound*, CClientPlayer*> sound, SoundEffectType::Enum effectType);
    static bool SetSoundPan(std::variant<CClientSound*, CClientPlayer*> sound, float pan);
    static std::variant<float, bool> GetSoundPan(std::variant<CClientSound*, CClientPlayer*> sound);

    // Radio functions
    static bool                            SetRadioChannel(unsigned char channel);
    static auto                            GetRadioChannel() noexcept;
    static std::variant<const char*, bool> GetRadioChannelName(std::uint32_t channel) noexcept;

    static bool ShowSound(bool state) noexcept;
    static bool IsShowSoundEnabled() noexcept;
};
