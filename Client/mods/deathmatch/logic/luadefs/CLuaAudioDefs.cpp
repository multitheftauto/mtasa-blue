/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaAudioDefs.cpp
 *  PURPOSE:     Lua audio definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CLuaAudioDefs::LoadFunctions()
{
    std::map<const char*, lua_CFunction> functions{
        // Audio funcs
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
    };

    // Add functions
    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
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
    lua_classfunction(luaVM, "setPan", "setSoundPan");
    lua_classfunction(luaVM, "setPannningEnabled", "setSoundPanningEnabled");
    lua_classfunction(luaVM, "setProperties", "setSoundProperties");

    lua_classfunction(luaVM, "getLength", "getSoundLength");
    lua_classfunction(luaVM, "getBufferLength", "getSoundBufferLength");
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
                if (CResourceManager::ParseResourcePathInput(strSound, pResource, &strFilename))
                    strSound = strFilename;
                else
                    bIsURL = true;

                // ParseResourcePathInput changes pResource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3"
                // ) Fixes #6507 - Caz
                if (pResource)
                {
                    CClientSound* pSound = CStaticFunctionDefinitions::PlaySound(pResource, strSound, bIsURL, bLoop, bThrottle);
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
                if (CResourceManager::ParseResourcePathInput(strSound, pResource, &strFilename))
                    strSound = strFilename;
                else
                    bIsURL = true;

                // ParseResourcePathInput changes pResource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3"
                // ) Fixes #6507 - Caz
                if (pResource)
                {
                    CClientSound* pSound = CStaticFunctionDefinitions::PlaySound3D(pResource, strSound, bIsURL, vecPosition, bLoop, bThrottle);
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

int CLuaAudioDefs::SetSoundEffectParameter(lua_State* luaVM)
{
    //  bool setSoundEffectParameter ( sound/player sound, string effectName, string effectParameter, var effectParameterValue  )
    CClientPlayer* pPlayer;
    CClientSound*  pSound;
    SString        strEffectName;
    SString        strEffectParameterName;

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
    argStream.ReadString(strEffectParameterName);

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            int iFxEffect = m_pManager->GetSoundManager()->GetFxEffectFromName(strEffectName);
            if (iFxEffect >= 0 && pSound->IsFxEffectEnabled(iFxEffect))
            {
                switch (iFxEffect)
                {
                    case BASS_FX_DX8_CHORUS:
                    {
                        BASS_DX8_CHORUS params;
                        CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &params);
                        if (strEffectParameterName == "wetDryMix")
                        {
                            float fWetDryMix;
                            argStream.ReadNumber(fWetDryMix);
                            if (!argStream.HasErrors())
                            {
                                params.fWetDryMix = fWetDryMix;
                            }
                        }
                        else if (strEffectParameterName == "depth")
                        {
                            float fDepth;
                            argStream.ReadNumber(fDepth);
                            if (!argStream.HasErrors())
                            {
                                params.fDepth = fDepth;
                            }
                        }
                        else if (strEffectParameterName == "feedback")
                        {
                            float fFeedback;
                            argStream.ReadNumber(fFeedback);
                            if (!argStream.HasErrors())
                            {
                                params.fFeedback = fFeedback;
                            }
                        }
                        else if (strEffectParameterName == "frequency")
                        {
                            float fFrequency;
                            argStream.ReadNumber(fFrequency);
                            if (!argStream.HasErrors())
                            {
                                params.fFrequency = fFrequency;
                            }
                        }
                        else if (strEffectParameterName == "waveform")
                        {
                            uint uiWaveform;
                            argStream.ReadNumber(uiWaveform);
                            if (!argStream.HasErrors())
                            {
                                params.lWaveform = uiWaveform;
                            }
                        }
                        else if (strEffectParameterName == "delay")
                        {
                            float fDelay;
                            argStream.ReadNumber(fDelay);
                            if (!argStream.HasErrors())
                            {
                                params.fDelay = fDelay;
                            }
                        }
                        else if (strEffectParameterName == "phase")
                        {
                            uint uiPhase;
                            argStream.ReadNumber(uiPhase);
                            if (!argStream.HasErrors())
                            {
                                params.lPhase = uiPhase;
                            }
                        }
                        else
                        {
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }
                        if (CStaticFunctionDefinitions::SetSoundEffectParameters(*pSound, strEffectName, &params))
                        {
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        break;
                    }
                    case BASS_FX_DX8_COMPRESSOR:
                    {
                        BASS_DX8_COMPRESSOR params;
                        CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &params);
                        if (strEffectParameterName == "gain")
                        {
                            float fGain;
                            argStream.ReadNumber(fGain);
                            if (!argStream.HasErrors())
                            {
                                params.fGain = fGain;
                            }
                        }
                        else if (strEffectParameterName == "attack")
                        {
                            float fAttack;
                            argStream.ReadNumber(fAttack);
                            if (!argStream.HasErrors())
                            {
                                params.fAttack = fAttack;
                            }
                        }
                        else if (strEffectParameterName == "release")
                        {
                            float fRelease;
                            argStream.ReadNumber(fRelease);
                            if (!argStream.HasErrors())
                            {
                                params.fRelease = fRelease;
                            }
                        }
                        else if (strEffectParameterName == "threshold")
                        {
                            float fThreshold;
                            argStream.ReadNumber(fThreshold);
                            if (!argStream.HasErrors())
                            {
                                params.fThreshold = fThreshold;
                            }
                        }
                        else if (strEffectParameterName == "ratio")
                        {
                            float fRatio;
                            argStream.ReadNumber(fRatio);
                            if (!argStream.HasErrors())
                            {
                                params.fRatio = fRatio;
                            }
                        }
                        else if (strEffectParameterName == "predelay")
                        {
                            float fPredelay;
                            argStream.ReadNumber(fPredelay);
                            if (!argStream.HasErrors())
                            {
                                params.fPredelay = fPredelay;
                            }
                        }
                        else
                        {
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }
                        if (CStaticFunctionDefinitions::SetSoundEffectParameters(*pSound, strEffectName, &params))
                        {
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        break;
                    }
                    case BASS_FX_DX8_DISTORTION:
                    {
                        BASS_DX8_DISTORTION params;
                        CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &params);
                        if (strEffectParameterName == "gain")
                        {
                            float fGain;
                            argStream.ReadNumber(fGain);
                            if (!argStream.HasErrors())
                            {
                                params.fGain = fGain;
                            }
                        }
                        else if (strEffectParameterName == "edge")
                        {
                            float fEdge;
                            argStream.ReadNumber(fEdge);
                            if (!argStream.HasErrors())
                            {
                                params.fEdge = fEdge;
                            }
                        }
                        else if (strEffectParameterName == "postEQCenterFrequency")
                        {
                            float fPostEQCenterFrequency;
                            argStream.ReadNumber(fPostEQCenterFrequency);
                            if (!argStream.HasErrors())
                            {
                                params.fPostEQCenterFrequency = fPostEQCenterFrequency;
                            }
                        }
                        else if (strEffectParameterName == "postEQBandwidth")
                        {
                            float fPostEQBandwidth;
                            argStream.ReadNumber(fPostEQBandwidth);
                            if (!argStream.HasErrors())
                            {
                                params.fPostEQBandwidth = fPostEQBandwidth;
                            }
                        }
                        else if (strEffectParameterName == "preLowpassCutoff")
                        {
                            float fPreLowpassCutoff;
                            argStream.ReadNumber(fPreLowpassCutoff);
                            if (!argStream.HasErrors())
                            {
                                params.fPreLowpassCutoff = fPreLowpassCutoff;
                            }
                        }
                        else
                        {
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }
                        if (CStaticFunctionDefinitions::SetSoundEffectParameters(*pSound, strEffectName, &params))
                        {
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        break;
                    }

                    case BASS_FX_DX8_ECHO:
                    {
                        BASS_DX8_ECHO params;
                        CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &params);
                        if (strEffectParameterName == "wetDryMix")
                        {
                            float fWetDryMix;
                            argStream.ReadNumber(fWetDryMix);
                            if (!argStream.HasErrors())
                            {
                                params.fWetDryMix = fWetDryMix;
                            }
                        }
                        else if (strEffectParameterName == "feedback")
                        {
                            float fFeedback;
                            argStream.ReadNumber(fFeedback);
                            if (!argStream.HasErrors())
                            {
                                params.fFeedback = fFeedback;
                            }
                        }
                        else if (strEffectParameterName == "leftDelay")
                        {
                            float fLeftDelay;
                            argStream.ReadNumber(fLeftDelay);
                            if (!argStream.HasErrors())
                            {
                                params.fLeftDelay = fLeftDelay;
                            }
                        }
                        else if (strEffectParameterName == "rightDelay")
                        {
                            float fRightDelay;
                            argStream.ReadNumber(fRightDelay);
                            if (!argStream.HasErrors())
                            {
                                params.fRightDelay = fRightDelay;
                            }
                        }
                        else if (strEffectParameterName == "panDelay")
                        {
                            bool bPanDelay;
                            argStream.ReadBool(bPanDelay);
                            if (!argStream.HasErrors())
                            {
                                params.lPanDelay = bPanDelay;
                            }
                        }
                        else
                        {
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }
                        if (CStaticFunctionDefinitions::SetSoundEffectParameters(*pSound, strEffectName, &params))
                        {
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        break;
                    }
                    case BASS_FX_DX8_FLANGER:
                    {
                        BASS_DX8_FLANGER params;
                        CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &params);
                        if (strEffectParameterName == "wetDryMix")
                        {
                            float fWetDryMix;
                            argStream.ReadNumber(fWetDryMix);
                            if (!argStream.HasErrors())
                            {
                                params.fWetDryMix = fWetDryMix;
                            }
                        }
                        else if (strEffectParameterName == "depth")
                        {
                            float fDepth;
                            argStream.ReadNumber(fDepth);
                            if (!argStream.HasErrors())
                            {
                                params.fDepth = fDepth;
                            }
                        }
                        else if (strEffectParameterName == "feedback")
                        {
                            float fFeedback;
                            argStream.ReadNumber(fFeedback);
                            if (!argStream.HasErrors())
                            {
                                params.fFeedback = fFeedback;
                            }
                        }
                        else if (strEffectParameterName == "frequency")
                        {
                            float fFrequency;
                            argStream.ReadNumber(fFrequency);
                            if (!argStream.HasErrors())
                            {
                                params.fFrequency = fFrequency;
                            }
                        }
                        else if (strEffectParameterName == "waveform")
                        {
                            uint uiWaveform;
                            argStream.ReadNumber(uiWaveform);
                            if (!argStream.HasErrors())
                            {
                                params.lWaveform = uiWaveform;
                            }
                        }
                        else if (strEffectParameterName == "delay")
                        {
                            float fDelay;
                            argStream.ReadNumber(fDelay);
                            if (!argStream.HasErrors())
                            {
                                params.fDelay = fDelay;
                            }
                        }
                        else if (strEffectParameterName == "phase")
                        {
                            uint uiPhase;
                            argStream.ReadNumber(uiPhase);
                            if (!argStream.HasErrors())
                            {
                                params.lPhase = uiPhase;
                            }
                        }
                        else
                        {
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }
                        if (CStaticFunctionDefinitions::SetSoundEffectParameters(*pSound, strEffectName, &params))
                        {
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        break;
                    }
                    case BASS_FX_DX8_GARGLE:
                    {
                        BASS_DX8_GARGLE params;
                        CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &params);
                        if (strEffectParameterName == "rateHz")
                        {
                            uint uiRateHz;
                            argStream.ReadNumber(uiRateHz);
                            if (!argStream.HasErrors())
                            {
                                params.dwRateHz = uiRateHz;
                            }
                        }
                        else if (strEffectParameterName == "waveShape")
                        {
                            uint uiWaveShape;
                            argStream.ReadNumber(uiWaveShape);
                            if (!argStream.HasErrors())
                            {
                                params.dwWaveShape = uiWaveShape;
                            }
                        }
                        else
                        {
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }
                        if (CStaticFunctionDefinitions::SetSoundEffectParameters(*pSound, strEffectName, &params))
                        {
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        break;
                    }
                    case BASS_FX_DX8_I3DL2REVERB:
                    {
                        BASS_DX8_I3DL2REVERB params;
                        CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &params);
                        if (strEffectParameterName == "room")
                        {
                            int iRoom;
                            argStream.ReadNumber(iRoom);
                            if (!argStream.HasErrors())
                            {
                                params.lRoom = iRoom;
                            }
                        }
                        else if (strEffectParameterName == "roomHF")
                        {
                            int iRoomHF;
                            argStream.ReadNumber(iRoomHF);
                            if (!argStream.HasErrors())
                            {
                                params.lRoomHF = iRoomHF;
                            }
                        }
                        else if (strEffectParameterName == "roomRolloffFactor")
                        {
                            float fRoomRolloffFactor;
                            argStream.ReadNumber(fRoomRolloffFactor);
                            if (!argStream.HasErrors())
                            {
                                params.flRoomRolloffFactor = fRoomRolloffFactor;
                            }
                        }
                        else if (strEffectParameterName == "decayTime")
                        {
                            float fDecayTime;
                            argStream.ReadNumber(fDecayTime);
                            if (!argStream.HasErrors())
                            {
                                params.flDecayTime = fDecayTime;
                            }
                        }
                        else if (strEffectParameterName == "decayHFRatio")
                        {
                            float fDecayHFRatio;
                            argStream.ReadNumber(fDecayHFRatio);
                            if (!argStream.HasErrors())
                            {
                                params.flDecayHFRatio = fDecayHFRatio;
                            }
                        }
                        else if (strEffectParameterName == "reflections")
                        {
                            int iReflections;
                            argStream.ReadNumber(iReflections);
                            if (!argStream.HasErrors())
                            {
                                params.lReflections = iReflections;
                            }
                        }
                        else if (strEffectParameterName == "reflectionsDelay")
                        {
                            float fReflectionsDelay;
                            argStream.ReadNumber(fReflectionsDelay);
                            if (!argStream.HasErrors())
                            {
                                params.flReflectionsDelay = fReflectionsDelay;
                            }
                        }
                        else if (strEffectParameterName == "reverb")
                        {
                            int iReverb;
                            argStream.ReadNumber(iReverb);
                            if (!argStream.HasErrors())
                            {
                                params.lReverb = iReverb;
                            }
                        }
                        else if (strEffectParameterName == "reverbDelay")
                        {
                            float fReverbDelay;
                            argStream.ReadNumber(fReverbDelay);
                            if (!argStream.HasErrors())
                            {
                                params.flReverbDelay = fReverbDelay;
                            }
                        }
                        else if (strEffectParameterName == "diffusion")
                        {
                            float fDiffusion;
                            argStream.ReadNumber(fDiffusion);
                            if (!argStream.HasErrors())
                            {
                                params.flDiffusion = fDiffusion;
                            }
                        }
                        else if (strEffectParameterName == "density")
                        {
                            float fDensity;
                            argStream.ReadNumber(fDensity);
                            if (!argStream.HasErrors())
                            {
                                params.flDensity = fDensity;
                            }
                        }
                        else if (strEffectParameterName == "HFReference")
                        {
                            float fHFReference;
                            argStream.ReadNumber(fHFReference);
                            if (!argStream.HasErrors())
                            {
                                params.flHFReference = fHFReference;
                            }
                        }
                        else
                        {
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }
                        if (CStaticFunctionDefinitions::SetSoundEffectParameters(*pSound, strEffectName, &params))
                        {
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        break;
                    }
                    case BASS_FX_DX8_PARAMEQ:
                    {
                        BASS_DX8_PARAMEQ params;
                        CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &params);
                        if (strEffectParameterName == "center")
                        {
                            float fCenter;
                            argStream.ReadNumber(fCenter);
                            if (!argStream.HasErrors())
                            {
                                params.fCenter = fCenter;
                            }
                        }
                        else if (strEffectParameterName == "bandwidth")
                        {
                            float fBandwidth;
                            argStream.ReadNumber(fBandwidth);
                            if (!argStream.HasErrors())
                            {
                                params.fBandwidth = fBandwidth;
                            }
                        }
                        else if (strEffectParameterName == "gain")
                        {
                            float fGain;
                            argStream.ReadNumber(fGain);
                            if (!argStream.HasErrors())
                            {
                                params.fGain = fGain;
                            }
                        }
                        else
                        {
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }
                        if (CStaticFunctionDefinitions::SetSoundEffectParameters(*pSound, strEffectName, &params))
                        {
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        break;
                    }
                    case BASS_FX_DX8_REVERB:
                    {
                        BASS_DX8_REVERB params;
                        CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &params);
                        if (strEffectParameterName == "inGain")
                        {
                            float fInGain;
                            argStream.ReadNumber(fInGain);
                            if (!argStream.HasErrors())
                            {
                                params.fInGain = fInGain;
                            }
                        }
                        else if (strEffectParameterName == "reverbMix")
                        {
                            float fReverbMix;
                            argStream.ReadNumber(fReverbMix);
                            if (!argStream.HasErrors())
                            {
                                params.fReverbMix = fReverbMix;
                            }
                        }
                        else if (strEffectParameterName == "reverbTime")
                        {
                            float fReverbTime;
                            argStream.ReadNumber(fReverbTime);
                            if (!argStream.HasErrors())
                            {
                                params.fReverbTime = fReverbTime;
                            }
                        }
                        else if (strEffectParameterName == "highFreqRTRatio")
                        {
                            float fHighFreqRTRatio;
                            argStream.ReadNumber(fHighFreqRTRatio);
                            if (!argStream.HasErrors())
                            {
                                params.fHighFreqRTRatio = fHighFreqRTRatio;
                            }
                        }
                        else
                        {
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }
                        if (CStaticFunctionDefinitions::SetSoundEffectParameters(*pSound, strEffectName, &params))
                        {
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        break;
                    }
                }
            }
        }
        else if (pPlayer)
        {
            // TODO: Make it work with player voice
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAudioDefs::GetSoundEffectParameters(lua_State* luaVM)
{
    //  table getSoundEffectParameters ( sound/player sound, string effectName )
    CClientPlayer* pPlayer;
    CClientSound*  pSound;
    SString        strEffectName;

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

    if (!argStream.HasErrors())
    {
        if (pSound)
        {
            int iFxEffect = m_pManager->GetSoundManager()->GetFxEffectFromName(strEffectName);
            switch (iFxEffect)
            {
                case BASS_FX_DX8_CHORUS:
                {
                    BASS_DX8_CHORUS fxChorusParams;
                    if (CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &fxChorusParams))
                    {
                        lua_newtable(luaVM);

                        lua_pushnumber(luaVM, fxChorusParams.fWetDryMix);
                        lua_setfield(luaVM, -2, "wetDryMix");

                        lua_pushnumber(luaVM, fxChorusParams.fDepth);
                        lua_setfield(luaVM, -2, "depth");

                        lua_pushnumber(luaVM, fxChorusParams.fFeedback);
                        lua_setfield(luaVM, -2, "feedback");

                        lua_pushnumber(luaVM, fxChorusParams.fFrequency);
                        lua_setfield(luaVM, -2, "frequency");

                        lua_pushnumber(luaVM, fxChorusParams.lWaveform);
                        lua_setfield(luaVM, -2, "waveform");

                        lua_pushnumber(luaVM, fxChorusParams.fDelay);
                        lua_setfield(luaVM, -2, "delay");

                        lua_pushnumber(luaVM, fxChorusParams.lPhase);
                        lua_setfield(luaVM, -2, "phase");
                        return 1;
                    }
                    break;
                }
                case BASS_FX_DX8_COMPRESSOR:
                {
                    BASS_DX8_COMPRESSOR fxCompressorParams;
                    if (CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &fxCompressorParams))
                    {
                        lua_newtable(luaVM);

                        lua_pushnumber(luaVM, fxCompressorParams.fGain);
                        lua_setfield(luaVM, -2, "gain");

                        lua_pushnumber(luaVM, fxCompressorParams.fAttack);
                        lua_setfield(luaVM, -2, "attack");

                        lua_pushnumber(luaVM, fxCompressorParams.fRelease);
                        lua_setfield(luaVM, -2, "release");

                        lua_pushnumber(luaVM, fxCompressorParams.fThreshold);
                        lua_setfield(luaVM, -2, "threshold");

                        lua_pushnumber(luaVM, fxCompressorParams.fRatio);
                        lua_setfield(luaVM, -2, "ratio");

                        lua_pushnumber(luaVM, fxCompressorParams.fPredelay);
                        lua_setfield(luaVM, -2, "predelay");
                        return 1;
                    }
                    break;
                }
                case BASS_FX_DX8_DISTORTION:
                {
                    BASS_DX8_DISTORTION fxDistortionParams;
                    if (CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &fxDistortionParams))
                    {
                        lua_newtable(luaVM);

                        lua_pushnumber(luaVM, fxDistortionParams.fGain);
                        lua_setfield(luaVM, -2, "gain");

                        lua_pushnumber(luaVM, fxDistortionParams.fEdge);
                        lua_setfield(luaVM, -2, "edge");

                        lua_pushnumber(luaVM, fxDistortionParams.fPostEQCenterFrequency);
                        lua_setfield(luaVM, -2, "postEQCenterFrequency");

                        lua_pushnumber(luaVM, fxDistortionParams.fPostEQBandwidth);
                        lua_setfield(luaVM, -2, "postEQBandwidth");

                        lua_pushnumber(luaVM, fxDistortionParams.fPreLowpassCutoff);
                        lua_setfield(luaVM, -2, "preLowpassCutoff");
                        return 1;
                    }
                    break;
                }
                case BASS_FX_DX8_ECHO:
                {
                    BASS_DX8_ECHO fxEchoParams;
                    if (CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &fxEchoParams))
                    {
                        lua_newtable(luaVM);

                        lua_pushnumber(luaVM, fxEchoParams.fWetDryMix);
                        lua_setfield(luaVM, -2, "wetDryMix");

                        lua_pushnumber(luaVM, fxEchoParams.fFeedback);
                        lua_setfield(luaVM, -2, "feedback");

                        lua_pushnumber(luaVM, fxEchoParams.fLeftDelay);
                        lua_setfield(luaVM, -2, "leftDelay");

                        lua_pushnumber(luaVM, fxEchoParams.fRightDelay);
                        lua_setfield(luaVM, -2, "rightDelay");

                        lua_pushboolean(luaVM, fxEchoParams.lPanDelay);
                        lua_setfield(luaVM, -2, "panDelay");
                        return 1;
                    }
                    break;
                }
                case BASS_FX_DX8_FLANGER:
                {
                    BASS_DX8_FLANGER fxFlangerParams;
                    if (CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &fxFlangerParams))
                    {
                        lua_newtable(luaVM);

                        lua_pushnumber(luaVM, fxFlangerParams.fWetDryMix);
                        lua_setfield(luaVM, -2, "wetDryMix");

                        lua_pushnumber(luaVM, fxFlangerParams.fDepth);
                        lua_setfield(luaVM, -2, "depth");

                        lua_pushnumber(luaVM, fxFlangerParams.fFeedback);
                        lua_setfield(luaVM, -2, "feedback");

                        lua_pushnumber(luaVM, fxFlangerParams.fFrequency);
                        lua_setfield(luaVM, -2, "frequency");

                        lua_pushnumber(luaVM, fxFlangerParams.lWaveform);
                        lua_setfield(luaVM, -2, "waveform");

                        lua_pushnumber(luaVM, fxFlangerParams.fDelay);
                        lua_setfield(luaVM, -2, "delay");

                        lua_pushnumber(luaVM, fxFlangerParams.lPhase);
                        lua_setfield(luaVM, -2, "phase");
                        return 1;
                    }
                    break;
                }
                case BASS_FX_DX8_GARGLE:
                {
                    BASS_DX8_GARGLE fxGargleParams;
                    if (CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &fxGargleParams))
                    {
                        lua_newtable(luaVM);

                        lua_pushnumber(luaVM, fxGargleParams.dwRateHz);
                        lua_setfield(luaVM, -2, "rateHz");

                        lua_pushnumber(luaVM, fxGargleParams.dwWaveShape);
                        lua_setfield(luaVM, -2, "waveShape");
                        return 1;
                    }
                    break;
                }
                case BASS_FX_DX8_I3DL2REVERB:
                {
                    BASS_DX8_I3DL2REVERB fxI3DL2ReverbParams;
                    if (CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &fxI3DL2ReverbParams))
                    {
                        lua_newtable(luaVM);

                        lua_pushnumber(luaVM, fxI3DL2ReverbParams.lRoom);
                        lua_setfield(luaVM, -2, "room");

                        lua_pushnumber(luaVM, fxI3DL2ReverbParams.lRoomHF);
                        lua_setfield(luaVM, -2, "roomHF");

                        lua_pushnumber(luaVM, fxI3DL2ReverbParams.flRoomRolloffFactor);
                        lua_setfield(luaVM, -2, "roomRolloffFactor");

                        lua_pushnumber(luaVM, fxI3DL2ReverbParams.flDecayTime);
                        lua_setfield(luaVM, -2, "decayTime");

                        lua_pushnumber(luaVM, fxI3DL2ReverbParams.flDecayHFRatio);
                        lua_setfield(luaVM, -2, "decayHFRatio");

                        lua_pushnumber(luaVM, fxI3DL2ReverbParams.lReflections);
                        lua_setfield(luaVM, -2, "reflections");

                        lua_pushnumber(luaVM, fxI3DL2ReverbParams.flReflectionsDelay);
                        lua_setfield(luaVM, -2, "reflectionsDelay");

                        lua_pushnumber(luaVM, fxI3DL2ReverbParams.lReverb);
                        lua_setfield(luaVM, -2, "reverb");

                        lua_pushnumber(luaVM, fxI3DL2ReverbParams.flReverbDelay);
                        lua_setfield(luaVM, -2, "reverbDelay");

                        lua_pushnumber(luaVM, fxI3DL2ReverbParams.flDiffusion);
                        lua_setfield(luaVM, -2, "diffusion");

                        lua_pushnumber(luaVM, fxI3DL2ReverbParams.flDensity);
                        lua_setfield(luaVM, -2, "density");

                        lua_pushnumber(luaVM, fxI3DL2ReverbParams.flHFReference);
                        lua_setfield(luaVM, -2, "HFReferenct");
                        return 1;
                    }
                    break;
                }
                case BASS_FX_DX8_PARAMEQ:
                {
                    BASS_DX8_PARAMEQ fxParameqParams;
                    if (CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &fxParameqParams))
                    {
                        lua_newtable(luaVM);

                        lua_pushnumber(luaVM, fxParameqParams.fCenter);
                        lua_setfield(luaVM, -2, "center");

                        lua_pushnumber(luaVM, fxParameqParams.fBandwidth);
                        lua_setfield(luaVM, -2, "bandwidth");

                        lua_pushnumber(luaVM, fxParameqParams.fGain);
                        lua_setfield(luaVM, -2, "gain");
                        return 1;
                    }
                    break;
                }
                case BASS_FX_DX8_REVERB:
                {
                    BASS_DX8_REVERB fxReverbParams;
                    if (CStaticFunctionDefinitions::GetSoundEffectParameters(*pSound, strEffectName, &fxReverbParams))
                    {
                        lua_newtable(luaVM);

                        lua_pushnumber(luaVM, fxReverbParams.fInGain);
                        lua_setfield(luaVM, -2, "inGain");

                        lua_pushnumber(luaVM, fxReverbParams.fReverbMix);
                        lua_setfield(luaVM, -2, "reverbMix");

                        lua_pushnumber(luaVM, fxReverbParams.fReverbTime);
                        lua_setfield(luaVM, -2, "reverbTime");

                        lua_pushnumber(luaVM, fxReverbParams.fHighFreqRTRatio);
                        lua_setfield(luaVM, -2, "highFreqRTRatio");
                        return 1;
                    }
                    break;
                }
            }
        }
        else if (pPlayer)
        {
            // TODO: Make it work with player voice
        }
    }
    else
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
