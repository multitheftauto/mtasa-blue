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
    static bool PlaySoundFrontEnd(unsigned char ucSound);
    LUA_DECLARE(SetAmbientSoundEnabled);
    LUA_DECLARE(IsAmbientSoundEnabled);
    LUA_DECLARE(ResetAmbientSounds);
    LUA_DECLARE(SetWorldSoundEnabled);
    LUA_DECLARE(IsWorldSoundEnabled);
    LUA_DECLARE(ResetWorldSounds);
    static std::variant<CClientSound*, bool>  PlaySFX(lua_State* luaVM, eAudioLookupIndex containerIndex, std::variant<int, eRadioStreamIndex> bankIndex,
                                                      int iAudioIndex, std::optional<bool> bLoop);
    static std::variant<CClientSound*, bool>  PlaySFX3D(lua_State* luaVM, eAudioLookupIndex containerIndex, std::variant<int, eRadioStreamIndex> bankIndex,
                                                        int iAudioIndex, CVector vecPosition, std::optional<bool> bLoop);
    static std::variant<bool, std::nullptr_t> GetSFXStatus(eAudioLookupIndex containerIndex);

    // Sound effects and synth functions
    static std::variant<CClientSound*, bool> PlaySound(lua_State* luaVM, const std::string strSound, std::optional<bool> bLoop, std::optional<bool> bThrottle);
    static std::variant<CClientSound*, bool> PlaySound3D(lua_State* luaVM, const std::string strSound, CVector vecPosition, std::optional<bool> bLoop,
                                                         std::optional<bool> bThrottle);
    static bool                              StopSound(CClientSound* pSound);
    static bool                              SetSoundPosition(std::variant<CClientSound*, CClientPlayer*> sound, double dPosition);
    static std::variant<double, bool>        GetSoundPosition(std::variant<CClientSound*, CClientPlayer*> sound);
    static std::variant<double, bool>        GetSoundLength(std::variant<CClientSound*, CClientPlayer*> sound);
    static std::variant<double, bool>        GetSoundBufferLength(CClientSound* pSound);
    static bool                              SetSoundLooped(CClientSound* pSound, bool bLoop);
    static bool                              IsSoundLooped(CClientSound* pSound);
    static bool                              SetSoundPaused(std::variant<CClientSound*, CClientPlayer*> sound, bool bPaused);
    static bool                              IsSoundPaused(std::variant<CClientSound*, CClientPlayer*> sound);
    static bool                              SetSoundVolume(std::variant<CClientSound*, CClientPlayer*> sound, float fVolume);
    static std::variant<float, bool>         GetSoundVolume(std::variant<CClientSound*, CClientPlayer*> sound);
    static bool                              SetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> sound, float fSpeed);
    static std::variant<float, bool>         GetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> sound);
    static bool SetSoundProperties(CClientSound* pSound, float fSampleRate, float fTempo, float fPitch, std::optional<bool> bReversed);
    static std::variant<CLuaMultiReturn<float, float, float, bool>, bool>  GetSoundProperties(CClientSound* pSound);
    static std::variant<std::unordered_map<int, float>, bool>              GetSoundFFTData(std::variant<CClientSound*, CClientPlayer*> sound, int iLength,
                                                                                           std::optional<int> iBands);
    static std::variant<std::unordered_map<int, float>, bool>              GetSoundWaveData(std::variant<CClientSound*, CClientPlayer*> sound, int iLength);
    static std::variant<CLuaMultiReturn<unsigned int, unsigned int>, bool> GetSoundLevelData(std::variant<CClientSound*, CClientPlayer*> sound);
    static std::variant<float, bool>                                       GetSoundBPM(CClientSound* pSound);
    static bool                                                            SetSoundPanEnabled(CClientSound* pSound, bool bEnabled);
    static bool                                                            IsSoundPanEnabled(CClientSound* pSound);
    static bool                                                            SetSoundMinDistance(CClientSound* pSound, float fDistance);
    static std::variant<float, bool>                                       GetSoundMinDistance(CClientSound* pSound);
    static bool                                                            SetSoundMaxDistance(CClientSound* pSound, float fDistance);
    static std::variant<float, bool>                                       GetSoundMaxDistance(CClientSound* pSound);
    static std::variant<SString, std::unordered_map<std::string, std::string>, bool> GetSoundMetaTags(CClientSound*              pSound,
                                                                                                      std::optional<std::string> strFormat);
    static bool SetSoundEffectEnabled(std::variant<CClientSound*, CClientPlayer*> sound, const std::string strEffectName, std::optional<bool> bEnable);
    static std::variant<std::unordered_map<std::string, bool>, bool> GetSoundEffects(std::variant<CClientSound*, CClientPlayer*> sound);
    static bool SetSoundEffectParameter(std::variant<CClientSound*, CClientPlayer*> sound, SoundEffectType::Enum eEffectType, std::string strEffectParameter,
                                        std::variant<float, bool> value);
    static std::variant<std::unordered_map<std::string, std::variant<float, int, bool>>, bool> GetSoundEffectParameters(
        std::variant<CClientSound*, CClientPlayer*> sound, SoundEffectType::Enum eEffectType);
    static bool                      SetSoundPan(std::variant<CClientSound*, CClientPlayer*> sound, float fPan);
    static std::variant<float, bool> GetSoundPan(std::variant<CClientSound*, CClientPlayer*> sound);

    // Radio functions
    static bool                              SetRadioChannel(unsigned char ucChannel);
    static std::variant<unsigned char, bool> GetRadioChannel();
    static std::variant<const char*, bool>   GetRadioChannelName(int iChannel);

    static bool ShowSound(bool state);
    static bool IsShowSoundEnabled();
};
