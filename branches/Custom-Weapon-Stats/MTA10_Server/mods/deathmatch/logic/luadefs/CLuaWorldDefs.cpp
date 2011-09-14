/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaWorldDefs.cpp
*  PURPOSE:     Lua game world function definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               lil_Toady <>
*               Kevin Whiteside <>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaWorldDefs::LoadFunctions ( void )
{
    // Get
    CLuaCFunctions::AddFunction ( "getTime", CLuaWorldDefs::getTime );
    CLuaCFunctions::AddFunction ( "getWeather", CLuaWorldDefs::getWeather );
    CLuaCFunctions::AddFunction ( "getZoneName", CLuaWorldDefs::getZoneName );
    CLuaCFunctions::AddFunction ( "getGravity", CLuaWorldDefs::getGravity );
    CLuaCFunctions::AddFunction ( "getGameSpeed", CLuaWorldDefs::getGameSpeed );
    CLuaCFunctions::AddFunction ( "getWaveHeight", CLuaWorldDefs::getWaveHeight );
    CLuaCFunctions::AddFunction ( "getFPSLimit", CLuaWorldDefs::getFPSLimit );
    CLuaCFunctions::AddFunction ( "getMinuteDuration", CLuaWorldDefs::getMinuteDuration );
    CLuaCFunctions::AddFunction ( "isGarageOpen", CLuaWorldDefs::isGarageOpen );
    CLuaCFunctions::AddFunction ( "getTrafficLightState", CLuaWorldDefs::getTrafficLightState );
    CLuaCFunctions::AddFunction ( "areTrafficLightsLocked", CLuaWorldDefs::areTrafficLightsLocked );
    CLuaCFunctions::AddFunction ( "getJetpackMaxHeight", CLuaWorldDefs::getJetpackMaxHeight );
    CLuaCFunctions::AddFunction ( "getSkyGradient", CLuaWorldDefs::getSkyGradient );
    CLuaCFunctions::AddFunction ( "getHeatHaze", CLuaWorldDefs::getHeatHaze );
    CLuaCFunctions::AddFunction ( "isGlitchEnabled", CLuaWorldDefs::isGlitchEnabled );
    CLuaCFunctions::AddFunction ( "getCloudsEnabled", CLuaWorldDefs::getCloudsEnabled );
    CLuaCFunctions::AddFunction ( "getInteriorSoundsEnabled", CLuaWorldDefs::getInteriorSoundsEnabled );
    CLuaCFunctions::AddFunction ( "getRainLevel", CLuaWorldDefs::getRainLevel );
    CLuaCFunctions::AddFunction ( "getSunSize", CLuaWorldDefs::getSunSize );
    CLuaCFunctions::AddFunction ( "getSunColor", CLuaWorldDefs::getSunColor );
    CLuaCFunctions::AddFunction ( "getWindVelocity", CLuaWorldDefs::getWindVelocity );
    CLuaCFunctions::AddFunction ( "getFarClipDistance", CLuaWorldDefs::getFarClipDistance );
    CLuaCFunctions::AddFunction ( "getFogDistance", CLuaWorldDefs::getFogDistance );
    CLuaCFunctions::AddFunction ( "getAircraftMaxHeight", CLuaWorldDefs::getAircraftMaxHeight );

    // Set
    CLuaCFunctions::AddFunction ( "setTime", CLuaWorldDefs::setTime );
    CLuaCFunctions::AddFunction ( "setWeather", CLuaWorldDefs::setWeather );
    CLuaCFunctions::AddFunction ( "setWeatherBlended", CLuaWorldDefs::setWeatherBlended );
    CLuaCFunctions::AddFunction ( "setGravity", CLuaWorldDefs::setGravity );
    CLuaCFunctions::AddFunction ( "setGameSpeed", CLuaWorldDefs::setGameSpeed );
    CLuaCFunctions::AddFunction ( "setWaveHeight", CLuaWorldDefs::setWaveHeight );
    CLuaCFunctions::AddFunction ( "setSkyGradient", CLuaWorldDefs::setSkyGradient );
    CLuaCFunctions::AddFunction ( "setHeatHaze", CLuaWorldDefs::setHeatHaze );
    CLuaCFunctions::AddFunction ( "setFPSLimit", CLuaWorldDefs::setFPSLimit );
    CLuaCFunctions::AddFunction ( "setMinuteDuration", CLuaWorldDefs::setMinuteDuration );
    CLuaCFunctions::AddFunction ( "setGarageOpen", CLuaWorldDefs::setGarageOpen );
    CLuaCFunctions::AddFunction ( "setGlitchEnabled", CLuaWorldDefs::setGlitchEnabled );
    CLuaCFunctions::AddFunction ( "setCloudsEnabled", CLuaWorldDefs::setCloudsEnabled );
    CLuaCFunctions::AddFunction ( "setTrafficLightState", CLuaWorldDefs::setTrafficLightState );
    CLuaCFunctions::AddFunction ( "setTrafficLightsLocked", CLuaWorldDefs::setTrafficLightsLocked );
    CLuaCFunctions::AddFunction ( "setJetpackMaxHeight", CLuaWorldDefs::setJetpackMaxHeight );
    CLuaCFunctions::AddFunction ( "setInteriorSoundsEnabled", CLuaWorldDefs::setInteriorSoundsEnabled );
    CLuaCFunctions::AddFunction ( "setRainLevel", CLuaWorldDefs::setRainLevel );
    CLuaCFunctions::AddFunction ( "setSunSize", CLuaWorldDefs::setSunSize );
    CLuaCFunctions::AddFunction ( "setSunColor", CLuaWorldDefs::setSunColor );
    CLuaCFunctions::AddFunction ( "setWindVelocity", CLuaWorldDefs::setWindVelocity );
    CLuaCFunctions::AddFunction ( "setFarClipDistance", CLuaWorldDefs::setFarClipDistance );
    CLuaCFunctions::AddFunction ( "setFogDistance", CLuaWorldDefs::setFogDistance );
    CLuaCFunctions::AddFunction ( "setAircraftMaxHeight", CLuaWorldDefs::setAircraftMaxHeight );

    // Reset
    CLuaCFunctions::AddFunction ( "resetSkyGradient", CLuaWorldDefs::resetSkyGradient );
    CLuaCFunctions::AddFunction ( "resetHeatHaze", CLuaWorldDefs::resetHeatHaze );
    CLuaCFunctions::AddFunction ( "resetRainLevel", CLuaWorldDefs::resetRainLevel );
    CLuaCFunctions::AddFunction ( "resetSunSize", CLuaWorldDefs::resetSunSize );
    CLuaCFunctions::AddFunction ( "resetSunColor", CLuaWorldDefs::resetSunColor );
    CLuaCFunctions::AddFunction ( "resetWindVelocity", CLuaWorldDefs::resetWindVelocity );
    CLuaCFunctions::AddFunction ( "resetFarClipDistance", CLuaWorldDefs::resetFarClipDistance );
    CLuaCFunctions::AddFunction ( "resetFogDistance", CLuaWorldDefs::resetFogDistance );
}


