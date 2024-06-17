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
#include <optional>

class CLuaAudioDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Audio funcs
    bool PlaySoundFrontEnd(std::uint8_t sound);
    bool SetAmbientSoundEnabled(eAmbientSoundType type, bool enabled) noexcept;
    bool IsAmbientSoundEnabled(eAmbientSoundType type) noexcept;
    bool ResetAmbientSounds() noexcept;
    bool SetWorldSoundEnabled (
        int group,
        std::optional<int> index,
        bool enable,
        std::optional<bool> immediate
    ) noexcept;
    bool IsWorldSoundEnabled(int group, std::optional<int> index) noexcept;
    bool ResetWorldSounds() noexcept;
    CClientSound* PlaySFX(lua_State* luaVM, eAudioLookupIndex container, int bank,
        int audio, std::optional<bool> loop
    );
    CClientSound* PlaySFX3D(lua_State* luaVM, eAudioLookupIndex container, int bank,
        int audio, float posX, float posY, float posZ, std::optional<bool> loop
    );
    bool GetSFXStatus(eAudioLookupIndex container) noexcept;

    // Sound effects and synth functions
    CClientSound* PlaySound(
        lua_State* luaVM,
        SString path,
        std::optional<bool> loop,
        std::optional<bool> throttle
    );
    CClientSound* PlaySound3D(
        lua_State* luaVM,
        SString path,
        float x,
        float y,
        float z,
        std::optional<bool> loop,
        std::optional<bool> throttle
    );
    bool StopSound(CClientSound* sound) noexcept;
    bool SetSoundPosition(
        std::variant<CClientSound*, CClientPlayer*> element,
        double pos
    ) noexcept;
    double GetSoundPosition(
        std::variant<CClientSound*, CClientPlayer*> element
    ) noexcept;
    double GetSoundLength(std::variant<CClientSound*, CClientPlayer*> element) noexcept;
    LUA_DECLARE(GetSoundBufferLength);
    LUA_DECLARE(SetSoundPaused);
    LUA_DECLARE(IsSoundPaused);
    LUA_DECLARE(SetSoundVolume);
    LUA_DECLARE(GetSoundVolume);
    LUA_DECLARE(SetSoundSpeed);
    LUA_DECLARE(GetSoundSpeed);
    LUA_DECLARE(SetSoundProperties);
    LUA_DECLARE(GetSoundProperties);
    LUA_DECLARE(GetSoundFFTData);
    LUA_DECLARE(GetSoundWaveData);
    LUA_DECLARE(SetSoundPanEnabled);
    LUA_DECLARE(IsSoundPanEnabled);
    LUA_DECLARE(GetSoundLevelData);
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
