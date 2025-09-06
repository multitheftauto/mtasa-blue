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

void CLuaAudioDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{// Audio funcs
                                                                             {"playSoundFrontEnd", PlaySoundFrontEnd},
                                                                             {"setAmbientSoundEnabled", SetAmbientSoundEnabled},
                                                                             {"isAmbientSoundEnabled", IsAmbientSoundEnabled},
                                                                             {"resetAmbientSounds", ResetAmbientSounds},
                                                                             {"setWorldSoundEnabled", SetWorldSoundEnabled},
                                                                             {"isWorldSoundEnabled", IsWorldSoundEnabled},
                                                                             {"resetWorldSounds", ResetWorldSounds},
                                                                             {"playSFX", PlaySFX},
                                                                             {"playSFX3D", PlaySFX3D},
                                                                             {"getSFXStatus", GetSFXStatus},

                                                                             // Sound effects and synth funcs
                                                                             {"playSound", PlaySound},
                                                                             {"playSound3D", PlaySound3D},
                                                                             {"stopSound", StopSound},
                                                                             {"setSoundPosition", SetSoundPosition},
                                                                             {"getSoundPosition", GetSoundPosition},
                                                                             {"getSoundLength", GetSoundLength},
                                                                             {"getSoundBufferLength", GetSoundBufferLength},
                                                                             {"setSoundLooped", ArgumentParser<SetSoundLooped>},
                                                                             {"isSoundLooped", ArgumentParser<IsSoundLooped>},
                                                                             {"setSoundPaused", SetSoundPaused},
                                                                             {"isSoundPaused", IsSoundPaused},
                                                                             {"setSoundVolume", SetSoundVolume},
                                                                             {"getSoundVolume", GetSoundVolume},
                                                                             {"setSoundSpeed", SetSoundSpeed},
                                                                             {"getSoundSpeed", GetSoundSpeed},
                                                                             {"setSoundProperties", SetSoundProperties},
                                                                             {"getSoundProperties", GetSoundProperties},
                                                                             {"getSoundFFTData", GetSoundFFTData},
                                                                             {"getSoundWaveData", GetSoundWaveData},
                                                                             {"getSoundLevelData", GetSoundLevelData},
                                                                             {"getSoundBPM", GetSoundBPM},
                                                                             {"setSoundPanningEnabled", SetSoundPanEnabled},
                                                                             {"isSoundPanningEnabled", IsSoundPanEnabled},
                                                                             {"setSoundMinDistance", SetSoundMinDistance},
                                                                             {"getSoundMinDistance", GetSoundMinDistance},
                                                                             {"setSoundMaxDistance", SetSoundMaxDistance},
                                                                             {"getSoundMaxDistance", GetSoundMaxDistance},
                                                                             {"getSoundMetaTags", GetSoundMetaTags},
                                                                             {"setSoundEffectEnabled", SetSoundEffectEnabled},
                                                                             {"getSoundEffects", GetSoundEffects},
                                                                             {"setSoundEffectParameter", SetSoundEffectParameter},
                                                                             {"getSoundEffectParameters", GetSoundEffectParameters},
                                                                             {"setSoundPan", SetSoundPan},
                                                                             {"getSoundPan", GetSoundPan},

                                                                             // Radio funcs
                                                                             {"setRadioChannel", SetRadioChannel},
                                                                             {"getRadioChannel", GetRadioChannel},
                                                                             {"getRadioChannelName", GetRadioChannelName},

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
}

