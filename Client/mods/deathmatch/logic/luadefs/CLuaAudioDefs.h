/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaAudioDefs.h
 *  PURPOSE:     Lua audio definitions class header
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <variant>

#include "../Shared/mods/deathmatch/logic/lua/CLuaMultiReturn.h"
#include <unordered_map>
#include <variant>
#include <optional>

class CLuaAudioDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Audio funcs
    static bool          PlaySoundFrontEnd(std::uint8_t sound);
    static bool          SetAmbientSoundEnabled(eAmbientSoundType type, bool enabled) noexcept;
    static bool          IsAmbientSoundEnabled(eAmbientSoundType type) noexcept;
    static bool          ResetAmbientSounds() noexcept;
    static bool          SetWorldSoundEnabled(int group, std::optional<int> index, bool enable, std::optional<bool> immediate) noexcept;
    static bool          IsWorldSoundEnabled(int group, std::optional<int> index) noexcept;
    static bool          ResetWorldSounds() noexcept;
    static CClientSound* PlaySFX(lua_State* luaVM, eAudioLookupIndex container, int bank, int audio, std::optional<bool> loop);
    static CClientSound* PlaySFX3D(lua_State* luaVM, eAudioLookupIndex container, int bank, int audio, float posX, float posY, float posZ,
                                   std::optional<bool> loop);
    static bool          GetSFXStatus(eAudioLookupIndex container) noexcept;

    // Sound effects and synth functions
    static CClientSound* PlaySound(lua_State* luaVM, std::string path, std::optional<bool> loop, std::optional<bool> throttle);
    static CClientSound* PlaySound3D(lua_State* luaVM, std::string path, float x, float y, float z, std::optional<bool> loop, std::optional<bool> throttle);
    static bool          StopSound(CClientSound* sound) noexcept;
    static bool          SetSoundPosition(std::variant<CClientSound*, CClientPlayer*> element, double pos) noexcept;
    static double        GetSoundPosition(std::variant<CClientSound*, CClientPlayer*> element) noexcept;
    static double        GetSoundLength(std::variant<CClientSound*, CClientPlayer*> element) noexcept;
    static double        GetSoundBufferLength(CClientSound* sound) noexcept;
    static bool          SetSoundPaused(std::variant<CClientSound*, CClientPlayer*> element, bool paused) noexcept;
    static bool          IsSoundPaused(std::variant<CClientSound*, CClientPlayer*> element) noexcept;
    static bool          SetSoundVolume(std::variant<CClientSound*, CClientPlayer*> element, float volume) noexcept;
    static float         GetSoundVolume(std::variant<CClientSound*, CClientPlayer*> element) noexcept;
    static bool          SetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> element, float speed) noexcept;
    static float         GetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> element) noexcept;
    static bool          SetSoundProperties(CClientSound* sound, float sampleRate, float tempo, float pitch, std::optional<bool> reverse) noexcept;

    static std::variant<bool, CLuaMultiReturn<float, float, float, bool>> GetSoundProperties(CClientSound* sound) noexcept;
    static std::variant<bool, std::vector<float>>                         GetSoundFFTData(std::variant<CClientSound*, CClientPlayer*> element, int samples,
                                                                                          std::optional<int> bands) noexcept;
    static std::variant<bool, std::vector<float>> GetSoundWaveData(std::variant<CClientSound*, CClientPlayer*> element, int samples) noexcept;
    static bool                                   SetSoundPanEnabled(CClientSound* sound, bool enable) noexcept;
    static std::variant<bool, CLuaMultiReturn<std::uint32_t, std::uint32_t>> GetSoundLevelData(std::variant<CClientSound*, CClientPlayer*> element) noexcept;
    static bool                                                              IsSoundPanningEnabled(CClientSound* sound) noexcept;
    static float                                                             GetSoundBPM(CClientSound* sound) noexcept;
    static bool                                                              SetSoundMinDistance(CClientSound* sound, float distance) noexcept;
    static float                                                             GetSoundMinDistance(CClientSound* sound) noexcept;
    static bool                                                              SetSoundMaxDistance(CClientSound* sound, float distance) noexcept;
    static float                                                             GetSoundMaxDistance(CClientSound* sound) noexcept;
    static std::variant<bool, std::string, std::unordered_map<std::string, std::string>> GetSoundMetaTags(CClientSound*              sound,
                                                                                                          std::optional<std::string> format) noexcept;
    static bool SetSoundEffectEnabled(std::variant<CClientSound*, CClientPlayer*> element, std::string effectName, std::optional<bool> enable) noexcept;
    static std::unordered_map<std::string, bool> GetSoundEffects(std::variant<CClientSound*, CClientPlayer*> element) noexcept;
    static bool SetSoundEffectParameter(CClientSound* sound, eSoundEffectType effectType, std::string effectParam, CLuaArgument value);
    static std::variant<bool, std::unordered_map<std::string, CLuaArgument>> GetSoundEffectParameters(CClientSound*    sound,
                                                                                                      eSoundEffectType effectType) noexcept;
    static bool  SetSoundPan(std::variant<CClientSound*, CClientPlayer*> element, float pan) noexcept;
    static float GetSoundPan(std::variant<CClientSound*, CClientPlayer*> element) noexcept;

    static bool SetSoundLooped(CClientSound* pSound, bool bLoop) noexcept;
    static bool IsSoundLooped(CClientSound* pSound) noexcept;

    // Radio functions
    static bool                            SetRadioChannel(std::uint8_t channel) noexcept;
    static std::uint8_t                    GetRadioChannel() noexcept;
    static std::variant<bool, std::string> GetRadioChannelName(int channel) noexcept;

    static bool ShowSound(bool state) noexcept;
    static bool IsShowSoundEnabled() noexcept;
};
