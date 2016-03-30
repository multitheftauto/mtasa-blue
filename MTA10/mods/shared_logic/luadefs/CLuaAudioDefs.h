/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaAudioDefs.h
*  PURPOSE:     Lua audio definitions class header
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaAudioDefs : public CLuaDefs
{
public:
    static void LoadFunctions ( void );
    static void AddClass ( lua_State* luaVM );

    // Audio funcs
    LUA_DECLARE ( PlaySoundFrontEnd );
    LUA_DECLARE ( SetAmbientSoundEnabled );
    LUA_DECLARE ( IsAmbientSoundEnabled );
    LUA_DECLARE ( ResetAmbientSounds );
    LUA_DECLARE ( SetWorldSoundEnabled );
    LUA_DECLARE ( IsWorldSoundEnabled );
    LUA_DECLARE ( ResetWorldSounds );
    LUA_DECLARE ( PlaySFX );
    LUA_DECLARE ( PlaySFX3D );
    LUA_DECLARE ( GetSFXStatus );

    // Sound effects and synth functions
    LUA_DECLARE ( PlaySound );
    LUA_DECLARE ( PlaySound3D );
    LUA_DECLARE ( StopSound );
    LUA_DECLARE ( SetSoundPosition );
    LUA_DECLARE ( GetSoundPosition );
    LUA_DECLARE ( GetSoundLength );
    LUA_DECLARE ( SetSoundPaused );
    LUA_DECLARE ( IsSoundPaused );
    LUA_DECLARE ( SetSoundVolume );
    LUA_DECLARE ( GetSoundVolume );
    LUA_DECLARE ( SetSoundSpeed );
    LUA_DECLARE ( GetSoundSpeed );
    LUA_DECLARE ( SetSoundProperties );
    LUA_DECLARE ( GetSoundProperties );
    LUA_DECLARE ( GetSoundFFTData );
    LUA_DECLARE ( GetSoundWaveData );
    LUA_DECLARE ( SetSoundPanEnabled );
    LUA_DECLARE ( IsSoundPanEnabled );
    LUA_DECLARE ( GetSoundLevelData );
    LUA_DECLARE ( GetSoundBPM );
    LUA_DECLARE ( SetSoundMinDistance );
    LUA_DECLARE ( GetSoundMinDistance );
    LUA_DECLARE ( SetSoundMaxDistance );
    LUA_DECLARE ( GetSoundMaxDistance );
    LUA_DECLARE ( GetSoundMetaTags );
    LUA_DECLARE ( SetSoundEffectEnabled );
    LUA_DECLARE ( GetSoundEffects );
    LUA_DECLARE ( SetSoundPan );
    LUA_DECLARE ( GetSoundPan );

    // Radio functions
    LUA_DECLARE ( SetRadioChannel );
    LUA_DECLARE ( GetRadioChannel );
    LUA_DECLARE ( GetRadioChannelName );
};