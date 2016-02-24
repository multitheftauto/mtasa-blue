/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaWorldDefs.h
*  PURPOSE:     Lua game world function definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               lil_Toady <>
*               Kevin Whiteside <>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaWorldDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );

    // Get
    LUA_DECLARE ( getTime );
    LUA_DECLARE ( getWeather );
    LUA_DECLARE ( getZoneName );
    LUA_DECLARE ( getGravity );
    LUA_DECLARE ( getGameSpeed );
    LUA_DECLARE ( getWaveHeight );
    LUA_DECLARE ( getFPSLimit );
    LUA_DECLARE ( getMinuteDuration );
    LUA_DECLARE ( isGarageOpen );
    LUA_DECLARE ( isGlitchEnabled );
    LUA_DECLARE ( setJetpackWeaponEnabled );
    LUA_DECLARE ( getJetpackWeaponEnabled );
    LUA_DECLARE ( getCloudsEnabled );
    LUA_DECLARE ( getTrafficLightState );
    LUA_DECLARE ( areTrafficLightsLocked );
    LUA_DECLARE ( getJetpackMaxHeight );
    LUA_DECLARE ( getHeatHaze );
    LUA_DECLARE ( getSkyGradient );
    LUA_DECLARE ( getInteriorSoundsEnabled );
    LUA_DECLARE ( getRainLevel );
    LUA_DECLARE ( getSunSize );
    LUA_DECLARE ( getSunColor );
    LUA_DECLARE ( getWindVelocity );
    LUA_DECLARE ( getFarClipDistance );
    LUA_DECLARE ( getFogDistance );
    LUA_DECLARE ( getAircraftMaxHeight );
    LUA_DECLARE ( getAircraftMaxVelocity );
    LUA_DECLARE ( setOcclusionsEnabled );
    LUA_DECLARE ( getOcclusionsEnabled );
    LUA_DECLARE ( getMoonSize );

    // Set
    LUA_DECLARE ( setTime );
    LUA_DECLARE ( setWeather );
    LUA_DECLARE ( setWeatherBlended );
    LUA_DECLARE ( setGravity );
    LUA_DECLARE ( setGameSpeed );
    LUA_DECLARE ( setWaveHeight );
    LUA_DECLARE ( setSkyGradient );
    LUA_DECLARE ( setHeatHaze );
    LUA_DECLARE ( setFPSLimit );
    LUA_DECLARE ( setMinuteDuration );
    LUA_DECLARE ( setGarageOpen );
    LUA_DECLARE ( setGlitchEnabled );
    LUA_DECLARE ( setCloudsEnabled );
    LUA_DECLARE ( setTrafficLightState );
    LUA_DECLARE ( setTrafficLightsLocked );
    LUA_DECLARE ( setJetpackMaxHeight );
    LUA_DECLARE ( setInteriorSoundsEnabled );
    LUA_DECLARE ( setRainLevel );
    LUA_DECLARE ( setSunSize );
    LUA_DECLARE ( setSunColor );
    LUA_DECLARE ( setWindVelocity );
    LUA_DECLARE ( setFarClipDistance );
    LUA_DECLARE ( setFogDistance );
    LUA_DECLARE ( setAircraftMaxHeight );
    LUA_DECLARE ( setAircraftMaxVelocity );
    LUA_DECLARE ( setPedTargetingMarkerEnabled );
    LUA_DECLARE ( isPedTargetingMarkerEnabled );
    LUA_DECLARE ( setMoonSize );

    // Reset
    LUA_DECLARE ( resetSkyGradient );
    LUA_DECLARE ( resetHeatHaze );
    LUA_DECLARE ( resetRainLevel );
    LUA_DECLARE ( resetSunSize );
    LUA_DECLARE ( resetSunColor );
    LUA_DECLARE ( resetWindVelocity );
    LUA_DECLARE ( resetFarClipDistance );
    LUA_DECLARE ( resetFogDistance );
    LUA_DECLARE ( RemoveWorldModel );
    LUA_DECLARE ( RestoreWorldModel );
    LUA_DECLARE ( RestoreAllWorldModels );
    LUA_DECLARE ( resetMoonSize );
};