/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaWorldDefs.cpp
 *  PURPOSE:     Lua game world function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaWorldDefs.h"
#include "CStaticFunctionDefinitions.h"
#include "CScriptArgReader.h"

void CLuaWorldDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{// Get
                                                                             {"getTime", getTime},
                                                                             {"getWeather", getWeather},
                                                                             {"getZoneName", getZoneName},
                                                                             {"getGravity", getGravity},
                                                                             {"getGameSpeed", getGameSpeed},
                                                                             {"getWaveHeight", getWaveHeight},
                                                                             {"getFPSLimit", getFPSLimit},
                                                                             {"getMinuteDuration", getMinuteDuration},
                                                                             {"getTrafficLightState", getTrafficLightState},
                                                                             {"getJetpackMaxHeight", getJetpackMaxHeight},
                                                                             {"getSkyGradient", getSkyGradient},
                                                                             {"getHeatHaze", getHeatHaze},
                                                                             {"getJetpackWeaponEnabled", getJetpackWeaponEnabled},
                                                                             {"getCloudsEnabled", getCloudsEnabled},
                                                                             {"getInteriorSoundsEnabled", getInteriorSoundsEnabled},
                                                                             {"getRainLevel", getRainLevel},
                                                                             {"getSunSize", getSunSize},
                                                                             {"getSunColor", getSunColor},
                                                                             {"getWindVelocity", getWindVelocity},
                                                                             {"getFarClipDistance", getFarClipDistance},
                                                                             {"getFogDistance", getFogDistance},
                                                                             {"getAircraftMaxHeight", getAircraftMaxHeight},
                                                                             {"getAircraftMaxVelocity", getAircraftMaxVelocity},
                                                                             {"getOcclusionsEnabled", getOcclusionsEnabled},
                                                                             {"getMoonSize", getMoonSize},

                                                                             // Set
                                                                             {"setTime", setTime},
                                                                             {"setWeather", setWeather},
                                                                             {"setWeatherBlended", setWeatherBlended},
                                                                             {"setGravity", setGravity},
                                                                             {"setGameSpeed", setGameSpeed},
                                                                             {"setWaveHeight", setWaveHeight},
                                                                             {"setSkyGradient", setSkyGradient},
                                                                             {"setHeatHaze", setHeatHaze},
                                                                             {"setFPSLimit", setFPSLimit},
                                                                             {"setMinuteDuration", setMinuteDuration},
                                                                             {"setGarageOpen", setGarageOpen},
                                                                             {"setGlitchEnabled", setGlitchEnabled},
                                                                             {"setCloudsEnabled", setCloudsEnabled},
                                                                             {"setTrafficLightState", setTrafficLightState},
                                                                             {"setTrafficLightsLocked", setTrafficLightsLocked},
                                                                             {"setJetpackMaxHeight", setJetpackMaxHeight},
                                                                             {"setInteriorSoundsEnabled", setInteriorSoundsEnabled},
                                                                             {"setRainLevel", setRainLevel},
                                                                             {"setSunSize", setSunSize},
                                                                             {"setSunColor", setSunColor},
                                                                             {"setWindVelocity", setWindVelocity},
                                                                             {"setFarClipDistance", setFarClipDistance},
                                                                             {"setFogDistance", setFogDistance},
                                                                             {"setAircraftMaxHeight", setAircraftMaxHeight},
                                                                             {"setAircraftMaxVelocity", setAircraftMaxVelocity},
                                                                             {"setOcclusionsEnabled", setOcclusionsEnabled},
                                                                             {"setMoonSize", setMoonSize},
                                                                             {"setJetpackWeaponEnabled", setJetpackWeaponEnabled},
                                                                             {"setWorldSpecialPropertyEnabled", ArgumentParserWarn<false, setWorldSpecialPropertyEnabled>},

                                                                             // Reset
                                                                             {"resetSkyGradient", resetSkyGradient},
                                                                             {"resetHeatHaze", resetHeatHaze},
                                                                             {"resetRainLevel", resetRainLevel},
                                                                             {"resetSunSize", resetSunSize},
                                                                             {"resetSunColor", resetSunColor},
                                                                             {"resetWindVelocity", resetWindVelocity},
                                                                             {"resetFarClipDistance", resetFarClipDistance},
                                                                             {"resetFogDistance", resetFogDistance},
                                                                             {"removeWorldModel", RemoveWorldModel},
                                                                             {"restoreWorldModel", RestoreWorldModel},
                                                                             {"restoreAllWorldModels", RestoreAllWorldModels},
                                                                             {"resetMoonSize", resetMoonSize},
                                                                             {"resetWorldProperties", ArgumentParser<ResetWorldProperties>},

                                                                             // Check
                                                                             {"isGarageOpen", isGarageOpen},
                                                                             {"isGlitchEnabled", isGlitchEnabled},
                                                                             {"isWorldSpecialPropertyEnabled", ArgumentParserWarn<false, isWorldSpecialPropertyEnabled>},
                                                                             {"areTrafficLightsLocked", areTrafficLightsLocked}};

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

