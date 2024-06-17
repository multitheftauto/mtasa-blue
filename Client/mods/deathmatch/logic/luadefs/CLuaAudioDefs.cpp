/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaAudioDefs.cpp
 *  PURPOSE:     Lua audio definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>
#include "CBassAudio.h"

void CLuaAudioDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[] {// Audio funcs
        {"playSoundFrontEnd", ArgumentParser<PlaySoundFrontEnd>},
        {"setAmbientSoundEnabled", ArgumentParser<SetAmbientSoundEnabled>},
        {"isAmbientSoundEnabled", ArgumentParser<IsAmbientSoundEnabled>},
        {"resetAmbientSounds", ArgumentParser<ResetAmbientSounds>},
        {"setWorldSoundEnabled", ArgumentParser<SetWorldSoundEnabled>},
        {"isWorldSoundEnabled", ArgumentParser<IsWorldSoundEnabled>},
        {"resetWorldSounds", ArgumentParser<ResetWorldSounds>},
        {"playSFX", ArgumentParser<PlaySFX>},
        {"playSFX3D", ArgumentParser<PlaySFX3D>},
        {"getSFXStatus", ArgumentParser<GetSFXStatus>},

        // Sound effects and synth funcs
        {"playSound", ArgumentParser<PlaySound>},
        {"playSound3D", ArgumentParser<PlaySound3D>},
        {"stopSound", ArgumentParser<StopSound>},
        {"setSoundPosition", ArgumentParser<SetSoundPosition>},
        {"getSoundPosition", ArgumentParser<GetSoundPosition>},
        {"getSoundLength", ArgumentParser<GetSoundLength>},
        {"getSoundBufferLength", ArgumentParser<GetSoundBufferLength>},
        {"setSoundLooped", ArgumentParser<SetSoundLooped>},
        {"isSoundLooped", ArgumentParser<IsSoundLooped>},
        {"setSoundPaused", ArgumentParser<SetSoundPaused>},
        {"isSoundPaused", ArgumentParser<IsSoundPaused>},
        {"setSoundVolume", ArgumentParser<SetSoundVolume>},
        {"getSoundVolume", ArgumentParser<GetSoundVolume>},
        {"setSoundSpeed", ArgumentParser<SetSoundSpeed>},
        {"getSoundSpeed", ArgumentParser<GetSoundSpeed>},
        {"setSoundProperties", ArgumentParser<SetSoundProperties>},
        {"getSoundProperties", ArgumentParser<GetSoundProperties>},
        {"getSoundFFTData", ArgumentParser<GetSoundFFTData>},
        {"getSoundWaveData", ArgumentParser<GetSoundWaveData>},
        {"getSoundLevelData", ArgumentParser<GetSoundLevelData>},
        {"setSoundPanningEnabled", ArgumentParser<SetSoundPanningEnabled>},
        {"isSoundPanningEnabled", ArgumentParser<IsSoundPanEnabled>},
        {"getSoundBPM", ArgumentParser<GetSoundBPM>},
        {"setSoundMinDistance", ArgumentParser<SetSoundMinDistance>},
        {"getSoundMinDistance", ArgumentParser<GetSoundMinDistance>},
        {"setSoundMaxDistance", ArgumentParser<SetSoundMaxDistance>},
        {"getSoundMaxDistance", ArgumentParser<GetSoundMaxDistance>},
        {"getSoundMetaTags", ArgumentParser<GetSoundMetaTags>},
        {"setSoundEffectEnabled", ArgumentParser<SetSoundEffectEnabled>},
        {"getSoundEffects", ArgumentParser<GetSoundEffects>},
        {"setSoundEffectParameter", ArgumentParser<SetSoundEffectParameter>},
        {"getSoundEffectParameters", ArgumentParser<GetSoundEffectParameters>},
        {"setSoundPan", ArgumentParser<SetSoundPan>},
        {"getSoundPan", ArgumentParser<GetSoundPan>},

        // Radio funcs
        {"setRadioChannel", ArgumentParser<SetRadioChannel>},
        {"getRadioChannel", ArgumentParser<GetRadioChannel>},
        {"getRadioChannelName", ArgumentParser<GetRadioChannelName>},

        // Dev funcs
        {"showSound", ArgumentParser<ShowSound>},
        {"isShowSoundEnabled", ArgumentParser<IsShowSoundEnabled>}
    };

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
    lua_classvariable(luaVM, "length", nullptr, "getSoundLength");
    lua_classvariable(luaVM, "bufferLength", nullptr, "getSoundBufferLength");

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

