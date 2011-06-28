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

#ifndef __CLUAWORLDDEFS_H
#define __CLUAWORLDDEFS_H

#include "CLuaDefs.h"

class CLuaWorldDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );

    // Get
    static int      getTime                             ( lua_State* luaVM );
    static int      getWeather                          ( lua_State* luaVM );
    static int      getZoneName                         ( lua_State* luaVM );
    static int      getGravity                          ( lua_State* luaVM );
    static int      getGameSpeed                        ( lua_State* luaVM );
    static int      getWaveHeight                       ( lua_State* luaVM );
    static int      getFPSLimit                         ( lua_State* luaVM );
    static int      getMinuteDuration                   ( lua_State* luaVM );
    static int      isGarageOpen                        ( lua_State* luaVM );
    static int      isGlitchEnabled                     ( lua_State* luaVM );
    static int      getCloudsEnabled                    ( lua_State* luaVM );
    static int      getTrafficLightState                ( lua_State* luaVM );
    static int      areTrafficLightsLocked              ( lua_State* luaVM );
    static int      getJetpackMaxHeight                 ( lua_State* luaVM );
    static int      getHeatHaze                         ( lua_State* luaVM );
    static int      getSkyGradient                      ( lua_State* luaVM );
    static int      getInteriorSoundsEnabled            ( lua_State* luaVM );
    static int      getRainLevel                        ( lua_State* luaVM );
    static int      getSunSize                          ( lua_State* luaVM );
    static int      getSunColor                         ( lua_State* luaVM );
    static int      getWindVelocity                     ( lua_State* luaVM );
    static int      getFarClipDistance                  ( lua_State* luaVM );
    static int      getFogDistance                      ( lua_State* luaVM );

    // Set
    static int      setTime                             ( lua_State* luaVM );
    static int      setWeather                          ( lua_State* luaVM );
    static int      setWeatherBlended                   ( lua_State* luaVM );
    static int      setGravity                          ( lua_State* luaVM );
    static int      setGameSpeed                        ( lua_State* luaVM );
    static int      setWaveHeight                       ( lua_State* luaVM );
    static int      setSkyGradient                      ( lua_State* luaVM );
    static int      setHeatHaze                         ( lua_State* luaVM );
    static int      setFPSLimit                         ( lua_State* luaVM );
    static int      setMinuteDuration                   ( lua_State* luaVM );
    static int      setGarageOpen                       ( lua_State* luaVM );
    static int      setGlitchEnabled                    ( lua_State* luaVM );
    static int      setCloudsEnabled                    ( lua_State* luaVM );
    static int      setTrafficLightState                ( lua_State* luaVM );
    static int      setTrafficLightsLocked              ( lua_State* luaVM );
    static int      setJetpackMaxHeight                 ( lua_State* luaVM );
    static int      setInteriorSoundsEnabled            ( lua_State* luaVM );
    static int      setRainLevel                        ( lua_State* luaVM );
    static int      setSunSize                          ( lua_State* luaVM );
    static int      setSunColor                         ( lua_State* luaVM );
    static int      setWindVelocity                     ( lua_State* luaVM );
    static int      setFarClipDistance                  ( lua_State* luaVM );
    static int      setFogDistance                      ( lua_State* luaVM );

    // Reset
    static int      resetSkyGradient                    ( lua_State* luaVM );
    static int      resetHeatHaze                       ( lua_State* luaVM );
    static int      resetRainLevel                      ( lua_State* luaVM );
    static int      resetSunSize                        ( lua_State* luaVM );
    static int      resetSunColor                       ( lua_State* luaVM );
    static int      resetWindVelocity                   ( lua_State* luaVM );
    static int      resetFarClipDistance                ( lua_State* luaVM );
    static int      resetFogDistance                    ( lua_State* luaVM );
};

#endif