int CLuaWorldDefs::getTime(lua_State* luaVM)
{
    // Get the time
    unsigned char ucHour, ucMinute;
    if (CStaticFunctionDefinitions::GetTime(ucHour, ucMinute))
    {
        // Return it
        lua_pushnumber(luaVM, static_cast<lua_Number>(ucHour));
        lua_pushnumber(luaVM, static_cast<lua_Number>(ucMinute));
        return 2;
    }

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getWeather(lua_State* luaVM)
{
    unsigned char ucWeather, ucWeatherBlendingTo;
    if (CStaticFunctionDefinitions::GetWeather(ucWeather, ucWeatherBlendingTo))
    {
        lua_pushnumber(luaVM, static_cast<lua_Number>(ucWeather));

        if (ucWeatherBlendingTo != 0xFF)
            lua_pushnumber(luaVM, static_cast<lua_Number>(ucWeatherBlendingTo));
        else
            lua_pushnil(luaVM);

        return 2;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getZoneName(lua_State* luaVM)
{
    CVector vecPosition;
    bool    bCitiesOnly;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadBool(bCitiesOnly, false);

    if (!argStream.HasErrors())
    {
        SString strZoneName;
        if (CStaticFunctionDefinitions::GetZoneName(vecPosition, strZoneName, bCitiesOnly))
        {
            lua_pushstring(luaVM, strZoneName);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getGravity(lua_State* luaVM)
{
    float fGravity;
    if (CStaticFunctionDefinitions::GetGravity(fGravity))
    {
        lua_pushnumber(luaVM, fGravity);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getGameSpeed(lua_State* luaVM)
{
    float fSpeed;
    if (CStaticFunctionDefinitions::GetGameSpeed(fSpeed))
    {
        lua_pushnumber(luaVM, fSpeed);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getWaveHeight(lua_State* luaVM)
{
    float fHeight;
    if (CStaticFunctionDefinitions::GetWaveHeight(fHeight))
    {
        lua_pushnumber(luaVM, fHeight);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getFPSLimit(lua_State* luaVM)
{
    unsigned short usLimit;
    if (CStaticFunctionDefinitions::GetFPSLimit(usLimit))
    {
        lua_pushnumber(luaVM, usLimit);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getMinuteDuration(lua_State* luaVM)
{
    unsigned long ulDuration;
    if (CStaticFunctionDefinitions::GetMinuteDuration(ulDuration))
    {
        lua_pushnumber(luaVM, ulDuration);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::isGarageOpen(lua_State* luaVM)
{
    //  bool isGarageOpen ( int garageID )
    int iGarageID;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iGarageID);

    if (!argStream.HasErrors())
    {
        bool bIsOpen;
        if (CStaticFunctionDefinitions::IsGarageOpen(iGarageID, bIsOpen))
        {
            lua_pushboolean(luaVM, bIsOpen);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getTrafficLightState(lua_State* luaVM)
{
    unsigned char ucTrafficLightState;
    if (CStaticFunctionDefinitions::GetTrafficLightState(ucTrafficLightState))
    {
        lua_pushnumber(luaVM, ucTrafficLightState);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::areTrafficLightsLocked(lua_State* luaVM)
{
    bool bTrafficLightsLocked;
    if (CStaticFunctionDefinitions::GetTrafficLightsLocked(bTrafficLightsLocked))
    {
        lua_pushboolean(luaVM, bTrafficLightsLocked);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setTime(lua_State* luaVM)
{
    //  bool setTimer ( int hour, int minute )
    unsigned char ucHour, ucMinute;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ucHour);
    argStream.ReadNumber(ucMinute);

    if (!argStream.HasErrors())
    {
        // Set the new time
        if (CStaticFunctionDefinitions::SetTime(ucHour, ucMinute))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setTrafficLightState(lua_State* luaVM)
{
    //  bool setTrafficLightState ( int state )
    //  bool setTrafficLightState ( string state )
    //  bool setTrafficLightState ( string colorNS, string colorEW )

    CScriptArgReader argStream(luaVM);

    // Determine which version to parse
    if (argStream.NextIsNumber())
    {
        //  bool setTrafficLightState ( int state )
        int iState;
        argStream.ReadNumber(iState);

        if (!argStream.HasErrors())
        {
            if (CStaticFunctionDefinitions::SetTrafficLightState(iState))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else if (!argStream.NextIsString(1))
    {
        //  bool setTrafficLightState ( string state )
        TrafficLight::EState eState;
        argStream.ReadEnumString(eState);

        if (!argStream.HasErrors())
        {
            if (eState == TrafficLight::AUTO)
            {
                bool bOk = CStaticFunctionDefinitions::SetTrafficLightsLocked(false) && CStaticFunctionDefinitions::SetTrafficLightState(0);
                lua_pushboolean(luaVM, bOk);
                return 1;
            }
            else
            {
                bool bOk = CStaticFunctionDefinitions::SetTrafficLightsLocked(true) && CStaticFunctionDefinitions::SetTrafficLightState(9);
                lua_pushboolean(luaVM, bOk);
                return 1;
            }
        }
    }
    else
    {
        //  bool setTrafficLightState ( string colorNS, string colorEW )
        TrafficLight::EColor eColorNS;
        TrafficLight::EColor eColorEW;
        argStream.ReadEnumString(eColorNS);
        argStream.ReadEnumString(eColorEW);

        if (!argStream.HasErrors())
        {
            unsigned char ucState = SharedUtil::GetTrafficLightStateFromColors(eColorNS, eColorEW);

            // Change it.
            bool bOk = CStaticFunctionDefinitions::SetTrafficLightsLocked(true) && CStaticFunctionDefinitions::SetTrafficLightState(ucState);
            lua_pushboolean(luaVM, bOk);
            return 1;
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setTrafficLightsLocked(lua_State* luaVM)
{
    //  bool setTrafficLightsLocked ( bool locked )
    bool bTrafficLightsLocked;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bTrafficLightsLocked);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetTrafficLightsLocked(bTrafficLightsLocked))
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

int CLuaWorldDefs::setJetpackMaxHeight(lua_State* luaVM)
{
    //  bool setJatpackMaxHeight ( float height )
    float fMaxHeight;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fMaxHeight);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetJetpackMaxHeight(fMaxHeight))
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

int CLuaWorldDefs::setWeather(lua_State* luaVM)
{
    //  bool setWeather ( int weatherID )
    int iWeather;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iWeather);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetWeather(iWeather))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setWeatherBlended(lua_State* luaVM)
{
    //  bool setWeatherBlended ( int weatherID )
    int iWeather;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iWeather);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetWeatherBlended(iWeather))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setGravity(lua_State* luaVM)
{
    //  bool setGravity ( float level )
    float fGravity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fGravity);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetGravity(fGravity))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setGameSpeed(lua_State* luaVM)
{
    //  bool setGameSpeed ( float value )
    float fGameSpeed;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fGameSpeed);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetGameSpeed(fGameSpeed))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setWaveHeight(lua_State* luaVM)
{
    //  bool setWaveHeight ( float height )
    float fHeight;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fHeight);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetWaveHeight(fHeight))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getSkyGradient(lua_State* luaVM)
{
    unsigned char ucTopR, ucTopG, ucTopB, ucBottomR, ucBottomG, ucBottomB;
    bool          bSuccess = CStaticFunctionDefinitions::GetSkyGradient(ucTopR, ucTopG, ucTopB, ucBottomR, ucBottomG, ucBottomB);

    if (bSuccess)
    {
        lua_pushnumber(luaVM, ucTopR);
        lua_pushnumber(luaVM, ucTopG);
        lua_pushnumber(luaVM, ucTopB);
        lua_pushnumber(luaVM, ucBottomR);
        lua_pushnumber(luaVM, ucBottomG);
        lua_pushnumber(luaVM, ucBottomB);
        return 6;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setSkyGradient(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    // Set the new sky gradient
    uchar ucTopRed, ucTopGreen, ucTopBlue;
    uchar ucBottomRed, ucBottomGreen, ucBottomBlue;

    argStream.ReadNumber(ucTopRed, 0);
    argStream.ReadNumber(ucTopGreen, 0);
    argStream.ReadNumber(ucTopBlue, 0);
    argStream.ReadNumber(ucBottomRed, 0);
    argStream.ReadNumber(ucBottomGreen, 0);
    argStream.ReadNumber(ucBottomBlue, 0);

    if (!argStream.HasErrors())
    {
        // Set the new sky gradient
        if (CStaticFunctionDefinitions::SetSkyGradient(ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::resetSkyGradient(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetSkyGradient())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getHeatHaze(lua_State* luaVM)
{
    SHeatHazeSettings settings;
    bool              bSuccess = CStaticFunctionDefinitions::GetHeatHaze(settings);

    if (bSuccess)
    {
        lua_pushnumber(luaVM, settings.ucIntensity);
        lua_pushnumber(luaVM, settings.ucRandomShift);
        lua_pushnumber(luaVM, settings.usSpeedMin);
        lua_pushnumber(luaVM, settings.usSpeedMax);
        lua_pushnumber(luaVM, settings.sScanSizeX);
        lua_pushnumber(luaVM, settings.sScanSizeY);
        lua_pushnumber(luaVM, settings.usRenderSizeX);
        lua_pushnumber(luaVM, settings.usRenderSizeY);
        lua_pushboolean(luaVM, settings.bInsideBuilding);
        return 9;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setHeatHaze(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    // Set the new heat haze settings
    SHeatHazeSettings heatHaze;
    argStream.ReadNumber(heatHaze.ucIntensity);
    argStream.ReadNumber(heatHaze.ucRandomShift, 0);
    argStream.ReadNumber(heatHaze.usSpeedMin, 12);
    argStream.ReadNumber(heatHaze.usSpeedMax, 18);
    argStream.ReadNumber(heatHaze.sScanSizeX, 75);
    argStream.ReadNumber(heatHaze.sScanSizeY, 80);
    argStream.ReadNumber(heatHaze.usRenderSizeX, 80);
    argStream.ReadNumber(heatHaze.usRenderSizeY, 85);
    argStream.ReadBool(heatHaze.bInsideBuilding, false);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetHeatHaze(heatHaze))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::resetHeatHaze(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetHeatHaze())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getInteriorSoundsEnabled(lua_State* luaVM)
{
    bool bEnabled;
    bool bSuccess = CStaticFunctionDefinitions::GetInteriorSoundsEnabled(bEnabled);

    if (bSuccess)
    {
        lua_pushboolean(luaVM, bEnabled);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getRainLevel(lua_State* luaVM)
{
    float fRainLevel;
    bool  bSuccess = CStaticFunctionDefinitions::GetRainLevel(fRainLevel);

    if (bSuccess)
    {
        lua_pushnumber(luaVM, fRainLevel);

        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getSunSize(lua_State* luaVM)
{
    float fSunSize;
    bool  bSuccess = CStaticFunctionDefinitions::GetSunSize(fSunSize);

    if (bSuccess)
    {
        lua_pushnumber(luaVM, fSunSize);

        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getMoonSize(lua_State* luaVM)
{
    int iMoonSize;
    if (CStaticFunctionDefinitions::GetMoonSize(iMoonSize))
    {
        lua_pushnumber(luaVM, iMoonSize);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getSunColor(lua_State* luaVM)
{
    unsigned char ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG, ucCoronaB;
    bool          bSuccess = CStaticFunctionDefinitions::GetSunColor(ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG, ucCoronaB);

    if (bSuccess)
    {
        lua_pushnumber(luaVM, ucCoreR);
        lua_pushnumber(luaVM, ucCoreG);
        lua_pushnumber(luaVM, ucCoreB);
        lua_pushnumber(luaVM, ucCoronaR);
        lua_pushnumber(luaVM, ucCoronaG);
        lua_pushnumber(luaVM, ucCoronaB);

        return 6;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getWindVelocity(lua_State* luaVM)
{
    float fVelocityX, fVelocityY, fVelocityZ;
    bool  bSuccess = CStaticFunctionDefinitions::GetWindVelocity(fVelocityX, fVelocityY, fVelocityZ);

    if (bSuccess)
    {
        lua_pushnumber(luaVM, fVelocityX);
        lua_pushnumber(luaVM, fVelocityY);
        lua_pushnumber(luaVM, fVelocityZ);

        return 3;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getFarClipDistance(lua_State* luaVM)
{
    float fFarClip;
    bool  bSuccess = CStaticFunctionDefinitions::GetFarClipDistance(fFarClip);

    if (bSuccess)
    {
        lua_pushnumber(luaVM, fFarClip);

        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::getFogDistance(lua_State* luaVM)
{
    float fFogDist;
    bool  bSuccess = CStaticFunctionDefinitions::GetFogDistance(fFogDist);

    if (bSuccess)
    {
        lua_pushnumber(luaVM, fFogDist);

        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setInteriorSoundsEnabled(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    bool bEnable;
    argStream.ReadBool(bEnable);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetInteriorSoundsEnabled(bEnable))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setRainLevel(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    float fRainLevel;
    argStream.ReadNumber(fRainLevel);

    if (!argStream.HasErrors())
    {
        // Clamp amount of rain to avoid game freezing/crash
        fRainLevel = Clamp(0.0f, fRainLevel, 10.0f);

        if (CStaticFunctionDefinitions::SetRainLevel(fRainLevel))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setSunSize(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    float fSunSize;
    argStream.ReadNumber(fSunSize);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetSunSize(fSunSize))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setMoonSize(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    int iMoonSize;
    argStream.ReadNumber(iMoonSize);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetMoonSize(iMoonSize))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setSunColor(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    unsigned char ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG, ucCoronaB;
    argStream.ReadNumber(ucCoreR, 0);
    argStream.ReadNumber(ucCoreG, 0);
    argStream.ReadNumber(ucCoreB, 0);
    argStream.ReadNumber(ucCoronaR, ucCoreR);
    argStream.ReadNumber(ucCoronaG, ucCoreG);
    argStream.ReadNumber(ucCoronaB, ucCoreB);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetSunColor(ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG, ucCoronaB))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setWindVelocity(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    float fVelX, fVelY, fVelZ;
    argStream.ReadNumber(fVelX);
    argStream.ReadNumber(fVelY);
    argStream.ReadNumber(fVelZ);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetWindVelocity(fVelX, fVelY, fVelZ))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setFarClipDistance(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    float fFarClip;
    argStream.ReadNumber(fFarClip);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetFarClipDistance(fFarClip))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setFogDistance(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    float fFogDist;
    argStream.ReadNumber(fFogDist);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetFogDistance(fFogDist))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::resetRainLevel(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetRainLevel())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::resetSunSize(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetSunSize())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::resetSunColor(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetSunColor())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::resetMoonSize(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetMoonSize())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::resetWindVelocity(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetWindVelocity())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::resetFarClipDistance(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetFarClipDistance())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::resetFogDistance(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetFogDistance())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::RemoveWorldModel(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    unsigned short usModel = 0;
    float          fRadius = 0.0f;
    char           cInterior = -1;
    CVector        vecPosition;
    argStream.ReadNumber(usModel);
    argStream.ReadNumber(fRadius);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(cInterior, -1);
    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::RemoveWorldModel(usModel, fRadius, vecPosition, cInterior))
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

int CLuaWorldDefs::RestoreWorldModel(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    unsigned short usModel = 0;
    float          fRadius = 0.0f;
    char           cInterior = -1;
    CVector        vecPosition;
    argStream.ReadNumber(usModel);
    argStream.ReadNumber(fRadius);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(cInterior, -1);
    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::RestoreWorldModel(usModel, fRadius, vecPosition, cInterior))
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

int CLuaWorldDefs::RestoreAllWorldModels(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::RestoreAllWorldModels())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setFPSLimit(lua_State* luaVM)
{
    //  bool setFPSLimit ( int fpsLimit )
    unsigned short usLimit;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(usLimit);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetFPSLimit(usLimit, false))
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

int CLuaWorldDefs::setMinuteDuration(lua_State* luaVM)
{
    unsigned long ulDuration;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ulDuration);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetMinuteDuration(ulDuration))
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

int CLuaWorldDefs::setGarageOpen(lua_State* luaVM)
{
    //  bool setGarageOpen ( int garageID, bool open )
    unsigned char ucGarage;
    bool          bOpen;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ucGarage);
    argStream.ReadBool(bOpen);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetGarageOpen(ucGarage, bOpen))
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

int CLuaWorldDefs::setGlitchEnabled(lua_State* luaVM)
{
    //  bool setGlitchEnabled ( string glitchName, bool enable )
    SString strGlitch;
    bool    bEnabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strGlitch);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetGlitchEnabled(strGlitch, bEnabled))
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

int CLuaWorldDefs::isGlitchEnabled(lua_State* luaVM)
{
    //  bool isGlitchEnabled ( string glitchName )
    SString strGlitch;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strGlitch);

    if (!argStream.HasErrors())
    {
        bool bEnabled;
        if (CStaticFunctionDefinitions::IsGlitchEnabled(strGlitch, bEnabled))
        {
            lua_pushboolean(luaVM, bEnabled);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setJetpackWeaponEnabled(lua_State* luaVM)
{
    eWeaponType      weaponType;
    bool             bEnabled;
    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumStringOrNumber(weaponType);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetJetpackWeaponEnabled(weaponType, bEnabled))
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

bool CLuaWorldDefs::isWorldSpecialPropertyEnabled(WorldSpecialProperty property)
{
    return CStaticFunctionDefinitions::IsWorldSpecialPropertyEnabled(property);
}

bool CLuaWorldDefs::setWorldSpecialPropertyEnabled(WorldSpecialProperty property, bool isEnabled)
{
    return CStaticFunctionDefinitions::SetWorldSpecialPropertyEnabled(property, isEnabled);
}

int CLuaWorldDefs::getJetpackWeaponEnabled(lua_State* luaVM)
{
    eWeaponType      weaponType;
    bool             bEnabled;
    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumStringOrNumber(weaponType);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::GetJetpackWeaponEnabled(weaponType, bEnabled))
        {
            lua_pushboolean(luaVM, bEnabled);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setCloudsEnabled(lua_State* luaVM)
{
    //  bool setCloudsEnabled ( bool enable )
    bool bEnable;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bEnable);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetCloudsEnabled(bEnable))
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

int CLuaWorldDefs::getCloudsEnabled(lua_State* luaVM)
{
    lua_pushboolean(luaVM, CStaticFunctionDefinitions::GetCloudsEnabled());
    return 1;
}

int CLuaWorldDefs::getJetpackMaxHeight(lua_State* luaVM)
{
    float fMaxHeight;
    if (CStaticFunctionDefinitions::GetJetpackMaxHeight(fMaxHeight))
    {
        lua_pushnumber(luaVM, fMaxHeight);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setAircraftMaxHeight(lua_State* luaVM)
{
    //  bool setAircraftMaxHeight ( float height )
    float fMaxHeight;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fMaxHeight);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetAircraftMaxHeight(fMaxHeight))
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

int CLuaWorldDefs::getAircraftMaxHeight(lua_State* luaVM)
{
    float fMaxHeight;
    if (CStaticFunctionDefinitions::GetAircraftMaxHeight(fMaxHeight))
    {
        lua_pushnumber(luaVM, fMaxHeight);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setAircraftMaxVelocity(lua_State* luaVM)
{
    //  bool setAircraftMaxVelocity ( float fVelocity )
    float            fVelocity;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fVelocity);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetAircraftMaxVelocity(fVelocity))
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

int CLuaWorldDefs::getAircraftMaxVelocity(lua_State* luaVM)
{
    float fVelocity;
    if (CStaticFunctionDefinitions::GetAircraftMaxVelocity(fVelocity))
    {
        lua_pushnumber(luaVM, fVelocity);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::setOcclusionsEnabled(lua_State* luaVM)
{
    //  bool setOcclusionsEnabled ( bool enabled )
    bool bEnabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetOcclusionsEnabled(bEnabled))
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

int CLuaWorldDefs::getOcclusionsEnabled(lua_State* luaVM)
{
    bool bEnabled;
    if (CStaticFunctionDefinitions::GetOcclusionsEnabled(bEnabled))
    {
        lua_pushboolean(luaVM, bEnabled);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

void CLuaWorldDefs::ResetWorldProperties(std::optional<bool> resetSpecialWorldProperties, std::optional<bool> resetWorldProperties, std::optional<bool> resetWeatherProperties, std::optional<bool> resetLODs, std::optional<bool> resetSounds, std::optional<bool> resetGlitches, std::optional<bool> resetJetpackWeapons) noexcept
{
    g_pGame->ResetWorldProperties(ResetWorldPropsInfo{resetSpecialWorldProperties.value_or(true), resetWorldProperties.value_or(true), resetWeatherProperties.value_or(true), resetLODs.value_or(true), resetSounds.value_or(true), resetGlitches.value_or(true), resetJetpackWeapons.value_or(true)});
}