bool CLuaAudioDefs::PlaySoundFrontEnd(std::uint8_t sound)
{
    if(sound > 101)
        throw std::invalid_argument("Invalid sound ID specified. Valid sound IDs are 0 - 101.");
    
    return CStaticFunctionDefinitions::PlaySoundFrontEnd(sound);
}

bool CLuaAudioDefs::SetAmbientSoundEnabled(eAmbientSoundType type, bool enabled) noexcept
{
    return CStaticFunctionDefinitions::SetAmbientSoundEnabled(type, enabled);
}

bool CLuaAudioDefs::IsAmbientSoundEnabled(eAmbientSoundType type) noexcept
{
    return CStaticFunctionDefinitions::IsAmbientSoundEnabled(type);
}

bool CLuaAudioDefs::ResetAmbientSounds() noexcept
{
    return CStaticFunctionDefinitions::ResetAmbientSounds();
}

bool CLuaAudioDefs::SetWorldSoundEnabled(
    int group,
    std::optional<int> index,
    bool enable,
    std::optional<bool> immediate
) noexcept
{
    return CStaticFunctionDefinitions::SetWorldSoundEnabled(
        group, index.value_or(-1), enable, immediate.value_or(false)
    );
}

bool CLuaAudioDefs::IsWorldSoundEnabled(int group, std::optional<int> index) noexcept
{
    // bool isWorldSoundEnabled ( int group, [ int index ] )
    return CStaticFunctionDefinitions::IsWorldSoundEnabled(group, index.value_or(-1));
}

bool CLuaAudioDefs::ResetWorldSounds() noexcept
{
    return CStaticFunctionDefinitions::ResetWorldSounds();
}

CClientSound* CLuaAudioDefs::PlaySFX (
    lua_State* luaVM,
    eAudioLookupIndex container,
    int bank,
    int audio,
    std::optional<bool> loop
) {
    // sound playSFX ( string audioContainer, int bankIndex, int audioIndex [, loop = false ] )    
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        throw std::runtime_error("Cannot detect virtual machine!");
        
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        throw std::runtime_error("Cannot get resource!");
    
    
    return CStaticFunctionDefinitions::PlaySFX(pResource, container, bank, audio,
        loop.value_or(false)
    );
}

CClientSound* CLuaAudioDefs::PlaySFX3D (
    lua_State* luaVM,
    eAudioLookupIndex container,
    int bank,
    int audio,
    float posX,
    float posY,
    float posZ,
    std::optional<bool> loop
) {
    // sound playSFX3D ( string audioContainer, int bankIndex, int audioIndex, float posX, float posY, float posZ [, loop = false ] )
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        throw std::runtime_error("Cannot detect virtual machine!");
        
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        throw std::runtime_error("Cannot get resource!");

    return CStaticFunctionDefinitions::PlaySFX3D(pResource, container, bank, audio,
        CVector(posX, posY, posZ), loop.value_or(false)
    );
}

bool CLuaAudioDefs::GetSFXStatus(eAudioLookupIndex container) noexcept
{
    //  bool getSFXStatus ( string audioContainer )
    return CStaticFunctionDefinitions::GetSFXStatus(container);
}

CClientSound* CLuaAudioDefs::PlaySound(
    lua_State* luaVM,
    SString path,
    std::optional<bool> loop,
    std::optional<bool> throttle
) {
    // element playSound ( string soundPath, [ bool looped = false, bool throttled = true ] )
    
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        throw std::runtime_error("Cannot detect virtual machine!");

    CResource* pResource = luaMain->GetResource();
    if (!pResource)
        throw std::runtime_error("Cannot get resource!");

    SString strFilename;
    bool bIsURL = false;
    bool bIsRawData = false;

    if (CResourceManager::ParseResourcePathInput(path, pResource, &strFilename))
        path = strFilename;
    else
    {
        if ((!stricmp(path.Left(4), "http") || !stricmp(path.Left(3), "ftp")) &&
            (path.length() <= 2048 || path.find('\n') == SString::npos))
            bIsURL = true;
        else
            bIsRawData = true;
    }

    // ParseResourcePathInput changes pResource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3"
    // ) Fixes #6507 - Caz
    // TODO: Fix ParseResourcePathInput
    if (!pResource)
        throw std::runtime_error("Cannot get resource!");
        
    CClientSound* pSound = CStaticFunctionDefinitions::PlaySound(pResource, path,
        bIsURL, bIsRawData, loop.value_or(false), throttle.value_or(true));

    if (!pSound)
        return nullptr;
        
    CLuaArguments Arguments;
    // Reason
    Arguments.PushString("play");
    // call onClientSoundStarted
    pSound->CallEvent("onClientSoundStarted", Arguments, false);

    return pSound;
}