int CLuaAudioDefs::PlaySound(lua_State* luaVM)
{
    SString          strSound = "";
    bool             bLoop = false;
    bool             bThrottle = true;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strSound);
    argStream.ReadBool(bLoop, false);
    argStream.ReadBool(bThrottle, true);

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            CResource* pResource = luaMain->GetResource();
            if (pResource)
            {
                SString strFilename;
                bool    bIsURL = false;
                bool    bIsRawData = false;

                if (CResourceManager::ParseResourcePathInput(strSound, pResource, &strFilename, nullptr, true))
                    strSound = strFilename;
                else
                {
                    if ((stricmp(strSound.Left(4), "http") == 0 || stricmp(strSound.Left(3), "ftp") == 0) &&
                        (strSound.length() <= 2048 || strSound.find('\n') == SString::npos))
                        bIsURL = true;
                    else
                        bIsRawData = true;
                }

                // ParseResourcePathInput changes pResource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3"
                // ) Fixes #6507 - Caz
                if (pResource)
                {
                    CClientSound* pSound = CStaticFunctionDefinitions::PlaySound(pResource, strSound, bIsURL, bIsRawData, bLoop, bThrottle);
                    if (pSound)
                    {
                        // call onClientSoundStarted
                        CLuaArguments Arguments;
                        Arguments.PushString("play");            // Reason
                        pSound->CallEvent("onClientSoundStarted", Arguments, false);

                        lua_pushelement(luaVM, pSound);
                        return 1;
                    }
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::PlaySound3D(lua_State* luaVM)
{
    SString          strSound = "";
    CVector          vecPosition;
    bool             bLoop = false;
    bool             bThrottle = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strSound);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadBool(bLoop, false);
    argStream.ReadBool(bThrottle, true);

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            CResource* pResource = luaMain->GetResource();
            if (pResource)
            {
                SString strFilename;
                bool    bIsURL = false;
                bool    bIsRawData = false;
                if (CResourceManager::ParseResourcePathInput(strSound, pResource, &strFilename))
                    strSound = strFilename;
                else
                {
                    if ((stricmp(strSound.Left(4), "http") == 0 || stricmp(strSound.Left(3), "ftp") == 0) &&
                        (strSound.length() <= 2048 || strSound.find('\n') == SString::npos))
                        bIsURL = true;
                    else
                        bIsRawData = true;
                }

                // ParseResourcePathInput changes pResource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3"
                // ) Fixes #6507 - Caz
                if (pResource)
                {
                    CClientSound* pSound = CStaticFunctionDefinitions::PlaySound3D(pResource, strSound, bIsURL, bIsRawData, vecPosition, bLoop, bThrottle);
                    if (pSound)
                    {
                        // call onClientSoundStarted
                        CLuaArguments Arguments;
                        Arguments.PushString("play");            // Reason
                        pSound->CallEvent("onClientSoundStarted", Arguments, false);

                        lua_pushelement(luaVM, pSound);
                        return 1;
                    }
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::StopSound(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pSound);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::StopSound(*pSound))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::SetSoundPosition(lua_State* luaVM)
{
    CClientPlayer*   pPlayer = NULL;
    CClientSound*    pSound = NULL;
    double           dPosition = 0.0;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }
    argStream.ReadNumber(dPosition);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::SetSoundPosition(*pSound, dPosition))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else if (pPlayer)
        {
            if (CStaticFunctionDefinitions::SetSoundPosition(*pPlayer, dPosition))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundPosition(lua_State* luaVM)
{
    CClientPlayer*   pPlayer = NULL;
    CClientSound*    pSound = NULL;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            double dPosition = 0;
            if (CStaticFunctionDefinitions::GetSoundPosition(*pSound, dPosition))
            {
                lua_pushnumber(luaVM, dPosition);
                return 1;
            }
        }
        else if (pPlayer)
        {
            double dPosition = 0;
            if (CStaticFunctionDefinitions::GetSoundPosition(*pPlayer, dPosition))
            {
                lua_pushnumber(luaVM, dPosition);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundLength(lua_State* luaVM)
{
    CClientPlayer*   pPlayer = NULL;
    CClientSound*    pSound = NULL;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            double dLength = 0;
            if (CStaticFunctionDefinitions::GetSoundLength(*pSound, dLength))
            {
                lua_pushnumber(luaVM, dLength);
                return 1;
            }
        }
        else if (pPlayer)
        {
            double dLength = 0;
            if (CStaticFunctionDefinitions::GetSoundLength(*pPlayer, dLength))
            {
                lua_pushnumber(luaVM, dLength);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundBufferLength(lua_State* luaVM)
{
    CClientSound* pSound;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pSound);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            double dBufferLength = 0;
            if (CStaticFunctionDefinitions::GetSoundBufferLength(*pSound, dBufferLength))
            {
                lua_pushnumber(luaVM, dBufferLength);
                return 1;
            }
            else
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

bool CLuaAudioDefs::SetSoundLooped(CClientSound* pSound, bool bLoop)
{
    return pSound->SetLooped(bLoop);
}

bool CLuaAudioDefs::IsSoundLooped(CClientSound* pSound)
{
    return pSound->IsLooped();
}

int CLuaAudioDefs::SetSoundPaused(lua_State* luaVM)
{
    CClientPlayer*   pPlayer = NULL;
    CClientSound*    pSound = NULL;
    bool             bPaused = false;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }
    argStream.ReadBool(bPaused);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::SetSoundPaused(*pSound, bPaused))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else if (pPlayer)
        {
            if (CStaticFunctionDefinitions::SetSoundPaused(*pPlayer, bPaused))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::IsSoundPaused(lua_State* luaVM)
{
    CClientPlayer*   pPlayer = NULL;
    CClientSound*    pSound = NULL;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            bool bPaused = false;
            if (CStaticFunctionDefinitions::IsSoundPaused(*pSound, bPaused))
            {
                lua_pushboolean(luaVM, bPaused);
                return 1;
            }
        }
        else if (pPlayer)
        {
            bool bPaused = false;
            if (CStaticFunctionDefinitions::IsSoundPaused(*pPlayer, bPaused))
            {
                lua_pushboolean(luaVM, bPaused);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::SetSoundVolume(lua_State* luaVM)
{
    CClientPlayer*   pPlayer = NULL;
    CClientSound*    pSound = NULL;
    float            fVolume = 0.0f;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }
    argStream.ReadNumber(fVolume);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::SetSoundVolume(*pSound, fVolume))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else if (pPlayer)
        {
            if (CStaticFunctionDefinitions::SetSoundVolume(*pPlayer, fVolume))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundVolume(lua_State* luaVM)
{
    CClientPlayer*   pPlayer = NULL;
    CClientSound*    pSound = NULL;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            float fVolume = 0.0f;
            if (CStaticFunctionDefinitions::GetSoundVolume(*pSound, fVolume))
            {
                lua_pushnumber(luaVM, fVolume);
                return 1;
            }
        }
        else if (pPlayer)
        {
            float fVolume = 0.0f;
            if (CStaticFunctionDefinitions::GetSoundVolume(*pPlayer, fVolume))
            {
                lua_pushnumber(luaVM, fVolume);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::SetSoundSpeed(lua_State* luaVM)
{
    CClientPlayer*   pPlayer = NULL;
    CClientSound*    pSound = NULL;
    float            fSpeed = 0.0f;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }
    argStream.ReadNumber(fSpeed);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::SetSoundSpeed(*pSound, fSpeed))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else if (pPlayer)
        {
            if (CStaticFunctionDefinitions::SetSoundSpeed(*pPlayer, fSpeed))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::SetSoundProperties(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    bool             bReversed = false;
    float            fSampleRate = 0.0f, fTempo = 0.0f, fPitch = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pSound);
    argStream.ReadNumber(fSampleRate);
    argStream.ReadNumber(fTempo);
    argStream.ReadNumber(fPitch);
    argStream.ReadBool(bReversed, false);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::SetSoundProperties(*pSound, fSampleRate, fTempo, fPitch, bReversed))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundProperties(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    bool             bReversed = false;
    float            fSampleRate = 0.0f, fTempo = 0.0f, fPitch = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pSound);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::GetSoundProperties(*pSound, fSampleRate, fTempo, fPitch, bReversed))
            {
                lua_pushnumber(luaVM, fSampleRate);
                lua_pushnumber(luaVM, fTempo);
                lua_pushnumber(luaVM, fPitch);
                lua_pushboolean(luaVM, bReversed);
                return 4;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundFFTData(lua_State* luaVM)
{
    CClientPlayer*   pPlayer = NULL;
    CClientSound*    pSound = NULL;
    float*           pData = NULL;
    int              iLength = 0;
    int              iBands = 0;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }
    argStream.ReadNumber(iLength);
    argStream.ReadNumber(iBands, 0);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            pData = CStaticFunctionDefinitions::GetSoundFFTData(*pSound, iLength, iBands);
        }
        else if (pPlayer)
        {
            pData = CStaticFunctionDefinitions::GetSoundFFTData(*pPlayer, iLength, iBands);
        }
        if (pData != NULL)
        {
            if (iBands == 0)
            {
                // Create a new table
                lua_newtable(luaVM);
                for (int i = 0; i <= iLength / 2; i++)
                {
                    lua_pushnumber(luaVM, i);
                    lua_pushnumber(luaVM, pData[i]);
                    lua_settable(luaVM, -3);
                }
            }
            else
            {
                // Create a new table
                lua_newtable(luaVM);
                for (int i = 0; i <= iBands - 1; i++)
                {
                    lua_pushnumber(luaVM, i);
                    lua_pushnumber(luaVM, pData[i]);
                    lua_settable(luaVM, -3);
                }
            }
            // Deallocate our data array here after it's used.
            delete[] pData;
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundWaveData(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    CClientPlayer*   pPlayer = NULL;
    float*           pData = NULL;
    int              iLength = 0;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return 1;
    }
    argStream.ReadNumber(iLength);

    if (!argStream.HasErrors())
    {
        if (pSound != NULL)
        {
            pData = CStaticFunctionDefinitions::GetSoundWaveData(*pSound, iLength);
        }
        else if (pPlayer != NULL)
        {
            pData = CStaticFunctionDefinitions::GetSoundWaveData(*pPlayer, iLength);
        }
        else
        {
            m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
            lua_pushboolean(luaVM, false);
            return 1;
        }
        if (pData != NULL)
        {
            // Create a new table
            lua_newtable(luaVM);
            for (int i = 0; i < iLength; i++)
            {
                lua_pushnumber(luaVM, i);
                lua_pushnumber(luaVM, pData[i]);
                lua_settable(luaVM, -3);
            }
            // Deallocate our data array here after it's used.
            delete[] pData;
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundLevelData(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    CClientPlayer*   pPlayer = NULL;
    DWORD            dwLeft = 0, dwRight = 0;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return 1;
    }

    if (!argStream.HasErrors())
    {
        if (pSound != NULL && CStaticFunctionDefinitions::GetSoundLevelData(*pSound, dwLeft, dwRight))
        {
            lua_pushnumber(luaVM, dwLeft);
            lua_pushnumber(luaVM, dwRight);
            return 2;
        }
        else if (pPlayer != NULL && CStaticFunctionDefinitions::GetSoundLevelData(*pPlayer, dwLeft, dwRight))
        {
            lua_pushnumber(luaVM, dwLeft);
            lua_pushnumber(luaVM, dwRight);
            return 2;
        }
        else
        {
            lua_pushboolean(luaVM, false);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundBPM(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    float            fBPM = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pSound);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::GetSoundBPM(*pSound, fBPM))
        {
            lua_pushnumber(luaVM, fBPM);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::SetSoundPanEnabled(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    bool             bEnabled = true;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pSound);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::SetSoundPanEnabled(*pSound, bEnabled))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::IsSoundPanEnabled(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    bool             bEnabled = true;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pSound);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::IsSoundPanEnabled(*pSound))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundSpeed(lua_State* luaVM)
{
    CClientPlayer*   pPlayer = NULL;
    CClientSound*    pSound = NULL;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            float fSpeed = 0.0f;
            if (CStaticFunctionDefinitions::GetSoundSpeed(*pSound, fSpeed))
            {
                lua_pushnumber(luaVM, fSpeed);
                return 1;
            }
        }
        else if (pPlayer)
        {
            float fSpeed = 0.0f;
            if (CStaticFunctionDefinitions::GetSoundSpeed(*pPlayer, fSpeed))
            {
                lua_pushnumber(luaVM, fSpeed);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::SetSoundMinDistance(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    float            fDistance = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pSound);
    argStream.ReadNumber(fDistance);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::SetSoundMinDistance(*pSound, fDistance))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundMinDistance(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    float            fDistance = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pSound);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::GetSoundMinDistance(*pSound, fDistance))
            {
                lua_pushnumber(luaVM, fDistance);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::SetSoundMaxDistance(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    float            fDistance = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pSound);
    argStream.ReadNumber(fDistance);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::SetSoundMaxDistance(*pSound, fDistance))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundMaxDistance(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    float            fDistance = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pSound);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::GetSoundMaxDistance(*pSound, fDistance))
            {
                lua_pushnumber(luaVM, fDistance);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundMetaTags(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    SString          strFormat = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pSound);
    argStream.ReadString(strFormat, "");

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            SString strMetaTags = "";
            if (strFormat != "")
            {
                if (CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, strFormat, strMetaTags))
                {
                    if (!strMetaTags.empty())
                    {
                        lua_pushstring(luaVM, strMetaTags);
                        return 1;
                    }
                }
            }
            else
            {
                lua_newtable(luaVM);
                CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, "%TITL", strMetaTags);
                if (!strMetaTags.empty())
                {
                    lua_pushstring(luaVM, strMetaTags);
                    lua_setfield(luaVM, -2, "title");
                }
                CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, "%ARTI", strMetaTags);
                if (!strMetaTags.empty())
                {
                    lua_pushstring(luaVM, strMetaTags);
                    lua_setfield(luaVM, -2, "artist");
                }
                CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, "%ALBM", strMetaTags);
                if (!strMetaTags.empty())
                {
                    lua_pushstring(luaVM, strMetaTags);
                    lua_setfield(luaVM, -2, "album");
                }
                CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, "%GNRE", strMetaTags);
                if (!strMetaTags.empty())
                {
                    lua_pushstring(luaVM, strMetaTags);
                    lua_setfield(luaVM, -2, "genre");
                }
                CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, "%YEAR", strMetaTags);
                if (!strMetaTags.empty())
                {
                    lua_pushstring(luaVM, strMetaTags);
                    lua_setfield(luaVM, -2, "year");
                }
                CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, "%CMNT", strMetaTags);
                if (!strMetaTags.empty())
                {
                    lua_pushstring(luaVM, strMetaTags);
                    lua_setfield(luaVM, -2, "comment");
                }
                CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, "%TRCK", strMetaTags);
                if (!strMetaTags.empty())
                {
                    lua_pushstring(luaVM, strMetaTags);
                    lua_setfield(luaVM, -2, "track");
                }
                CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, "%COMP", strMetaTags);
                if (!strMetaTags.empty())
                {
                    lua_pushstring(luaVM, strMetaTags);
                    lua_setfield(luaVM, -2, "composer");
                }
                CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, "%COPY", strMetaTags);
                if (!strMetaTags.empty())
                {
                    lua_pushstring(luaVM, strMetaTags);
                    lua_setfield(luaVM, -2, "copyright");
                }
                CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, "%SUBT", strMetaTags);
                if (!strMetaTags.empty())
                {
                    lua_pushstring(luaVM, strMetaTags);
                    lua_setfield(luaVM, -2, "subtitle");
                }
                CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, "%AART", strMetaTags);
                if (!strMetaTags.empty())
                {
                    lua_pushstring(luaVM, strMetaTags);
                    lua_setfield(luaVM, -2, "album_artist");
                }
                CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, "streamName", strMetaTags);
                if (!strMetaTags.empty())
                {
                    lua_pushstring(luaVM, strMetaTags);
                    lua_setfield(luaVM, -2, "stream_name");
                }
                CStaticFunctionDefinitions::GetSoundMetaTags(*pSound, "streamTitle", strMetaTags);
                if (!strMetaTags.empty())
                {
                    lua_pushstring(luaVM, strMetaTags);
                    lua_setfield(luaVM, -2, "stream_title");
                }
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::SetSoundEffectEnabled(lua_State* luaVM)
{
    CClientPlayer*   pPlayer = NULL;
    CClientSound*    pSound = NULL;
    SString          strEffectName = "";
    bool             bEnable = false;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }
    argStream.ReadString(strEffectName);
    argStream.ReadBool(bEnable, false);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            if (CStaticFunctionDefinitions::SetSoundEffectEnabled(*pSound, strEffectName, bEnable))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else if (pPlayer)
        {
            if (CStaticFunctionDefinitions::SetSoundEffectEnabled(*pPlayer, strEffectName, bEnable))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundEffects(lua_State* luaVM)
{
    CClientPlayer*   pPlayer = NULL;
    CClientSound*    pSound = NULL;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            std::map<std::string, int> iFxEffects = m_pManager->GetSoundManager()->GetFxEffects();
            lua_newtable(luaVM);
            for (std::map<std::string, int>::const_iterator iter = iFxEffects.begin(); iter != iFxEffects.end(); ++iter)
            {
                lua_pushboolean(luaVM, pSound->IsFxEffectEnabled((*iter).second));
                lua_setfield(luaVM, -2, (*iter).first.c_str());
            }
            return 1;
        }
        else if (pPlayer)
        {
            CClientPlayerVoice*        pPlayerVoice = pPlayer->GetVoice();
            std::map<std::string, int> iFxEffects = m_pManager->GetSoundManager()->GetFxEffects();
            lua_newtable(luaVM);
            for (std::map<std::string, int>::const_iterator iter = iFxEffects.begin(); iter != iFxEffects.end(); ++iter)
            {
                lua_pushboolean(luaVM, pPlayerVoice->IsFxEffectEnabled((*iter).second));
                lua_setfield(luaVM, -2, (*iter).first.c_str());
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
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

int CLuaAudioDefs::SetSoundEffectParameter(lua_State* luaVM)
{
    //  bool setSoundEffectParameter ( sound/player sound, string effectName, string effectParameter, var effectParameterValue  )
    CClientSound* pSound{};
    SPlayerVoiceWrapper playerVoice;
    SoundEffectType eEffectType;

    CScriptArgReader argStream(luaVM);    

    // Call `SetFxEffectParameters` and log errors if any
    const auto SetParamWithErrorLog = [luaVM, &eEffectType](auto* pSound, auto effectParam, auto& params) {
        // Try setting parameter
        if (pSound->SetFxEffectParameters((uint)eEffectType, &params))
        {
            lua::Push(luaVM, true);
            return 1;
        }

        // Unsuccessful, log error. (Hard error on usage mistakes)
        // `luaL_error` with a format string straight out crashes, so we have to do it this way..
        const SString msg("BASS Error %i, after setting parameter %s -> %s. (Message: %s)", CBassAudio::ErrorGetCode(), EnumToString(eEffectType).c_str(),
                          EnumToString(effectParam).c_str(), CBassAudio::ErrorGetMessage());

        // Do not use `luaL_error` here and pass in `msg` as the format string,
        // user could inject paramters into the format string, and that would be bad :D
        // The below code is based on the code from `luaL_error`
        luaL_where(luaVM, 1);
        lua::Push(luaVM, msg);
        lua_concat(luaVM, 2);
        lua_error(luaVM);

        return 1;
    };

    const auto ProcessSoundParams = [&eEffectType, luaVM, &argStream, &SetParamWithErrorLog](auto* pSound) {
        if (!pSound->IsFxEffectEnabled((std::uint32_t)eEffectType))
            return luaL_error(luaVM, "Effect's parameters can't be set unless it's enabled");

        using namespace eSoundEffectParams;
        switch (eEffectType)
        {
            case SoundEffectType::FX_DX8_CHORUS:
            {
                BASS_DX8_CHORUS params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Chorus eEffectParameter;
                argStream.ReadEnumString(eEffectParameter);
                switch (eEffectParameter)
                {
                    case Chorus::WET_DRY_MIX:
                    {
                        argStream.ReadNumber(params.fWetDryMix);
                        break;
                    }
                    case Chorus::DEPTH:
                    {
                        argStream.ReadNumber(params.fDepth);
                        break;
                    }
                    case Chorus::FEEDBACK:
                    {
                        argStream.ReadNumber(params.fFeedback);
                        break;
                    }
                    case Chorus::FREQUENCY:
                    {
                        argStream.ReadNumber(params.fFrequency);
                        break;
                    }
                    case Chorus::WAVEFORM:
                    {
                        argStream.ReadNumber(params.lWaveform);
                        break;
                    }
                    case Chorus::DELAY:
                    {
                        argStream.ReadNumber(params.fDelay);
                        break;
                    }
                    case Chorus::PHASE:
                    {
                        argStream.ReadNumber(params.lPhase);
                        break;
                    }
                }

                if (argStream.HasErrors())
                    break;

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_COMPRESSOR:
            {
                BASS_DX8_COMPRESSOR params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Compressor eEffectParameter;
                argStream.ReadEnumString(eEffectParameter);
                switch (eEffectParameter)
                {
                    case Compressor::GAIN:
                    {
                        argStream.ReadNumber(params.fGain);
                        break;
                    }
                    case Compressor::ATTACK:
                    {
                        argStream.ReadNumber(params.fAttack);
                        break;
                    }
                    case Compressor::RELEASE:
                    {
                        argStream.ReadNumber(params.fRelease);
                        break;
                    }
                    case Compressor::THRESHOLD:
                    {
                        argStream.ReadNumber(params.fThreshold);
                        break;
                    }
                    case Compressor::RATIO:
                    {
                        argStream.ReadNumber(params.fRatio);
                        break;
                    }
                    case Compressor::PREDELAY:
                    {
                        argStream.ReadNumber(params.fPredelay);
                        break;
                    }
                }

                if (argStream.HasErrors())
                    break;

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_DISTORTION:
            {
                BASS_DX8_DISTORTION params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Distortion eEffectParameter;
                argStream.ReadEnumString(eEffectParameter);
                switch (eEffectParameter)
                {
                    case Distortion::GAIN:
                    {
                        argStream.ReadNumber(params.fGain);
                        break;
                    }
                    case Distortion::EDGE:
                    {
                        argStream.ReadNumber(params.fEdge);
                        break;
                    }
                    case Distortion::POST_EQ_CENTER_FREQUENCY:
                    {
                        argStream.ReadNumber(params.fPostEQCenterFrequency);
                        break;
                    }
                    case Distortion::POST_EQ_BANDWIDTH:
                    {
                        argStream.ReadNumber(params.fPostEQBandwidth);
                        break;
                    }
                    case Distortion::PRE_LOWPASS_CUTOFF:
                    {
                        argStream.ReadNumber(params.fPreLowpassCutoff);
                        break;
                    }
                }

                if (argStream.HasErrors())
                    break;

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_ECHO:
            {
                BASS_DX8_ECHO params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Echo eEffectParameter;
                argStream.ReadEnumString(eEffectParameter);
                switch (eEffectParameter)
                {
                    case Echo::WET_DRY_MIX:
                    {
                        argStream.ReadNumber(params.fWetDryMix);
                        break;
                    }
                    case Echo::FEEDBACK:
                    {
                        argStream.ReadNumber(params.fFeedback);
                        break;
                    }
                    case Echo::LEFT_DELAY:
                    {
                        argStream.ReadNumber(params.fLeftDelay);
                        break;
                    }
                    case Echo::RIGHT_DELAY:
                    {
                        argStream.ReadNumber(params.fRightDelay);
                        break;
                    }
                    case Echo::PAN_DELAY:
                    {
                        bool bPanDelay;
                        argStream.ReadBool(bPanDelay);
                        params.lPanDelay = bPanDelay;
                        break;
                    }
                }

                if (argStream.HasErrors())
                    break;

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_FLANGER:
            {
                BASS_DX8_FLANGER params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Flanger eEffectParameter;
                argStream.ReadEnumString(eEffectParameter);
                switch (eEffectParameter)
                {
                    case Flanger::WET_DRY_MIX:
                    {
                        argStream.ReadNumber(params.fWetDryMix);
                        break;
                    }
                    case Flanger::DEPTH:
                    {
                        argStream.ReadNumber(params.fDepth);
                        break;
                    }
                    case Flanger::FEEDBACK:
                    {
                        argStream.ReadNumber(params.fFeedback);
                        break;
                    }
                    case Flanger::FREQUENCY:
                    {
                        argStream.ReadNumber(params.fFrequency);
                        break;
                    }
                    case Flanger::WAVEFORM:
                    {
                        argStream.ReadNumber(params.lWaveform);
                        break;
                    }
                    case Flanger::DELAY:
                    {
                        argStream.ReadNumber(params.fDelay);
                        break;
                    }
                    case Flanger::PHASE:
                    {
                        argStream.ReadNumber(params.lPhase);
                        break;
                    }
                }

                if (argStream.HasErrors())
                    break;

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_GARGLE:
            {
                BASS_DX8_GARGLE params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Gargle eEffectParameter;
                argStream.ReadEnumString(eEffectParameter);
                switch (eEffectParameter)
                {
                    case Gargle::RATE_HZ:
                    {
                        argStream.ReadNumber(params.dwRateHz);
                        break;
                    }
                    case Gargle::WAVE_SHAPE:
                    {
                        argStream.ReadNumber(params.dwWaveShape);
                        break;
                    }
                }

                if (argStream.HasErrors())
                    break;

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_I3DL2REVERB:
            {
                BASS_DX8_I3DL2REVERB params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                I3DL2Reverb eEffectParameter;
                argStream.ReadEnumString(eEffectParameter);
                switch (eEffectParameter)
                {
                    case I3DL2Reverb::ROOM:
                    {
                        argStream.ReadNumber(params.lRoom);
                        break;
                    }
                    case I3DL2Reverb::ROOM_HF:
                    {
                        argStream.ReadNumber(params.lRoomHF);
                        break;
                    }
                    case I3DL2Reverb::ROOM_ROLLOFF_FACTOR:
                    {
                        argStream.ReadNumber(params.flRoomRolloffFactor);
                        break;
                    }
                    case I3DL2Reverb::DECAY_TIME:
                    {
                        argStream.ReadNumber(params.flDecayTime);
                        break;
                    }
                    case I3DL2Reverb::DECAY_HF_RATIO:
                    {
                        argStream.ReadNumber(params.flDecayHFRatio);
                        break;
                    }
                    case I3DL2Reverb::REFLECTIONS:
                    {
                        argStream.ReadNumber(params.lReflections);
                        break;
                    }
                    case I3DL2Reverb::REFLECTIONS_DELAY:
                    {
                        argStream.ReadNumber(params.flReflectionsDelay);
                        break;
                    }
                    case I3DL2Reverb::REVERB:
                    {
                        argStream.ReadNumber(params.lReverb);
                        break;
                    }
                    case I3DL2Reverb::REVERB_DELAY:
                    {
                        argStream.ReadNumber(params.flReverbDelay);
                        break;
                    }
                    case I3DL2Reverb::DIFFUSION:
                    {
                        argStream.ReadNumber(params.flDiffusion);
                        break;
                    }
                    case I3DL2Reverb::DENSITY:
                    {
                        argStream.ReadNumber(params.flDensity);
                        break;
                    }
                    case I3DL2Reverb::HF_REFERENCE:
                    {
                        argStream.ReadNumber(params.flHFReference);
                        break;
                    }
                }

                if (argStream.HasErrors())
                    break;

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_PARAMEQ:
            {
                BASS_DX8_PARAMEQ params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                ParamEq eEffectParameter;
                argStream.ReadEnumString(eEffectParameter);
                switch (eEffectParameter)
                {
                    case ParamEq::CENTER:
                    {
                        argStream.ReadNumber(params.fCenter);
                        break;
                    }
                    case ParamEq::BANDWIDTH:
                    {
                        argStream.ReadNumber(params.fBandwidth);
                        break;
                    }
                    case ParamEq::GAIN:
                    {
                        argStream.ReadNumber(params.fGain);
                        break;
                    }
                }

                if (argStream.HasErrors())
                    break;

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
            case SoundEffectType::FX_DX8_REVERB:
            {
                BASS_DX8_REVERB params;
                pSound->GetFxEffectParameters((std::uint32_t)eEffectType, &params);

                Reverb eEffectParameter;
                argStream.ReadEnumString(eEffectParameter);
                switch (eEffectParameter)
                {
                    case Reverb::IN_GAIN:
                    {
                        argStream.ReadNumber(params.fInGain);
                        break;
                    }
                    case Reverb::REVERB_MIX:
                    {
                        argStream.ReadNumber(params.fReverbMix);
                        break;
                    }
                    case Reverb::REVERB_TIME:
                    {
                        argStream.ReadNumber(params.fReverbTime);
                        break;
                    }
                    case Reverb::HIGH_FREQ_RT_RATIO:
                    {
                        argStream.ReadNumber(params.fHighFreqRTRatio);
                        break;
                    }
                }

                if (argStream.HasErrors())
                    break;

                return SetParamWithErrorLog(pSound, eEffectParameter, params);
            }
        }

        lua_pushboolean(luaVM, false);
        return 1;
    };
   
    if (argStream.NextIsUserDataOfType<CClientSound>())
        argStream.ReadUserData(pSound);
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
        argStream.ReadUserData(playerVoice.pPlayer);
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }
    argStream.ReadEnumString(eEffectType);

    if (!argStream.HasErrors())
    {
        if (pSound)
            return ProcessSoundParams(pSound);
        else if (playerVoice.pPlayer)
            return ProcessSoundParams(&playerVoice);
        else
            assert(nullptr && "Unreachable");
    }

    m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundEffectParameters(lua_State* luaVM)
{
    //  table getSoundEffectParameters ( sound/player sound, string effectName )
    CClientSound* pSound{};
    SPlayerVoiceWrapper playerVoice;
    SoundEffectType eEffectType;

    CScriptArgReader argStream(luaVM);

    const auto ProcessSoundParams = [luaVM, &eEffectType](auto* pSound) {
        if (!pSound->IsFxEffectEnabled((std::uint32_t)eEffectType))
            return luaL_error(luaVM, "Effect's parameters can't be set unless it's enabled");

        using namespace eSoundEffectParams;
        switch (eEffectType)
        {
            case SoundEffectType::FX_DX8_CHORUS:
            {
                BASS_DX8_CHORUS fxChorusParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxChorusParams))
                {
                    lua_createtable(luaVM, 0, 7);

                    lua_pushnumber(luaVM, fxChorusParams.fWetDryMix);
                    lua_setfield(luaVM, -2, EnumToString(Chorus::WET_DRY_MIX));

                    lua_pushnumber(luaVM, fxChorusParams.fDepth);
                    lua_setfield(luaVM, -2, EnumToString(Chorus::DEPTH));

                    lua_pushnumber(luaVM, fxChorusParams.fFeedback);
                    lua_setfield(luaVM, -2, EnumToString(Chorus::FEEDBACK));

                    lua_pushnumber(luaVM, fxChorusParams.fFrequency);
                    lua_setfield(luaVM, -2, EnumToString(Chorus::FREQUENCY));

                    lua_pushnumber(luaVM, fxChorusParams.lWaveform);
                    lua_setfield(luaVM, -2, EnumToString(Chorus::WAVEFORM));

                    lua_pushnumber(luaVM, fxChorusParams.fDelay);
                    lua_setfield(luaVM, -2, EnumToString(Chorus::DELAY));

                    lua_pushnumber(luaVM, fxChorusParams.lPhase);
                    lua_setfield(luaVM, -2, EnumToString(Chorus::PHASE));
                    return 1;
                }
                break;
            }
            case SoundEffectType::FX_DX8_COMPRESSOR:
            {
                BASS_DX8_COMPRESSOR fxCompressorParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxCompressorParams))
                {
                    lua_createtable(luaVM, 0, 6);

                    lua_pushnumber(luaVM, fxCompressorParams.fGain);
                    lua_setfield(luaVM, -2, EnumToString(Compressor::GAIN));

                    lua_pushnumber(luaVM, fxCompressorParams.fAttack);
                    lua_setfield(luaVM, -2, EnumToString(Compressor::ATTACK));

                    lua_pushnumber(luaVM, fxCompressorParams.fRelease);
                    lua_setfield(luaVM, -2, EnumToString(Compressor::RELEASE));

                    lua_pushnumber(luaVM, fxCompressorParams.fThreshold);
                    lua_setfield(luaVM, -2, EnumToString(Compressor::THRESHOLD));

                    lua_pushnumber(luaVM, fxCompressorParams.fRatio);
                    lua_setfield(luaVM, -2, EnumToString(Compressor::RATIO));

                    lua_pushnumber(luaVM, fxCompressorParams.fPredelay);
                    lua_setfield(luaVM, -2, EnumToString(Compressor::PREDELAY));
                    return 1;
                }
                break;
            }
            case SoundEffectType::FX_DX8_DISTORTION:
            {
                BASS_DX8_DISTORTION fxDistortionParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxDistortionParams))
                {
                    lua_createtable(luaVM, 0, 5);

                    lua_pushnumber(luaVM, fxDistortionParams.fGain);
                    lua_setfield(luaVM, -2, EnumToString(Distortion::GAIN));

                    lua_pushnumber(luaVM, fxDistortionParams.fEdge);
                    lua_setfield(luaVM, -2, EnumToString(Distortion::EDGE));

                    lua_pushnumber(luaVM, fxDistortionParams.fPostEQCenterFrequency);
                    lua_setfield(luaVM, -2, EnumToString(Distortion::POST_EQ_CENTER_FREQUENCY));

                    lua_pushnumber(luaVM, fxDistortionParams.fPostEQBandwidth);
                    lua_setfield(luaVM, -2, EnumToString(Distortion::POST_EQ_BANDWIDTH));

                    lua_pushnumber(luaVM, fxDistortionParams.fPreLowpassCutoff);
                    lua_setfield(luaVM, -2, EnumToString(Distortion::PRE_LOWPASS_CUTOFF));
                    return 1;
                }
                break;
            }
            case SoundEffectType::FX_DX8_ECHO:
            {
                BASS_DX8_ECHO fxEchoParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxEchoParams))
                {
                    lua_createtable(luaVM, 0, 5);

                    lua_pushnumber(luaVM, fxEchoParams.fWetDryMix);
                    lua_setfield(luaVM, -2, EnumToString(Echo::WET_DRY_MIX));

                    lua_pushnumber(luaVM, fxEchoParams.fFeedback);
                    lua_setfield(luaVM, -2, EnumToString(Echo::FEEDBACK));

                    lua_pushnumber(luaVM, fxEchoParams.fLeftDelay);
                    lua_setfield(luaVM, -2, EnumToString(Echo::LEFT_DELAY));

                    lua_pushnumber(luaVM, fxEchoParams.fRightDelay);
                    lua_setfield(luaVM, -2, EnumToString(Echo::RIGHT_DELAY));

                    lua_pushboolean(luaVM, fxEchoParams.lPanDelay);
                    lua_setfield(luaVM, -2, EnumToString(Echo::PAN_DELAY));
                    return 1;
                }
                break;
            }
            case SoundEffectType::FX_DX8_FLANGER:
            {
                BASS_DX8_FLANGER fxFlangerParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxFlangerParams))
                {
                    lua_createtable(luaVM, 0, 7);

                    lua_pushnumber(luaVM, fxFlangerParams.fWetDryMix);
                    lua_setfield(luaVM, -2, EnumToString(Flanger::WET_DRY_MIX));

                    lua_pushnumber(luaVM, fxFlangerParams.fDepth);
                    lua_setfield(luaVM, -2, EnumToString(Flanger::DEPTH));

                    lua_pushnumber(luaVM, fxFlangerParams.fFeedback);
                    lua_setfield(luaVM, -2, EnumToString(Flanger::FEEDBACK));

                    lua_pushnumber(luaVM, fxFlangerParams.fFrequency);
                    lua_setfield(luaVM, -2, EnumToString(Flanger::FREQUENCY));

                    lua_pushnumber(luaVM, fxFlangerParams.lWaveform);
                    lua_setfield(luaVM, -2, EnumToString(Flanger::WAVEFORM));

                    lua_pushnumber(luaVM, fxFlangerParams.fDelay);
                    lua_setfield(luaVM, -2, EnumToString(Flanger::DELAY));

                    lua_pushnumber(luaVM, fxFlangerParams.lPhase);
                    lua_setfield(luaVM, -2, EnumToString(Flanger::PHASE));
                    return 1;
                }
                break;
            }
            case SoundEffectType::FX_DX8_GARGLE:
            {
                BASS_DX8_GARGLE fxGargleParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxGargleParams))
                {
                    lua_createtable(luaVM, 0, 2);

                    lua_pushnumber(luaVM, fxGargleParams.dwRateHz);
                    lua_setfield(luaVM, -2, EnumToString(Gargle::RATE_HZ));

                    lua_pushnumber(luaVM, fxGargleParams.dwWaveShape);
                    lua_setfield(luaVM, -2, EnumToString(Gargle::WAVE_SHAPE));
                    return 1;
                }
                break;
            }
            case SoundEffectType::FX_DX8_I3DL2REVERB:
            {
                BASS_DX8_I3DL2REVERB fxI3DL2ReverbParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxI3DL2ReverbParams))
                {
                    lua_createtable(luaVM, 0, 12);

                    lua_pushnumber(luaVM, fxI3DL2ReverbParams.lRoom);
                    lua_setfield(luaVM, -2, EnumToString(I3DL2Reverb::ROOM));

                    lua_pushnumber(luaVM, fxI3DL2ReverbParams.lRoomHF);
                    lua_setfield(luaVM, -2, EnumToString(I3DL2Reverb::ROOM_HF));

                    lua_pushnumber(luaVM, fxI3DL2ReverbParams.flRoomRolloffFactor);
                    lua_setfield(luaVM, -2, EnumToString(I3DL2Reverb::ROOM_ROLLOFF_FACTOR));

                    lua_pushnumber(luaVM, fxI3DL2ReverbParams.flDecayTime);
                    lua_setfield(luaVM, -2, EnumToString(I3DL2Reverb::DECAY_TIME));

                    lua_pushnumber(luaVM, fxI3DL2ReverbParams.flDecayHFRatio);
                    lua_setfield(luaVM, -2, EnumToString(I3DL2Reverb::DECAY_HF_RATIO));

                    lua_pushnumber(luaVM, fxI3DL2ReverbParams.lReflections);
                    lua_setfield(luaVM, -2, EnumToString(I3DL2Reverb::REFLECTIONS));

                    lua_pushnumber(luaVM, fxI3DL2ReverbParams.flReflectionsDelay);
                    lua_setfield(luaVM, -2, EnumToString(I3DL2Reverb::REFLECTIONS_DELAY));

                    lua_pushnumber(luaVM, fxI3DL2ReverbParams.lReverb);
                    lua_setfield(luaVM, -2, EnumToString(I3DL2Reverb::REVERB));

                    lua_pushnumber(luaVM, fxI3DL2ReverbParams.flReverbDelay);
                    lua_setfield(luaVM, -2, EnumToString(I3DL2Reverb::REVERB_DELAY));

                    lua_pushnumber(luaVM, fxI3DL2ReverbParams.flDiffusion);
                    lua_setfield(luaVM, -2, EnumToString(I3DL2Reverb::DIFFUSION));

                    lua_pushnumber(luaVM, fxI3DL2ReverbParams.flDensity);
                    lua_setfield(luaVM, -2, EnumToString(I3DL2Reverb::DENSITY));

                    lua_pushnumber(luaVM, fxI3DL2ReverbParams.flHFReference);
                    lua_setfield(luaVM, -2, EnumToString(I3DL2Reverb::HF_REFERENCE));
                    return 1;
                }
                break;
            }
            case SoundEffectType::FX_DX8_PARAMEQ:
            {
                BASS_DX8_PARAMEQ fxParameqParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxParameqParams))
                {
                    lua_createtable(luaVM, 0, 3);

                    lua_pushnumber(luaVM, fxParameqParams.fCenter);
                    lua_setfield(luaVM, -2, EnumToString(ParamEq::CENTER));

                    lua_pushnumber(luaVM, fxParameqParams.fBandwidth);
                    lua_setfield(luaVM, -2, EnumToString(ParamEq::BANDWIDTH));

                    lua_pushnumber(luaVM, fxParameqParams.fGain);
                    lua_setfield(luaVM, -2, EnumToString(ParamEq::GAIN));
                    return 1;
                }
                break;
            }
            case SoundEffectType::FX_DX8_REVERB:
            {
                BASS_DX8_REVERB fxReverbParams;
                if (pSound->GetFxEffectParameters((uint)eEffectType, &fxReverbParams))
                {
                    lua_createtable(luaVM, 0, 4);

                    lua_pushnumber(luaVM, fxReverbParams.fInGain);
                    lua_setfield(luaVM, -2, EnumToString(Reverb::IN_GAIN));

                    lua_pushnumber(luaVM, fxReverbParams.fReverbMix);
                    lua_setfield(luaVM, -2, EnumToString(Reverb::REVERB_MIX));

                    lua_pushnumber(luaVM, fxReverbParams.fReverbTime);
                    lua_setfield(luaVM, -2, EnumToString(Reverb::REVERB_TIME));

                    lua_pushnumber(luaVM, fxReverbParams.fHighFreqRTRatio);
                    lua_setfield(luaVM, -2, EnumToString(Reverb::HIGH_FREQ_RT_RATIO));
                    return 1;
                }
                break;
            }
        }

        lua_pushboolean(luaVM, false);
        return 1;
    };

    if (argStream.NextIsUserDataOfType<CClientSound>())
        argStream.ReadUserData(pSound);
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
        argStream.ReadUserData(playerVoice.pPlayer);
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return false;
    }
    argStream.ReadEnumString(eEffectType);

    if (!argStream.HasErrors())
    {
        if (pSound)
            return ProcessSoundParams(pSound);
        else if (playerVoice.pPlayer)
            return ProcessSoundParams(&playerVoice);
        else
            assert(nullptr && "Unreachable");
    }

    m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::PlaySoundFrontEnd(lua_State* luaVM)
{
    CClientSound*    pSound = NULL;
    CVector          vecPosition;
    unsigned char    ucSound = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ucSound);

    if (!argStream.HasErrors())
    {
        if (ucSound <= 101)
        {
            if (CStaticFunctionDefinitions::PlaySoundFrontEnd(ucSound))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogError(luaVM, "Invalid sound ID specified. Valid sound IDs are 0 - 101.");
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
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

int CLuaAudioDefs::PlaySFX(lua_State* luaVM)
{
    //  sound playSFX ( string audioContainer, int bankIndex, int audioIndex [, loop = false ] )
    eAudioLookupIndex containerIndex;
    int               iBankIndex;
    int               iAudioIndex;
    bool              bLoop;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(containerIndex);

    if (!argStream.HasErrors() && containerIndex == AUDIO_LOOKUP_RADIO)
        argStream.ReadEnumString<eRadioStreamIndex>((eRadioStreamIndex&)iBankIndex);
    else
        argStream.ReadNumber(iBankIndex);

    argStream.ReadNumber(iAudioIndex);
    argStream.ReadBool(bLoop, false);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                CClientSound* pSound;
                if (CStaticFunctionDefinitions::PlaySFX(pResource, containerIndex, iBankIndex, iAudioIndex, bLoop, pSound))
                {
                    lua_pushelement(luaVM, pSound);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::PlaySFX3D(lua_State* luaVM)
{
    //  sound playSFX3D ( string audioContainer, int bankIndex, int audioIndex, float posX, float posY, float posZ [, loop = false ] )
    eAudioLookupIndex containerIndex;
    int               iBankIndex;
    int               iAudioIndex;
    CVector           vecPosition;
    bool              bLoop;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(containerIndex);

    if (!argStream.HasErrors() && containerIndex == AUDIO_LOOKUP_RADIO)
        argStream.ReadEnumString<eRadioStreamIndex>((eRadioStreamIndex&)iBankIndex);
    else
        argStream.ReadNumber(iBankIndex);

    argStream.ReadNumber(iAudioIndex);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadBool(bLoop, false);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                CClientSound* pSound;
                if (CStaticFunctionDefinitions::PlaySFX3D(pResource, containerIndex, iBankIndex, iAudioIndex, vecPosition, bLoop, pSound))
                {
                    lua_pushelement(luaVM, pSound);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSFXStatus(lua_State* luaVM)
{
    //  bool getSFXStatus ( string audioContainer )
    eAudioLookupIndex containerIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(containerIndex);

    if (!argStream.HasErrors())
    {
        bool bNotCut;
        if (CStaticFunctionDefinitions::GetSFXStatus(containerIndex, bNotCut))
        {
            lua_pushboolean(luaVM, bNotCut);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaAudioDefs::SetSoundPan(lua_State* luaVM)
{
    //  setSoundPan ( sound theSound, float pan )
    //  setSoundPan ( player thePlayer, float pan )
    CClientSound*  pSound = NULL;
    CClientPlayer* pPlayer = NULL;
    float          fPan;

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return 1;
    }

    argStream.ReadNumber(fPan);

    if (!argStream.HasErrors())
    {
        if (pSound && CStaticFunctionDefinitions::SetSoundPan(*pSound, fPan))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
        else if (pPlayer && CStaticFunctionDefinitions::SetSoundPan(*pPlayer, fPan))
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

int CLuaAudioDefs::GetSoundPan(lua_State* luaVM)
{
    //  getSoundPan ( element theSound )
    //  getSoundPan ( player thePlayer )
    CClientSound*  pSound = NULL;
    CClientPlayer* pPlayer = NULL;

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientSound>())
    {
        argStream.ReadUserData(pSound);
    }
    else if (argStream.NextIsUserDataOfType<CClientPlayer>())
    {
        argStream.ReadUserData(pPlayer);
    }
    else
    {
        m_pScriptDebugging->LogBadPointer(luaVM, "sound/player", 1);
        lua_pushboolean(luaVM, false);
        return 1;
    }

    if (!argStream.HasErrors())
    {
        float fPan = 0.0;
        if (pSound && CStaticFunctionDefinitions::GetSoundPan(*pSound, fPan))
        {
            lua_pushnumber(luaVM, fPan);
            return 1;
        }
        else if (pPlayer && CStaticFunctionDefinitions::GetSoundPan(*pPlayer, fPan))
        {
            lua_pushnumber(luaVM, fPan);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

// Radio
int CLuaAudioDefs::SetRadioChannel(lua_State* luaVM)
{
    unsigned char    ucChannel = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ucChannel);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetRadioChannel(ucChannel))
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

int CLuaAudioDefs::GetRadioChannel(lua_State* luaVM)
{
    unsigned char ucChannel = 0;
    if (CStaticFunctionDefinitions::GetRadioChannel(ucChannel))
    {
        lua_pushnumber(luaVM, ucChannel);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetRadioChannelName(lua_State* luaVM)
{
    static const SFixedArray<const char*, 13> szRadioStations = {{"Radio off", "Playback FM", "K-Rose", "K-DST", "Bounce FM", "SF-UR", "Radio Los Santos",
                                                                  "Radio X", "CSR 103.9", "K-Jah West", "Master Sounds 98.3", "WCTR", "User Track Player"}};

    int              iChannel = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iChannel);

    if (!argStream.HasErrors())
    {
        if (iChannel >= 0 && iChannel < NUMELMS(szRadioStations))
        {
            lua_pushstring(luaVM, szRadioStations[iChannel]);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
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
