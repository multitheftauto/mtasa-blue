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

class CLuaAudioDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Audio funcs
    static bool PlaySoundFrontEnd(unsigned char sound);
    LUA_DECLARE(SetAmbientSoundEnabled);
    LUA_DECLARE(IsAmbientSoundEnabled);
    LUA_DECLARE(ResetAmbientSounds);
    LUA_DECLARE(SetWorldSoundEnabled);
    LUA_DECLARE(IsWorldSoundEnabled);
    LUA_DECLARE(ResetWorldSounds);
    static std::variant<CClientSound*, bool>  PlaySFX(lua_State* luaVM, eAudioLookupIndex containerIndex, std::variant<int, eRadioStreamIndex> bank,
                                                      int audioIndex, std::optional<bool> loop);
    static std::variant<CClientSound*, bool>  PlaySFX3D(lua_State* luaVM, eAudioLookupIndex containerIndex, std::variant<int, eRadioStreamIndex> bank,
                                                        int audioIndex, CVector vecPosition, std::optional<bool> loop);
    static auto GetSFXStatus(eAudioLookupIndex containerIndex);

    // Sound effects and synth functions
    static std::variant<CClientSound*, bool> PlaySound(lua_State* luaVM, const std::string path, std::optional<bool> loop, std::optional<bool> throttle);
    static std::variant<CClientSound*, bool> PlaySound3D(lua_State* luaVM, const std::string path, CVector vecPosition, std::optional<bool> loop,
                                                         std::optional<bool> throttle);
    static bool                              StopSound(CClientSound* sound);
    static bool                              SetSoundPosition(std::variant<CClientSound*, CClientPlayer*> sound, double position);
    static std::variant<double, bool>        GetSoundPosition(std::variant<CClientSound*, CClientPlayer*> sound);
    static std::variant<double, bool>        GetSoundLength(std::variant<CClientSound*, CClientPlayer*> sound);
    static std::variant<double, bool>        GetSoundBufferLength(CClientSound* sound);
    static bool                              SetSoundLooped(CClientSound* sound, bool loop);
    static bool                              IsSoundLooped(CClientSound* sound);
    static bool                              SetSoundPaused(std::variant<CClientSound*, CClientPlayer*> sound, bool paused);
    static bool                              IsSoundPaused(std::variant<CClientSound*, CClientPlayer*> sound);
    static bool                              SetSoundVolume(std::variant<CClientSound*, CClientPlayer*> sound, float volume);
    static std::variant<float, bool>         GetSoundVolume(std::variant<CClientSound*, CClientPlayer*> sound);
    static bool                              SetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> sound, float speed);
    static std::variant<float, bool>         GetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> sound);
    static bool SetSoundProperties(CClientSound* sound, float sampleRate, float tempo, float pitch, std::optional<bool> reversed);
    static auto GetSoundProperties(CClientSound* sound);
    static auto GetSoundFFTData(std::variant<CClientSound*, CClientPlayer*> sound, int length, std::optional<int> bands);
    static auto GetSoundWaveData(std::variant<CClientSound*, CClientPlayer*> sound, int length);
    static auto GetSoundLevelData(std::variant<CClientSound*, CClientPlayer*> sound);
    static std::variant<float, bool>                                       GetSoundBPM(CClientSound* sound);
    static bool                                                            SetSoundPanEnabled(CClientSound* sound, bool enabled);
    static bool                                                            IsSoundPanEnabled(CClientSound* sound);
    static bool                                                            SetSoundMinDistance(CClientSound* sound, float distance);
    static std::variant<float, bool>                                       GetSoundMinDistance(CClientSound* sound);
    static bool                                                            SetSoundMaxDistance(CClientSound* sound, float distance);
    static std::variant<float, bool>                                       GetSoundMaxDistance(CClientSound* sound);
    static auto GetSoundMetaTags(CClientSound* sound, std::optional<std::string> format);
    static bool SetSoundEffectEnabled(std::variant<CClientSound*, CClientPlayer*> sound, const std::string effectName, std::optional<bool> enable);
    static auto GetSoundEffects(std::variant<CClientSound*, CClientPlayer*> sound);
    static bool SetSoundEffectParameter(std::variant<CClientSound*, CClientPlayer*> sound, SoundEffectType::Enum eEffectType, std::string strEffectParameter,
                                        std::variant<float, bool> value);
    static auto GetSoundEffectParameters(std::variant<CClientSound*, CClientPlayer*> sound, SoundEffectType::Enum eEffectType);
    static bool                      SetSoundPan(std::variant<CClientSound*, CClientPlayer*> sound, float pan);
    static std::variant<float, bool> GetSoundPan(std::variant<CClientSound*, CClientPlayer*> sound);

    // Radio functions
    static bool                              SetRadioChannel(unsigned char channel);
    static auto GetRadioChannel();
    static std::variant<const char*, bool>   GetRadioChannelName(int channel);

    static bool ShowSound(bool state);
    static bool IsShowSoundEnabled();
};