CClientSound* CLuaAudioDefs::PlaySound3D (
    lua_State* luaVM,
    SString path,
    float x,
    float y,
    float z,
    std::optional<bool> loop,
    std::optional<bool> throttle
) {
    // element playSound3D ( string soundPath, float x, float y, float z, [ bool looped = false, bool throttled = true ] )
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        throw std::runtime_error("Cannot detect virtual machine!");

    CResource* pResource = luaMain->GetResource();
    if (!pResource)
        throw std::runtime_error("Cannot get resource!");

    SString strFilename;
    bool bIsURL = false;
    bool bIsRawData = false;

    if (CResourceManager::ParseResourcePathInput(path, pResource, &strFilename))
        path = strFilename;
    else
    {
        if ((!stricmp(path.Left(4), "http") || !stricmp(path.Left(3), "ftp")) &&
            (path.length() <= 2048 || path.find('\n') == SString::npos))
            bIsURL = true;
        else
            bIsRawData = true;
    }

    // ParseResourcePathInput changes pResource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3"
    // ) Fixes #6507 - Caz
    // TODO: Fix ParseResourcePathInput
    if (!pResource)
        throw std::runtime_error("Cannot get resource!");
        
    CClientSound* pSound = CStaticFunctionDefinitions::PlaySound3D(pResource, path,
        bIsURL, bIsRawData, CVector(x, y, z), loop.value_or(false), throttle.value_or(true));

    if (!pSound)
        return nullptr;
        
    CLuaArguments Arguments;
    // Reason
    Arguments.PushString("play");
    // call onClientSoundStarted
    pSound->CallEvent("onClientSoundStarted", Arguments, false);

    return pSound;
}

bool CLuaAudioDefs::StopSound(CClientSound* sound) noexcept
{
    // bool stopSound ( element theSound )
    return CStaticFunctionDefinitions::StopSound(*sound);
}

bool CLuaAudioDefs::SetSoundPosition(
    std::variant<CClientSound*, CClientPlayer*> element,
    double pos
) noexcept {
    // bool setSoundPosition ( element theSound, float pos )
    return CStaticFunctionDefinitions::SetSoundPosition(element, pos);
}

double CLuaAudioDefs::GetSoundPosition(std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    // float getSoundPosition ( element theSound )
    return CStaticFunctionDefinitions::GetSoundPosition(element);
}

double CLuaAudioDefs::GetSoundLength(std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    // float getSoundLength ( element theSound )
    return CStaticFunctionDefinitions::GetSoundLength(element);
}

double CLuaAudioDefs::GetSoundBufferLength(CClientSound* sound) noexcept
{
    return CStaticFunctionDefinitions::GetSoundBufferLength(*sound).value_or(0.0);
}

bool CLuaAudioDefs::SetSoundPaused(std::variant<CClientSound*, CClientPlayer*> element, bool paused) noexcept
{
    // bool setSoundPaused ( element theSound, bool paused )
    return CStaticFunctionDefinitions::SetSoundPaused(element, paused);
}

bool CLuaAudioDefs::IsSoundPaused(std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    return CStaticFunctionDefinitions::IsSoundPaused(element);
}

bool CLuaAudioDefs::SetSoundVolume(
    std::variant<CClientSound*, CClientPlayer*> element,
    float volume
) noexcept {
    // bool setSoundVolume ( element theSound/thePlayer, float volume )
    return CStaticFunctionDefinitions::SetSoundVolume(element, volume);
}

float CLuaAudioDefs::GetSoundVolume(std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    return CStaticFunctionDefinitions::GetSoundVolume(element);
}

bool CLuaAudioDefs::SetSoundSpeed(
    std::variant<CClientSound*, CClientPlayer*> element,
    float speed
) noexcept {
    // bool setSoundSpeed ( element theSound, float speed )
    return CStaticFunctionDefinitions::SetSoundSpeed(element, speed);
}

