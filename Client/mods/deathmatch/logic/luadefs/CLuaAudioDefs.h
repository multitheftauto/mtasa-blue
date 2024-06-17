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

#include "../Shared/mods/deathmatch/logic/lua/CLuaMultiReturn.h"
#include <variant>
#include <optional>

class CLuaAudioDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Audio funcs
    static bool PlaySoundFrontEnd(std::uint8_t sound);
    static bool SetAmbientSoundEnabled(eAmbientSoundType type, bool enabled) noexcept;
    static bool IsAmbientSoundEnabled(eAmbientSoundType type) noexcept;
    static bool ResetAmbientSounds() noexcept;
    static bool SetWorldSoundEnabled(
        int group,
        std::optional<int> index,
        bool enable,
        std::optional<bool> immediate
    ) noexcept;
    static bool IsWorldSoundEnabled(int group, std::optional<int> index) noexcept;
    static bool ResetWorldSounds() noexcept;
    static CClientSound* PlaySFX(lua_State* luaVM, eAudioLookupIndex container, int bank,
        int audio, std::optional<bool> loop
    );
    static CClientSound* PlaySFX3D(lua_State* luaVM, eAudioLookupIndex container, int bank,
        int audio, float posX, float posY, float posZ, std::optional<bool> loop
    );
    static bool GetSFXStatus(eAudioLookupIndex container) noexcept;

    // Sound effects and synth functions
    static CClientSound* PlaySound(
        lua_State* luaVM,
        SString path,
        std::optional<bool> loop,
        std::optional<bool> throttle
    );
    static CClientSound* PlaySound3D(
        lua_State* luaVM,
        SString path,
        float x,
        float y,
        float z,
        std::optional<bool> loop,
        std::optional<bool> throttle
    );
    static bool StopSound(CClientSound* sound) noexcept;
    static bool SetSoundPosition(
        std::variant<CClientSound*, CClientPlayer*> element,
        double pos
    ) noexcept;
    static double GetSoundPosition(
        std::variant<CClientSound*, CClientPlayer*> element
    ) noexcept;
    static double GetSoundLength(std::variant<CClientSound*, CClientPlayer*> element) noexcept;
    static double GetSoundBufferLength(CClientSound* sound) noexcept;
    static bool SetSoundPaused(std::variant<CClientSound*, CClientPlayer*> element, bool paused) noexcept;
    static bool IsSoundPaused(std::variant<CClientSound*, CClientPlayer*> element) noexcept;
    static bool SetSoundVolume(std::variant<CClientSound*, CClientPlayer*> element, float volume) noexcept;
    static float GetSoundVolume(std::variant<CClientSound*, CClientPlayer*> element) noexcept;
    static bool SetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> element, float speed) noexcept;
    static float GetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> element) noexcept;
    static bool SetSoundProperties(
        CClientSound* sound,
        float sampleRate,
        float tempo,
        float pitch,
        std::optional<bool> reverse
    ) noexcept;
    static std::variant<bool, CLuaMultiReturn<float, float, float, bool>> GetSoundProperties(
        CClientSound* sound
    ) noexcept;
    static std::variant<bool, std::vector<float>> GetSoundFFTData(
        std::variant<CClientSound*, CClientPlayer*> element,
        int samples,
        std::optional<int> bands
    ) noexcept;
    static std::variant<bool, std::vector<float>> GetSoundWaveData(
        std::variant<CClientSound*, CClientPlayer*> element,
        int samples
    ) noexcept;
    static bool SetSoundPanningEnabled(CClientSound* sound, bool enable) noexcept;
    LUA_DECLARE(IsSoundPanEnabled);
    static CLuaMultiReturn<int, int> GetSoundLevelData(
        std::variant<CClientSound*, CClientPlayer*> element
    ) noexcept;
    LUA_DECLARE(GetSoundBPM);
    LUA_DECLARE(SetSoundMinDistance);
    LUA_DECLARE(GetSoundMinDistance);
    LUA_DECLARE(SetSoundMaxDistance);
    LUA_DECLARE(GetSoundMaxDistance);
    LUA_DECLARE(GetSoundMetaTags);
    LUA_DECLARE(SetSoundEffectEnabled);
    LUA_DECLARE(GetSoundEffects);
    LUA_DECLARE(SetSoundEffectParameter);
    LUA_DECLARE(GetSoundEffectParameters);
    LUA_DECLARE(SetSoundPan);
    LUA_DECLARE(GetSoundPan);

    static bool SetSoundLooped(CClientSound* pSound, bool bLoop);
    static bool IsSoundLooped(CClientSound* pSound);

    // Radio functions
    LUA_DECLARE(SetRadioChannel);
    LUA_DECLARE(GetRadioChannel);
    LUA_DECLARE(GetRadioChannelName);

    static bool ShowSound(bool state);
    static bool IsShowSoundEnabled();
};