int CLuaWorldDefs::getTime ( lua_State* luaVM )
{
    // Get the time
    unsigned char ucHour, ucMinute;
    if ( CStaticFunctionDefinitions::GetTime ( ucHour, ucMinute ) )
    {
        // Return it
        lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucHour ) );
        lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucMinute ) );
        return 2;
    }

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::getWeather ( lua_State* luaVM )
{
    unsigned char ucWeather, ucWeatherBlendingTo;
    if ( CStaticFunctionDefinitions::GetWeather ( ucWeather, ucWeatherBlendingTo ) )
    {
        lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucWeather ) );

        if ( ucWeatherBlendingTo != 0xFF )
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucWeatherBlendingTo ) );
        else
            lua_pushnil ( luaVM );

        return 2;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::getZoneName ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TSTRING || iArgument1 == LUA_TNUMBER ) &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) &&
         ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        bool bCitiesOnly = false;
        if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )
            bCitiesOnly = ( lua_toboolean ( luaVM, 4 ) ) ? true:false;

        char szZoneName [ 128 ];
        if ( CStaticFunctionDefinitions::GetZoneName ( vecPosition, szZoneName, 128, bCitiesOnly ) )
        {
            lua_pushstring ( luaVM, szZoneName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getZoneName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::getGravity ( lua_State* luaVM )
{
    float fGravity;
    if ( CStaticFunctionDefinitions::GetGravity ( fGravity ) )
    {
        lua_pushnumber ( luaVM, fGravity );
        return 1;
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::getGameSpeed ( lua_State* luaVM )
{
    float fSpeed;
    if ( CStaticFunctionDefinitions::GetGameSpeed ( fSpeed ) )
    {
        lua_pushnumber ( luaVM, fSpeed );
        return 1;
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::getWaveHeight ( lua_State* luaVM )
{
    float fHeight;
    if ( CStaticFunctionDefinitions::GetWaveHeight ( fHeight ) )
    {
        lua_pushnumber ( luaVM, fHeight );
        return 1;
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::getFPSLimit ( lua_State* luaVM )
{
    unsigned short usLimit;
    if ( CStaticFunctionDefinitions::GetFPSLimit ( usLimit ) )
    {
        lua_pushnumber ( luaVM, usLimit );
        return 1;
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::getMinuteDuration ( lua_State* luaVM )
{
    unsigned long ulDuration;
    if ( CStaticFunctionDefinitions::GetMinuteDuration ( ulDuration ) )
    {
        lua_pushnumber ( luaVM, ulDuration );
        return 1;
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::isGarageOpen ( lua_State* luaVM )
{
    if ( lua_type( luaVM, 1 ) == LUA_TNUMBER )
    {
        unsigned char ucGarageID = static_cast < unsigned char > ( lua_tonumber( luaVM, 1 ) );
        bool bIsOpen;

        if ( CStaticFunctionDefinitions::IsGarageOpen ( ucGarageID, bIsOpen ) )
        {
            lua_pushboolean ( luaVM, bIsOpen );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "isGarageOpen" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isGarageOpen" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::getTrafficLightState ( lua_State* luaVM )
{
    unsigned char ucTrafficLightState;
    if ( CStaticFunctionDefinitions::GetTrafficLightState ( ucTrafficLightState ) )
    {
        lua_pushnumber ( luaVM, ucTrafficLightState );
        return 1;
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::areTrafficLightsLocked ( lua_State* luaVM )
{
    bool bTrafficLightsLocked;
    if ( CStaticFunctionDefinitions::GetTrafficLightsLocked ( bTrafficLightsLocked ) )
    {
        lua_pushboolean ( luaVM, bTrafficLightsLocked );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setTime ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        // Set the new time
        if ( CStaticFunctionDefinitions::SetTime ( static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) ), static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTime" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setTrafficLightState ( lua_State* luaVM )
{
//  bool setTrafficLightState ( int state )
//  bool setTrafficLightState ( string state )
//  bool setTrafficLightState ( string colorNS, string colorEW )

    CScriptArgReader argStream ( luaVM );

    // Determine which version to parse
    if ( argStream.NextIsNumber () )
    {
    //  bool setTrafficLightState ( int state )
        int iState;
        argStream.ReadNumber ( iState );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetTrafficLightState ( iState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
    if ( !argStream.NextIsString ( 1 ) )
    {
    //  bool setTrafficLightState ( string state )
        TrafficLight::EState eState;
        argStream.ReadEnumString ( eState );

        if ( !argStream.HasErrors () )
        {
            if ( eState == TrafficLight::AUTO )
            {
                bool bOk = CStaticFunctionDefinitions::SetTrafficLightsLocked ( false ) &&
                           CStaticFunctionDefinitions::SetTrafficLightState ( 0 );
                lua_pushboolean ( luaVM, bOk );
                return 1;
            }
            else
            {
                bool bOk = CStaticFunctionDefinitions::SetTrafficLightsLocked ( true ) &&
                           CStaticFunctionDefinitions::SetTrafficLightState ( 9 );
                lua_pushboolean ( luaVM, bOk );
                return 1;
            }
        }
    }
    else
    {
    //  bool setTrafficLightState ( string colorNS, string colorEW )
        TrafficLight::EColor eColorNS;
        TrafficLight::EColor eColorEW;
        argStream.ReadEnumString ( eColorNS );
        argStream.ReadEnumString ( eColorEW );

        if ( !argStream.HasErrors () )
        {
            unsigned char ucState = SharedUtil::GetTrafficLightStateFromColors ( eColorNS, eColorEW );

            // Change it.
            bool bOk = CStaticFunctionDefinitions::SetTrafficLightsLocked ( true ) &&
                       CStaticFunctionDefinitions::SetTrafficLightState ( ucState );
            lua_pushboolean ( luaVM, bOk );
            return 1;
        }
    }

    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setTrafficLightState", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setTrafficLightsLocked ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TBOOLEAN )
    {
        bool bTrafficLightsLocked = ( ( lua_toboolean( luaVM, 1 ) == 0 ) ? false : true );
        if ( CStaticFunctionDefinitions::SetTrafficLightsLocked ( bTrafficLightsLocked ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTrafficLightsLocked" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::setJetpackMaxHeight ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        float fMaxHeight = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetJetpackMaxHeight ( fMaxHeight ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setJetpackMaxHeight" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::setWeather ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        // Set the new time
        if ( CStaticFunctionDefinitions::SetWeather ( static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWeather" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;        
}


int CLuaWorldDefs::setWeatherBlended ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        // Set the new time
        if ( CStaticFunctionDefinitions::SetWeatherBlended ( static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWeatherBlended" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;        
}


int CLuaWorldDefs::setGravity ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        float fGravity = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetGravity ( fGravity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setGravity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::setGameSpeed ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        float fSpeed = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetGameSpeed ( fSpeed ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setGameSpeed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::setWaveHeight ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        float fHeight = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetWaveHeight ( fHeight ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWaveHeight" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::getSkyGradient ( lua_State* luaVM )
{
    unsigned char ucTopR, ucTopG, ucTopB, ucBottomR, ucBottomG, ucBottomB;
    bool bSuccess = CStaticFunctionDefinitions::GetSkyGradient ( ucTopR, ucTopG, ucTopB, ucBottomR, ucBottomG, ucBottomB );

    if ( bSuccess )
    {
        lua_pushnumber ( luaVM, ucTopR );
        lua_pushnumber ( luaVM, ucTopG );
        lua_pushnumber ( luaVM, ucTopB );
        lua_pushnumber ( luaVM, ucBottomR );
        lua_pushnumber ( luaVM, ucBottomG );
        lua_pushnumber ( luaVM, ucBottomB );
        return 6;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::setSkyGradient ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );

    // Set the new sky gradient
    uchar ucTopRed, ucTopGreen, ucTopBlue;
    uchar ucBottomRed, ucBottomGreen, ucBottomBlue;

    argStream.ReadNumber ( ucTopRed, 0 );
    argStream.ReadNumber ( ucTopGreen, 0 );
    argStream.ReadNumber ( ucTopBlue, 0 );
    argStream.ReadNumber ( ucBottomRed, 0 );
    argStream.ReadNumber ( ucBottomGreen, 0 );
    argStream.ReadNumber ( ucBottomBlue, 0 );

    if ( !argStream.HasErrors () )
    {
        // Set the new sky gradient
        if ( CStaticFunctionDefinitions::SetSkyGradient ( ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setSkyGradient" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::resetSkyGradient ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetSkyGradient () )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::getHeatHaze ( lua_State* luaVM )
{
    SHeatHazeSettings settings;
    bool bSuccess = CStaticFunctionDefinitions::GetHeatHaze ( settings );

    if ( bSuccess )
    {
        lua_pushnumber ( luaVM, settings.ucIntensity );
        lua_pushnumber ( luaVM, settings.ucRandomShift );
        lua_pushnumber ( luaVM, settings.usSpeedMin );
        lua_pushnumber ( luaVM, settings.usSpeedMax );
        lua_pushnumber ( luaVM, settings.sScanSizeX );
        lua_pushnumber ( luaVM, settings.sScanSizeY );
        lua_pushnumber ( luaVM, settings.usRenderSizeX );
        lua_pushnumber ( luaVM, settings.usRenderSizeY );
        lua_pushboolean ( luaVM, settings.bInsideBuilding );
        return 9;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::setHeatHaze ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );

    // Set the new heat haze settings
    SHeatHazeSettings heatHaze;
    argStream.ReadNumber ( heatHaze.ucIntensity );
    argStream.ReadNumber ( heatHaze.ucRandomShift, 0 );
    argStream.ReadNumber ( heatHaze.usSpeedMin, 12 );
    argStream.ReadNumber ( heatHaze.usSpeedMax, 18 );
    argStream.ReadNumber ( heatHaze.sScanSizeX, 75 );
    argStream.ReadNumber ( heatHaze.sScanSizeY, 80 );
    argStream.ReadNumber ( heatHaze.usRenderSizeX, 80 );
    argStream.ReadNumber ( heatHaze.usRenderSizeY, 85 );
    argStream.ReadBool ( heatHaze.bInsideBuilding, false );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetHeatHaze ( heatHaze ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setHeatHaze" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::resetHeatHaze ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetHeatHaze () )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::getInteriorSoundsEnabled ( lua_State* luaVM)
{
    bool bEnabled;
    bool bSuccess = CStaticFunctionDefinitions::GetInteriorSoundsEnabled ( bEnabled );

    if ( bSuccess )
    {
        lua_pushboolean ( luaVM, bEnabled );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::getRainLevel ( lua_State* luaVM )
{
    float fRainLevel;
    bool bSuccess = CStaticFunctionDefinitions::GetRainLevel ( fRainLevel );

    if ( bSuccess )
    {
        lua_pushnumber ( luaVM, fRainLevel );

        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::getSunSize ( lua_State* luaVM )
{
    float fSunSize;
    bool bSuccess = CStaticFunctionDefinitions::GetSunSize ( fSunSize );

    if ( bSuccess )
    {
        lua_pushnumber ( luaVM, fSunSize );

        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::getSunColor ( lua_State* luaVM )
{
    unsigned char ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG, ucCoronaB;
    bool bSuccess = CStaticFunctionDefinitions::GetSunColor ( ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG, ucCoronaB );

    if ( bSuccess )
    {
        lua_pushnumber ( luaVM, ucCoreR );
        lua_pushnumber ( luaVM, ucCoreG );
        lua_pushnumber ( luaVM, ucCoreB );
        lua_pushnumber ( luaVM, ucCoronaR );
        lua_pushnumber ( luaVM, ucCoronaG );
        lua_pushnumber ( luaVM, ucCoronaB );

        return 6;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::getWindVelocity ( lua_State* luaVM )
{
    float fVelocityX, fVelocityY, fVelocityZ;
    bool bSuccess = CStaticFunctionDefinitions::GetWindVelocity ( fVelocityX, fVelocityY, fVelocityZ );

    if ( bSuccess )
    {
        lua_pushnumber ( luaVM, fVelocityX );
        lua_pushnumber ( luaVM, fVelocityY );
        lua_pushnumber ( luaVM, fVelocityZ );

        return 3;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::getFarClipDistance ( lua_State* luaVM )
{
    float fFarClip;
    bool bSuccess = CStaticFunctionDefinitions::GetFarClipDistance ( fFarClip );

    if ( bSuccess )
    {
        lua_pushnumber ( luaVM, fFarClip );

        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::getFogDistance ( lua_State* luaVM )
{
    float fFogDist;
    bool bSuccess = CStaticFunctionDefinitions::GetFogDistance ( fFogDist );

    if ( bSuccess )
    {
        lua_pushnumber ( luaVM, fFogDist );

        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setInteriorSoundsEnabled ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );

    bool bEnable;
    argStream.ReadBool ( bEnable );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetInteriorSoundsEnabled ( bEnable ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setInteriorSoundsEnabled" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setRainLevel ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );

    float fRainLevel;
    argStream.ReadNumber ( fRainLevel );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetRainLevel ( fRainLevel ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setRainLevel" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setSunSize ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );

    float fSunSize;
    argStream.ReadNumber ( fSunSize );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetSunSize ( fSunSize ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setSunSize" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setSunColor ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );

    unsigned char ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG, ucCoronaB;
    argStream.ReadNumber ( ucCoreR, 0 );
    argStream.ReadNumber ( ucCoreG, 0 );
    argStream.ReadNumber ( ucCoreB, 0 );
    argStream.ReadNumber ( ucCoronaR, ucCoreR );
    argStream.ReadNumber ( ucCoronaG, ucCoreG );
    argStream.ReadNumber ( ucCoronaB, ucCoreB );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetSunColor ( ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG, ucCoronaB ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setSunColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setWindVelocity ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );

    float fVelX, fVelY, fVelZ;
    argStream.ReadNumber ( fVelX );
    argStream.ReadNumber ( fVelY );
    argStream.ReadNumber ( fVelZ );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetWindVelocity ( fVelX, fVelY, fVelZ ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWindVelocity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setFarClipDistance ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );

    float fFarClip;
    argStream.ReadNumber ( fFarClip );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetFarClipDistance ( fFarClip ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setFarClipDistance" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setFogDistance ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );

    float fFogDist;
    argStream.ReadNumber ( fFogDist );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetFogDistance ( fFogDist ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setFogDistance" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::resetRainLevel ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetRainLevel ( ) )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::resetSunSize ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetSunSize ( ) )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::resetSunColor ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetSunColor ( ) )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::resetWindVelocity ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetWindVelocity ( ) )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::resetFarClipDistance ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetFarClipDistance ( ) )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::resetFogDistance ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetFogDistance ( ) )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setFPSLimit ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING || iArgument1 == LUA_TBOOLEAN )
    {
        unsigned short usLimit = static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetFPSLimit ( usLimit, false ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setFPSLimit" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaWorldDefs::setMinuteDuration ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned long ulDuration = static_cast < unsigned long > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetMinuteDuration ( ulDuration ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setGarageOpen ( lua_State* luaVM )
{
    if ( lua_type( luaVM, 1 ) == LUA_TNUMBER && lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        unsigned char ucGarageID = static_cast < unsigned char > ( lua_tonumber( luaVM, 1 ) );
        bool bIsOpen = ( ( lua_toboolean( luaVM, 2 ) == 0 ) ? false : true );

        if ( CStaticFunctionDefinitions::SetGarageOpen ( ucGarageID, bIsOpen ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "setGarageOpen" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setGarageOpen" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setGlitchEnabled ( lua_State* luaVM )
{
    int iArgument = lua_type ( luaVM, 1 );
    if ( iArgument == LUA_TSTRING )
    {
        iArgument = lua_type ( luaVM, 2 );
        if ( iArgument == LUA_TBOOLEAN )
        {
            std::string szGlitchName = lua_tostring ( luaVM, 1 );
            bool bEnabled = (lua_toboolean ( luaVM, 2 ) == 1);
            if ( CStaticFunctionDefinitions::SetGlitchEnabled ( szGlitchName, bEnabled ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
        {
            m_pScriptDebugging->LogBadType ( luaVM, "setGlitchEnabled" );
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setGlitchEnabled" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::isGlitchEnabled ( lua_State* luaVM )
{
    int iArgument = lua_type ( luaVM, 1 );
    if ( iArgument == LUA_TSTRING )
    {
        std::string szGlitchName = lua_tostring ( luaVM, 1 );
        bool bEnabled;
        if ( CStaticFunctionDefinitions::IsGlitchEnabled ( szGlitchName, bEnabled ) )
        {
            lua_pushboolean ( luaVM, bEnabled );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isGlitchEnabled" );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaWorldDefs::setCloudsEnabled ( lua_State* luaVM )
{
    int iArgument = lua_type ( luaVM, 1 );
    if ( iArgument == LUA_TBOOLEAN )
    {
        bool bEnabled = lua_toboolean ( luaVM, 1 ) ? true : false;
        if ( CStaticFunctionDefinitions::SetCloudsEnabled ( bEnabled ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setCloudsEnabled" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::getCloudsEnabled ( lua_State* luaVM )
{
     lua_pushboolean ( luaVM, CStaticFunctionDefinitions::GetCloudsEnabled ( ) );
     return 1;
}

int CLuaWorldDefs::getJetpackMaxHeight ( lua_State* luaVM )
{
    float fMaxHeight;
    if ( CStaticFunctionDefinitions::GetJetpackMaxHeight ( fMaxHeight ) )
    {
        lua_pushnumber ( luaVM, fMaxHeight );
        return 1;
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::setAircraftMaxHeight ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        float fMaxHeight = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetAircraftMaxHeight ( fMaxHeight ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setAircraftMaxHeight" );
        
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaWorldDefs::getAircraftMaxHeight ( lua_State* luaVM )
{
    float fMaxHeight;
    if ( CStaticFunctionDefinitions::GetAircraftMaxHeight ( fMaxHeight ) )
    {
        lua_pushnumber ( luaVM, fMaxHeight );
        return 1;
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}