float CLuaAudioDefs::GetSoundSpeed(std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    return CStaticFunctionDefinitions::GetSoundSpeed(element);
}

bool CLuaAudioDefs::SetSoundProperties(
    CClientSound* sound,
    float sampleRate,
    float tempo,
    float pitch,
    std::optional<bool> reverse
) noexcept {
    // bool setSoundProperties(element sound, float fSampleRate, float fTempo, float fPitch [, bool bReverse = false ] )
    return CStaticFunctionDefinitions::SetSoundProperties(*sound, sampleRate,
        tempo, pitch, reverse.value_or(false)
    );
}

std::variant<bool, CLuaMultiReturn<float, float, float, bool>> CLuaAudioDefs::GetSoundProperties(
    CClientSound* sound
) noexcept {
    // float, float, float, bool getSoundProperties( element sound )
    float sampleRate;
    float tempo;
    float pitch;
    bool reversed;

    if (!CStaticFunctionDefinitions::GetSoundProperties(*sound, sampleRate, tempo, pitch, reversed))
        return false;
    
    return CLuaMultiReturn<float, float, float, bool>(sampleRate, tempo, pitch, reversed);
}

std::variant<bool, std::vector<float>> CLuaAudioDefs::GetSoundFFTData(
    std::variant<CClientSound*, CClientPlayer*> element,
    int samples,
    std::optional<int> bands
) noexcept {
    // table getSoundFFTData ( element sound, int samples [, int bands = 0 ] )
    int    iBands = bands.value_or(0);
    float* pData = CStaticFunctionDefinitions::GetSoundFFTData(element, samples, iBands);
    if (!pData)
        return false;
    
    std::vector<float> arr;

    for (auto i = 0; i <= (iBands ? iBands - 1 : samples / 2); i++)
    {
        arr.push_back(pData[i]);
    }

    delete[] pData;
    return arr;
}

std::variant<bool, std::vector<float>> CLuaAudioDefs::GetSoundWaveData(
    std::variant<CClientSound*, CClientPlayer*> element,
    int samples
) noexcept {
    // table getSoundWaveData ( element sound, int samples )
    float* pData = CStaticFunctionDefinitions::GetSoundWaveData(element, samples);
    if (!pData)
        return false;
    
    std::vector<float> arr;
    
    for (auto i = 0; i < samples; i++)
    {
        arr.push_back(pData[i]);
    }
    
    delete[] pData;
    return 1;
}

bool CLuaAudioDefs::SetSoundPanningEnabled(CClientSound* sound, bool enable) noexcept
{
    // bool setSoundPanningEnabled ( element sound, bool enable )
    return CStaticFunctionDefinitions::SetSoundPanEnabled(*sound, enable);
}

int CLuaAudioDefs::IsSoundPanEnabled(lua_State* luaVM)
{
    CClientSound*    pSound = nullptr;
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

std::variant<bool, CLuaMultiReturn<std::uint32_t, std::uint32_t>> CLuaAudioDefs::GetSoundLevelData(
    std::variant<CClientSound*, CClientPlayer*> element
) noexcept {
    // int, int getSoundLevelData ( element theSound )
    DWORD left;
    DWORD right;
    if (!CStaticFunctionDefinitions::GetSoundLevelData(element, left, right))
        return false;

    return CLuaMultiReturn<std::uint32_t, std::uint32_t>(left, right);
}

int CLuaAudioDefs::GetSoundBPM(lua_State* luaVM)
{
    CClientSound*    pSound = nullptr;
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

int CLuaAudioDefs::SetSoundMinDistance(lua_State* luaVM)
{
    CClientSound*    pSound = nullptr;
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
    CClientSound*    pSound = nullptr;
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
    CClientSound*    pSound = nullptr;
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
    CClientSound*    pSound = nullptr;
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
    CClientSound*    pSound = nullptr;
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
    CClientPlayer*   pPlayer = nullptr;
    CClientSound*    pSound = nullptr;
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
    CClientPlayer*   pPlayer = nullptr;
    CClientSound*    pSound = nullptr;
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

bool CLuaAudioDefs::SetSoundEffectParameter(CClientSound* sound, eSoundEffectType effectType, const char* effectParam)
{
    if (!pSound || !pSound->IsFxEffectEnabled(eEffectType))
        throw std::invalid_argument("Effect's parameters can't be set unless it's enabled");

    using namespace eSoundEffectParams;
    switch (eEffectType)
    {
        case BASS_FX_DX8_CHORUS:
        {
            BASS_DX8_CHORUS params;
            pSound->GetFxEffectParameters(eEffectType, &params);

            Chorus eEffectParameter;            
            StringToEnum(effectParam, eEffectParameter);
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

            return SetParamWithErrorLog(eEffectParameter, params);
        }
    };
}

int CLuaAudioDefs::SetSoundEffectParameter(CClientSound* sound, eSoundEffectType effectType)
{
    // bool setSoundEffectParameter ( sound sound, string effectName, string effectParameter, var effectParameterValue  )
    CClientSound*    pSound = nullptr;
    eSoundEffectType eEffectType;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pSound);
    argStream.ReadEnumString(eEffectType);

    if (argStream.HasErrors())
    {
        return luaL_error(luaVM, argStream.GetFullErrorMessage());
    }

    if (!pSound->IsFxEffectEnabled(eEffectType))
    {
        return luaL_error(luaVM, "Effect's parameters can't be set unless it's enabled");
    }

    // Call `SetFxEffectParameters` and log errors if any
    const auto SetParamWithErrorLog = [&](auto effectParam, auto& params) {
        // Try setting parameter
        if (pSound->SetFxEffectParameters(eEffectType, &params))
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

    using namespace eSoundEffectParams;
    switch (eEffectType)
    {
        case BASS_FX_DX8_CHORUS:
        {
            BASS_DX8_CHORUS params;
            pSound->GetFxEffectParameters(eEffectType, &params);

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

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_COMPRESSOR:
        {
            BASS_DX8_COMPRESSOR params;
            pSound->GetFxEffectParameters(eEffectType, &params);

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

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_DISTORTION:
        {
            BASS_DX8_DISTORTION params;
            pSound->GetFxEffectParameters(eEffectType, &params);

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

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_ECHO:
        {
            BASS_DX8_ECHO params;
            pSound->GetFxEffectParameters(eEffectType, &params);

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

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_FLANGER:
        {
            BASS_DX8_FLANGER params;
            pSound->GetFxEffectParameters(eEffectType, &params);

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

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_GARGLE:
        {
            BASS_DX8_GARGLE params;
            pSound->GetFxEffectParameters(eEffectType, &params);

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

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_I3DL2REVERB:
        {
            BASS_DX8_I3DL2REVERB params;
            pSound->GetFxEffectParameters(eEffectType, &params);

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

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_PARAMEQ:
        {
            BASS_DX8_PARAMEQ params;
            pSound->GetFxEffectParameters(eEffectType, &params);

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

            return SetParamWithErrorLog(eEffectParameter, params);
        }
        case BASS_FX_DX8_REVERB:
        {
            BASS_DX8_REVERB params;
            pSound->GetFxEffectParameters(eEffectType, &params);

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

            return SetParamWithErrorLog(eEffectParameter, params);
        }
    }

    // Only ever reaches this point if there's an error
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

std::variant<bool, std::unordered_map<const char*, float>> CLuaAudioDefs::GetSoundEffectParameters(
    CClientSound* sound,
    eSoundEffectType effectType
) noexcept {
    // table getSoundEffectParameters ( sound sound, string effectName )
    if (!sound || !sound->IsFxEffectEnabled(effectType))
        return false;

    std::unordered_map<const char*, float> arr;

    using namespace eSoundEffectParams;

    switch (effectType)
    {
        case BASS_FX_DX8_CHORUS:
        {
            BASS_DX8_CHORUS fxChorusParams;
            if (!sound->GetFxEffectParameters(effectType, &fxChorusParams))
                break;

            arr.insert({EnumToString(Chorus::WET_DRY_MIX), fxChorusParams.fWetDryMix});
            arr.insert({EnumToString(Chorus::DEPTH), fxChorusParams.fDepth});
            arr.insert({EnumToString(Chorus::FEEDBACK), fxChorusParams.fFeedback});
            arr.insert({EnumToString(Chorus::FREQUENCY), fxChorusParams.fFrequency});
            arr.insert({EnumToString(Chorus::WAVEFORM), fxChorusParams.lWaveform});
            arr.insert({EnumToString(Chorus::DELAY), fxChorusParams.fDelay});
            arr.insert({EnumToString(Chorus::PHASE), fxChorusParams.lPhase});
            break;
        }
        case BASS_FX_DX8_COMPRESSOR:
        {
            BASS_DX8_COMPRESSOR fxCompressorParams;
            if (!sound->GetFxEffectParameters(effectType, &fxCompressorParams))
                break;

            arr.insert({EnumToString(Compressor::GAIN), fxCompressorParams.fGain});
            arr.insert({EnumToString(Compressor::ATTACK), fxCompressorParams.fAttack});
            arr.insert({EnumToString(Compressor::RELEASE), fxCompressorParams.fRelease});
            arr.insert({EnumToString(Compressor::THRESHOLD), fxCompressorParams.fThreshold});
            arr.insert({EnumToString(Compressor::RATIO), fxCompressorParams.fRatio});
            arr.insert({EnumToString(Compressor::PREDELAY), fxCompressorParams.fPredelay});
            break;
        }
        case BASS_FX_DX8_DISTORTION:
        {
            BASS_DX8_DISTORTION fxDistortionParams;
            if (!sound->GetFxEffectParameters(effectType, &fxDistortionParams))
                break;

            arr.insert({EnumToString(Distortion::GAIN), fxDistortionParams.fGain});
            arr.insert({EnumToString(Distortion::EDGE), fxDistortionParams.fEdge});
            arr.insert({EnumToString(Distortion::POST_EQ_CENTER_FREQUENCY), fxDistortionParams.fPostEQCenterFrequency});
            arr.insert({EnumToString(Distortion::POST_EQ_BANDWIDTH), fxDistortionParams.fPostEQBandwidth});
            arr.insert({EnumToString(Distortion::PRE_LOWPASS_CUTOFF), fxDistortionParams.fPreLowpassCutoff});
            break;
        }
        case BASS_FX_DX8_ECHO:
        {
            BASS_DX8_ECHO fxEchoParams;
            if (!sound->GetFxEffectParameters(effectType, &fxEchoParams))
                break;

            arr.insert({EnumToString(Echo::WET_DRY_MIX), fxEchoParams.fWetDryMix});
            arr.insert({EnumToString(Echo::FEEDBACK), fxEchoParams.fFeedback});
            arr.insert({EnumToString(Echo::LEFT_DELAY), fxEchoParams.fLeftDelay});
            arr.insert({EnumToString(Echo::RIGHT_DELAY), fxEchoParams.fRightDelay});
            arr.insert({EnumToString(Echo::PAN_DELAY), fxEchoParams.lPanDelay});
            break;
        }
        case BASS_FX_DX8_FLANGER:
        {
            BASS_DX8_FLANGER fxFlangerParams;
            if (!sound->GetFxEffectParameters(effectType, &fxFlangerParams))
                break;

            arr.insert({EnumToString(Flanger::WET_DRY_MIX), fxFlangerParams.fWetDryMix});
            arr.insert({EnumToString(Flanger::DEPTH), fxFlangerParams.fDepth});
            arr.insert({EnumToString(Flanger::FEEDBACK), fxFlangerParams.fFeedback});
            arr.insert({EnumToString(Flanger::FREQUENCY), fxFlangerParams.fFrequency});
            arr.insert({EnumToString(Flanger::WAVEFORM), fxFlangerParams.lWaveform});
            arr.insert({EnumToString(Flanger::DELAY), fxFlangerParams.fDelay});
            arr.insert({EnumToString(Flanger::PHASE), fxFlangerParams.lPhase});
            break;
        }
        case BASS_FX_DX8_GARGLE:
        {
            BASS_DX8_GARGLE fxGargleParams;
            if (!sound->GetFxEffectParameters(effectType, &fxGargleParams))
                break;

            arr.insert({EnumToString(Gargle::RATE_HZ), fxGargleParams.dwRateHz});
            arr.insert({EnumToString(Gargle::WAVE_SHAPE), fxGargleParams.dwWaveShape});
            break;
        }
        case BASS_FX_DX8_I3DL2REVERB:
        {
            BASS_DX8_I3DL2REVERB fxI3DL2ReverbParams;
            if (!sound->GetFxEffectParameters(effectType, &fxI3DL2ReverbParams))
                break;

            arr.insert({EnumToString(I3DL2Reverb::ROOM), fxI3DL2ReverbParams.lRoom});
            arr.insert({EnumToString(I3DL2Reverb::ROOM_HF), fxI3DL2ReverbParams.lRoomHF});
            arr.insert({EnumToString(I3DL2Reverb::ROOM_ROLLOFF_FACTOR), fxI3DL2ReverbParams.flRoomRolloffFactor});
            arr.insert({EnumToString(I3DL2Reverb::DECAY_TIME), fxI3DL2ReverbParams.flDecayTime});
            arr.insert({EnumToString(I3DL2Reverb::DECAY_HF_RATIO), fxI3DL2ReverbParams.flDecayHFRatio});
            arr.insert({EnumToString(I3DL2Reverb::REFLECTIONS), fxI3DL2ReverbParams.lReflections});
            arr.insert({EnumToString(I3DL2Reverb::REFLECTIONS_DELAY), fxI3DL2ReverbParams.flReflectionsDelay});
            arr.insert({EnumToString(I3DL2Reverb::REVERB), fxI3DL2ReverbParams.lReverb});
            arr.insert({EnumToString(I3DL2Reverb::REVERB_DELAY), fxI3DL2ReverbParams.flReverbDelay});
            arr.insert({EnumToString(I3DL2Reverb::DIFFUSION), fxI3DL2ReverbParams.flDiffusion});
            arr.insert({EnumToString(I3DL2Reverb::DENSITY), fxI3DL2ReverbParams.flDensity});
            arr.insert({EnumToString(I3DL2Reverb::HF_REFERENCE), fxI3DL2ReverbParams.flHFReference});
            break;
        }
        case BASS_FX_DX8_PARAMEQ:
        {
            BASS_DX8_PARAMEQ fxParameqParams;
            if (!sound->GetFxEffectParameters(effectType, &fxParameqParams))
                break;

            arr.insert({EnumToString(ParamEq::CENTER), fxParameqParams.fCenter});
            arr.insert({EnumToString(ParamEq::BANDWIDTH), fxParameqParams.fBandwidth});
            arr.insert({EnumToString(ParamEq::GAIN), fxParameqParams.fGain});
            break;
        }
        case BASS_FX_DX8_REVERB:
        {
            BASS_DX8_REVERB fxReverbParams;
            if (!sound->GetFxEffectParameters(effectType, &fxReverbParams))
                break;

            arr.insert({EnumToString(Reverb::IN_GAIN), fxReverbParams.fInGain});
            arr.insert({EnumToString(Reverb::REVERB_MIX), fxReverbParams.fReverbMix});
            arr.insert({EnumToString(Reverb::REVERB_TIME), fxReverbParams.fReverbTime});
            arr.insert({EnumToString(Reverb::HIGH_FREQ_RT_RATIO), fxReverbParams.fHighFreqRTRatio});
            break;
        }
    }
    return arr;
}

bool CLuaAudioDefs::SetSoundPan(
    std::variant<CClientSound*, CClientPlayer*> element,
    float pan
) noexcept {
    // bool setSoundPan ( element theSound, float pan )
    return CStaticFunctionDefinitions::SetSoundPan(element, pan);
}

float CLuaAudioDefs::GetSoundPan(std::variant<CClientSound*, CClientPlayer*> element) noexcept
{
    // float getSoundPan ( element theSound )
    return CStaticFunctionDefinitions::GetSoundPan(element);
}

bool CLuaAudioDefs::SetSoundLooped(CClientSound* pSound, bool bLoop) noexcept
{
    return pSound->SetLooped(bLoop);
}

bool CLuaAudioDefs::IsSoundLooped(CClientSound* pSound) noexcept
{
    return pSound->IsLooped();
}

bool CLuaAudioDefs::SetRadioChannel(std::uint8_t channel) noexcept
{
    // bool setRadioChannel ( int ID )
    return CStaticFunctionDefinitions::SetRadioChannel(channel);
}

std::uint8_t CLuaAudioDefs::GetRadioChannel() noexcept
{
    return CStaticFunctionDefinitions::GetRadioChannel();
}

std::variant<bool, const char*> CLuaAudioDefs::GetRadioChannelName(int channel) noexcept
{
    static const SFixedArray<const char*, 13> szRadioStations = {{
        "Radio off", "Playback FM", "K-Rose", "K-DST", "Bounce FM", "SF-UR", "Radio Los Santos",
        "Radio X", "CSR 103.9", "K-Jah West", "Master Sounds 98.3", "WCTR", "User Track Player"
    }};

    // TODO: Add .size() method to SFixedArray and call that method here
    if (channel < 0 || channel > NUMELMS(szRadioStations))
        return false;

    return szRadioStations[channel];
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
