/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.World.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

#define MIN_CLIENT_REQ_CALLREMOTE_QUEUE_NAME                "1.5.3-9.11270"
#define MIN_CLIENT_REQ_FETCHREMOTE_CONNECT_TIMEOUT          "1.3.5"
#define MIN_CLIENT_REQ_CALLREMOTE_OPTIONS_TABLE             "1.5.4-9.11342"
#define MIN_CLIENT_REQ_CALLREMOTE_OPTIONS_FORMFIELDS        "1.5.4-9.11413"

int CLuaFunctionDefs::CreateExplosion ( lua_State* luaVM )
{
//  bool createExplosion ( float x, float y, float z, int type [, bool makeSound = true, float camShake = -1.0, bool damaging = true ] )
    CVector vecPosition; int iType; bool bMakeSound; float fCamShake = -1.0; bool bDamaging;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( iType );
    argStream.ReadBool ( bMakeSound, true );
    argStream.ReadNumber ( fCamShake, -1.0f );
    argStream.ReadBool ( bDamaging, true );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::CreateExplosion ( vecPosition, iType, bMakeSound, fCamShake, bDamaging ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::CreateFire ( lua_State* luaVM )
{
//  bool createFire ( float x, float y, float z [, float size = 1.8 ] )
    CVector vecPosition; float fSize;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( fSize, 1.8f );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::CreateFire ( vecPosition, fSize ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTime_ ( lua_State* luaVM )
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



int CLuaFunctionDefs::GetGroundPosition ( lua_State* luaVM )
{
//  float getGroundPosition ( float x, float y, float z )
    CVector vecStart;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecStart );

    if ( !argStream.HasErrors () )
    {
        // Get the ground position and return it
        float fGround = g_pGame->GetWorld ()->FindGroundZFor3DPosition ( &vecStart );
        lua_pushnumber ( luaVM, fGround );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ProcessLineOfSight ( lua_State * luaVM )
{
//  bool float float float element float float float int int int processLineOfSight ( float startX, float startY, float startZ, float endX, float endY, float endZ,
//      [ bool checkBuildings = true, bool checkVehicles = true, bool checkPlayers = true, bool checkObjects = true, bool checkDummies = true,
//        bool seeThroughStuff = false, bool ignoreSomeObjectsForCamera = false, bool shootThroughStuff = false, element ignoredElement = nil, bool returnBuildingInfo = false,
//        bCheckCarTires = false ] )
    CVector vecStart; CVector vecEnd;
    SLineOfSightFlags flags; CClientEntity* pIgnoredElement; bool bIncludeBuildingInfo;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecStart );
    argStream.ReadVector3D ( vecEnd );
    argStream.ReadBool ( flags.bCheckBuildings, true );
    argStream.ReadBool ( flags.bCheckVehicles, true );
    argStream.ReadBool ( flags.bCheckPeds, true );
    argStream.ReadBool ( flags.bCheckObjects, true );
    argStream.ReadBool ( flags.bCheckDummies, true );
    argStream.ReadBool ( flags.bSeeThroughStuff, false );
    argStream.ReadBool ( flags.bIgnoreSomeObjectsForCamera, false );
    argStream.ReadBool ( flags.bShootThroughStuff, false );
    argStream.ReadUserData ( pIgnoredElement, NULL );
    argStream.ReadBool ( bIncludeBuildingInfo, false );
    argStream.ReadBool ( flags.bCheckCarTires, false );

    if ( !argStream.HasErrors () )
    {
        CEntity* pIgnoredEntity = pIgnoredElement ? pIgnoredElement->GetGameEntity() : NULL;
        CColPoint* pColPoint = NULL;
        CClientEntity* pColEntity = NULL;
        bool bCollision;
        SLineOfSightBuildingResult buildingResult;
        if ( CStaticFunctionDefinitions::ProcessLineOfSight ( vecStart, vecEnd, bCollision, &pColPoint, &pColEntity, flags, pIgnoredEntity, bIncludeBuildingInfo ? &buildingResult : NULL ) )
        {    
            // Got a collision?
            CVector vecColPosition;
            CVector vecColNormal;

            int iMaterial = -1;
            float fLighting = -1;
            int iPiece = -1;

            if ( pColPoint )
            {
                // Get the collision position
                vecColPosition = pColPoint->GetPosition ();
                vecColNormal = pColPoint->GetNormal ();
                iMaterial = pColPoint->GetSurfaceTypeB (); //From test, only B function return relevant data
                fLighting = pColPoint->GetLightingForTimeOfDay ();
                iPiece = pColPoint->GetPieceTypeB ();

                // Delete the colpoint
                pColPoint->Destroy ();
            }

            lua_pushboolean ( luaVM, bCollision );
            if ( bCollision )
            {
                lua_pushnumber ( luaVM, vecColPosition.fX );
                lua_pushnumber ( luaVM, vecColPosition.fY );
                lua_pushnumber ( luaVM, vecColPosition.fZ );

                if ( pColEntity )
                    lua_pushelement ( luaVM, pColEntity );
                else
                    lua_pushnil ( luaVM );

                lua_pushnumber ( luaVM, vecColNormal.fX );
                lua_pushnumber ( luaVM, vecColNormal.fY );
                lua_pushnumber ( luaVM, vecColNormal.fZ );

                lua_pushinteger ( luaVM, iMaterial );
                lua_pushnumber ( luaVM, fLighting );
                lua_pushinteger ( luaVM, iPiece );

                if ( bIncludeBuildingInfo && buildingResult.bValid )
                {
                    lua_pushnumber ( luaVM, buildingResult.usModelID );

                    lua_pushnumber ( luaVM, buildingResult.vecPosition.fX );
                    lua_pushnumber ( luaVM, buildingResult.vecPosition.fY );
                    lua_pushnumber ( luaVM, buildingResult.vecPosition.fZ );

                    lua_pushnumber ( luaVM, ConvertRadiansToDegrees ( buildingResult.vecRotation.fX ) );
                    lua_pushnumber ( luaVM, ConvertRadiansToDegrees ( buildingResult.vecRotation.fY ) );
                    lua_pushnumber ( luaVM, ConvertRadiansToDegrees ( buildingResult.vecRotation.fZ ) );

                    lua_pushnumber ( luaVM, buildingResult.usLODModelID );
                    return 19;
                }
                return 11;
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsLineOfSightClear ( lua_State * luaVM )
{
    //bool isLineOfSightClear ( float startX, 
    //    float startY, 
    //    float startZ, 
    //    float endX, 
    //    float endY, 
    //    float endZ, 
    //    [ bool checkBuildings = true, 
    //    bool checkVehicles = true, 
    //    bool checkPeds = true, 
    //    bool checkObjects = true, 
    //    bool checkDummies = true, 
    //    bool seeThroughStuff = false, 
    //    bool ignoreSomeObjectsForCamera = false, 
    //    element ignoredElement = nil ] )
    CVector vecStart; CVector vecEnd;
    SLineOfSightFlags flags; CClientEntity* pIgnoredElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecStart );
    argStream.ReadVector3D ( vecEnd );
    argStream.ReadBool ( flags.bCheckBuildings, true );
    argStream.ReadBool ( flags.bCheckVehicles, true );
    argStream.ReadBool ( flags.bCheckPeds, true );
    argStream.ReadBool ( flags.bCheckObjects, true );
    argStream.ReadBool ( flags.bCheckDummies, true );
    argStream.ReadBool ( flags.bSeeThroughStuff, false );
    argStream.ReadBool ( flags.bIgnoreSomeObjectsForCamera, false );
    argStream.ReadUserData ( pIgnoredElement, NULL );

    if ( !argStream.HasErrors () )
    {
        CEntity* pIgnoredEntity = pIgnoredElement ? pIgnoredElement->GetGameEntity() : NULL;
        bool bIsClear;
        if ( CStaticFunctionDefinitions::IsLineOfSightClear ( vecStart, vecEnd, bIsClear, flags, pIgnoredEntity ) )
        {        
            lua_pushboolean ( luaVM, bIsClear );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetWorldFromScreenPosition ( lua_State * luaVM )
{
//  float, float, float getWorldFromScreenPosition ( float x, float y, float depth )
    CVector vecScreen;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecScreen );

    if ( !argStream.HasErrors () )
    {
        CVector vecWorld;
        if ( CStaticFunctionDefinitions::GetWorldFromScreenPosition ( vecScreen, vecWorld ) )
        {
            lua_pushnumber ( luaVM, vecWorld.fX );
            lua_pushnumber ( luaVM, vecWorld.fY );
            lua_pushnumber ( luaVM, vecWorld.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetScreenFromWorldPosition ( lua_State * luaVM )
{
//  float float getScreenFromWorldPosition ( float x, float y, float z, [ float edgeTolerance=0, bool relative=true ] )
    CVector vecWorld; float fEdgeTolerance; bool bRelative;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecWorld );
    argStream.ReadNumber ( fEdgeTolerance, 0 );
    argStream.ReadBool ( bRelative, true );

    if ( !argStream.HasErrors () )
    {
        CVector vecScreen;
        if ( CStaticFunctionDefinitions::GetScreenFromWorldPosition ( vecWorld, vecScreen, fEdgeTolerance, bRelative ) )
        {
            lua_pushnumber ( luaVM, vecScreen.fX );
            lua_pushnumber ( luaVM, vecScreen.fY );
            lua_pushnumber ( luaVM, vecScreen.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefs::GetWeather ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetZoneName ( lua_State* luaVM )
{
//  string getZoneName ( float x, float y, float z, [bool citiesonly=false] )
    CVector vecPosition; bool bCitiesOnly;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadBool ( bCitiesOnly, false );

    if ( !argStream.HasErrors () )
    {
        SString strZoneName;
        if ( CStaticFunctionDefinitions::GetZoneName ( vecPosition, strZoneName, bCitiesOnly ) )
        {
            lua_pushstring ( luaVM, strZoneName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetGravity ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetGameSpeed ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetMinuteDuration ( lua_State* luaVM )
{
    unsigned long ulDelay;
    if ( CStaticFunctionDefinitions::GetMinuteDuration ( ulDelay ) )
    {
        lua_pushnumber ( luaVM, ulDelay );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetWaveHeight ( lua_State* luaVM )
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

int CLuaFunctionDefs::IsGarageOpen ( lua_State* luaVM )
{
//  bool isGarageOpen ( int garageID )
    int iGarageID;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iGarageID );

    if ( !argStream.HasErrors () )
    {
        bool bIsOpen;

        if ( CStaticFunctionDefinitions::IsGarageOpen ( iGarageID, bIsOpen ) )
        {
            lua_pushboolean ( luaVM, bIsOpen );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetGaragePosition ( lua_State* luaVM )
{
//  float, float, float getGaragePosition ( int garageID )
    int iGarageID;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iGarageID );

    if ( !argStream.HasErrors () )
    {
        CVector vecPosition;

        if ( CStaticFunctionDefinitions::GetGaragePosition ( iGarageID, vecPosition ) )
        {
            lua_pushnumber ( luaVM, vecPosition.fX );
            lua_pushnumber ( luaVM, vecPosition.fY );
            lua_pushnumber ( luaVM, vecPosition.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetGarageSize ( lua_State* luaVM )
{
//  float, float, float getGarageSize ( int garageID )
    int iGarageID;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iGarageID );

    if ( !argStream.HasErrors () )
    {
        float fDepth;
        float fWidth;
        float fHeight;

        if ( CStaticFunctionDefinitions::GetGarageSize ( iGarageID, fHeight, fWidth, fDepth ) )
        {
            lua_pushnumber ( luaVM, fHeight );
            lua_pushnumber ( luaVM, fWidth );
            lua_pushnumber ( luaVM, fDepth );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetGarageBoundingBox ( lua_State* luaVM )
{
//  float, float, float, float getGarageBoundingBox ( int garageID )
    int iGarageID;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iGarageID );

    if ( !argStream.HasErrors () )
    {
        float fLeft;
        float fRight;
        float fFront;
        float fBack;

        if ( CStaticFunctionDefinitions::GetGarageBoundingBox ( iGarageID, fLeft, fRight, fFront, fBack ) )
        {
            lua_pushnumber ( luaVM, fLeft );
            lua_pushnumber ( luaVM, fRight );
            lua_pushnumber ( luaVM, fFront );
            lua_pushnumber ( luaVM, fBack );
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetTrafficLightState ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pMultiplayer->GetTrafficLightState () );
    return 1;
}

int CLuaFunctionDefs::AreTrafficLightsLocked ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, g_pMultiplayer->GetTrafficLightsLocked () );
    return 1;
}

int CLuaFunctionDefs::GetBlurLevel ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pGame->GetBlurLevel () );
    return 1;
}

int CLuaFunctionDefs::SetBlurLevel ( lua_State* luaVM )
{
//  bool setBlurLevel ( int level )
    int iLevel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iLevel );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetBlurLevel ( static_cast < unsigned char > ( iLevel ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetTime ( lua_State* luaVM )
{
//  bool setTime ( int hour, int minute )
    int iHour; int iMinute;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iHour );
    argStream.ReadNumber ( iMinute );

    if ( !argStream.HasErrors () )
    {
        // Set the new time
        if ( CStaticFunctionDefinitions::SetTime ( static_cast < unsigned char > ( iHour ), static_cast < unsigned char > ( iMinute ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSkyGradient ( lua_State* luaVM )
{
    unsigned char ucTopR, ucTopG, ucTopB, ucBottomR, ucBottomG, ucBottomB;
    CStaticFunctionDefinitions::GetSkyGradient ( ucTopR, ucTopG, ucTopB, ucBottomR, ucBottomG, ucBottomB );

    lua_pushnumber ( luaVM, ucTopR );
    lua_pushnumber ( luaVM, ucTopG );
    lua_pushnumber ( luaVM, ucTopB );
    lua_pushnumber ( luaVM, ucBottomR );
    lua_pushnumber ( luaVM, ucBottomG );
    lua_pushnumber ( luaVM, ucBottomB );
    return 6;
}


int CLuaFunctionDefs::SetSkyGradient ( lua_State* luaVM )
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
        if ( CStaticFunctionDefinitions::SetSkyGradient ( ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ResetSkyGradient ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetSkyGradient () )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetHeatHaze ( lua_State* luaVM )
{
    SHeatHazeSettings settings;
    CStaticFunctionDefinitions::GetHeatHaze ( settings );

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


int CLuaFunctionDefs::SetHeatHaze ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ResetHeatHaze ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetHeatHaze () )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWeather ( lua_State* luaVM )
{
//  bool setWeather ( int weatherID )
    int iWeatherID;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iWeatherID );

    if ( !argStream.HasErrors () )
    {
        // Set the new time
        if ( CStaticFunctionDefinitions::SetWeather ( static_cast < unsigned char > ( iWeatherID ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;        
}


int CLuaFunctionDefs::SetWeatherBlended ( lua_State* luaVM )
{
//  bool setWeatherBlended ( int weatherID )
    int iWeatherID;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iWeatherID );

    if ( !argStream.HasErrors () )
    {
        // Set the new time
        if ( CStaticFunctionDefinitions::SetWeatherBlended ( static_cast < unsigned char > ( iWeatherID ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;        
}


int CLuaFunctionDefs::SetGravity ( lua_State* luaVM )
{
//  bool setGravity ( float level )
    float fGravity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fGravity );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetGravity ( fGravity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetGameSpeed ( lua_State* luaVM )
{
//  bool setGameSpeed ( float value )
    float fSpeed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fSpeed );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetGameSpeed ( fSpeed ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetMinuteDuration ( lua_State* luaVM )
{
//  bool setMinuteDuration ( int milliseconds )
    int iMilliseconds;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iMilliseconds );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetMinuteDuration ( iMilliseconds ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWaveHeight ( lua_State* luaVM )
{
//  bool setWaveHeight ( float height )
    float fHeight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fHeight );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetWaveHeight ( fHeight ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetGarageOpen ( lua_State* luaVM )
{
//  bool setGarageOpen ( int garageID, bool open )
    int iGarageID; bool bOpen;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iGarageID );
    argStream.ReadBool ( bOpen );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetGarageOpen ( iGarageID, bOpen ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetJetpackMaxHeight ( lua_State* luaVM )
{
//  bool setJetpackMaxHeight ( float Height )
    float fHeight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fHeight );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetJetpackMaxHeight ( fHeight ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::RemoveWorldBuilding ( lua_State* luaVM )
{
    int iModelToRemove; 
    CVector vecPosition;
    float fRadius = 0;
    char cInterior = -1;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iModelToRemove );
    argStream.ReadNumber ( fRadius );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( cInterior, -1 );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                uint uiAmount;
                CStaticFunctionDefinitions::RemoveWorldBuilding ( iModelToRemove, fRadius, vecPosition.fX, vecPosition.fY, vecPosition.fZ, cInterior, uiAmount );

                lua_pushboolean ( luaVM, true );
                lua_pushnumber ( luaVM, uiAmount );
                return 2;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::RestoreWorldBuildings ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        CResource* pResource = pLuaMain->GetResource ();
        if ( pResource )
        {
            uint uiAmount;
            CStaticFunctionDefinitions::RestoreWorldBuildings ( uiAmount );

            lua_pushboolean ( luaVM, true );
            lua_pushnumber ( luaVM, uiAmount );
            return 2;
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::RestoreWorldBuilding ( lua_State* luaVM )
{
    int iModelToRestore; 
    CVector vecPosition;
    float fRadius = 0;
    char cInterior = -1;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iModelToRestore );
    argStream.ReadNumber ( fRadius );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( cInterior, -1 );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                uint uiAmount;
                CStaticFunctionDefinitions::RestoreWorldBuilding ( iModelToRestore, fRadius, vecPosition.fX, vecPosition.fY, vecPosition.fZ, cInterior, uiAmount );

                lua_pushboolean ( luaVM, true );
                lua_pushnumber ( luaVM, uiAmount );
                return 2;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetAircraftMaxHeight ( lua_State* luaVM )
{
//  bool setAircraftMaxHeight ( float Height )
    float fHeight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fHeight );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetAircraftMaxHeight ( fHeight ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetAircraftMaxVelocity ( lua_State* luaVM )
{
//  bool setAircraftMaxVelocity ( float fVelocity )
    float fVelocity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fVelocity );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetAircraftMaxVelocity ( fVelocity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetOcclusionsEnabled ( lua_State* luaVM )
{
//  bool setOcclusionsEnabled ( bool enabled )
    bool bEnabled;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadBool ( bEnabled );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetOcclusionsEnabled ( bEnabled ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsWorldSpecialPropertyEnabled ( lua_State* luaVM )
{
//  bool isWorldSpecialPropertyEnabled ( string propname )
    SString strPropName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strPropName );

    if ( !argStream.HasErrors () )
    {
        bool bResult = CStaticFunctionDefinitions::IsWorldSpecialPropertyEnabled ( strPropName );
        lua_pushboolean ( luaVM, bResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWorldSpecialPropertyEnabled ( lua_State* luaVM )
{
//  bool setWorldSpecialPropertyEnabled ( string propname, bool enable )
    SString strPropName; bool bEnable;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strPropName );
    argStream.ReadBool ( bEnable );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetWorldSpecialPropertyEnabled ( strPropName, bEnable ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetCloudsEnabled ( lua_State* luaVM )
{
//  bool setCloudsEnabled ( bool enabled )
    bool bEnabled;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadBool ( bEnabled );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetCloudsEnabled ( bEnabled ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetJetpackMaxHeight ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pGame->GetWorld ()->GetJetpackMaxHeight ( ) );
    return 1;
}

int CLuaFunctionDefs::GetAircraftMaxHeight ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pGame->GetWorld ()->GetAircraftMaxHeight ( ) );
    return 1;
}

int CLuaFunctionDefs::GetAircraftMaxVelocity ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pGame->GetWorld ()->GetAircraftMaxVelocity ( ) );
    return 1;
}

int CLuaFunctionDefs::GetOcclusionsEnabled ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, g_pGame->GetWorld ()->GetOcclusionsEnabled ( ) );
    return 1;
}

int CLuaFunctionDefs::GetCloudsEnabled ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, CStaticFunctionDefinitions::GetCloudsEnabled () );
    return 1;
}


int CLuaFunctionDefs::SetTrafficLightState ( lua_State *luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetTrafficLightsLocked ( lua_State *luaVM )
{
//  bool setTrafficLightsLocked ( bool bLocked )
    bool bLocked;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadBool ( bLocked );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrafficLightsLocked ( bLocked ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1 ;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetWindVelocity ( lua_State *luaVM )
{
    float fX, fY, fZ;

    if ( CStaticFunctionDefinitions::GetWindVelocity ( fX, fY, fZ ) )
    {
        lua_pushnumber ( luaVM, fX );
        lua_pushnumber ( luaVM, fY );
        lua_pushnumber ( luaVM, fZ );
        return 3;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWindVelocity ( lua_State *luaVM )
{
//  bool setWindVelocity ( float velocityX, float velocityY, float velocityZ )
    CVector vecVelocity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecVelocity );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetWindVelocity ( vecVelocity.fX, vecVelocity.fY, vecVelocity.fZ ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::ResetWindVelocity ( lua_State *luaVM )
{
    if ( CStaticFunctionDefinitions::RestoreWindVelocity ( ) )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetInteriorSoundsEnabled ( lua_State* luaVM)
{
    lua_pushboolean ( luaVM, g_pMultiplayer->GetInteriorSoundsEnabled ( ) );
    return 1;
}

int CLuaFunctionDefs::SetInteriorSoundsEnabled ( lua_State* luaVM )
{
//  bool setInteriorSoundsEnabled ( bool enabled )
    bool bEnabled;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadBool ( bEnabled );

    if ( !argStream.HasErrors () )
    {
        g_pMultiplayer->SetInteriorSoundsEnabled ( bEnabled );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetInteriorFurnitureEnabled ( lua_State* luaVM )
{
//  bool getInteriorFurnitureEnabled ( int roomId )
    char cRoomId;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( cRoomId );

    if ( !argStream.HasErrors () )
    {
        if ( cRoomId >= 0 && cRoomId <= 4 )
        {
            lua_pushboolean ( luaVM, g_pMultiplayer->GetInteriorFurnitureEnabled ( cRoomId ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaFunctionDefs::SetInteriorFurnitureEnabled ( lua_State* luaVM )
{
//  bool setInteriorFurnitureEnabled ( int roomId, bool enabled )
    char cRoomId; bool bEnabled;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( cRoomId );
    argStream.ReadBool ( bEnabled );

    if ( !argStream.HasErrors () )
    {
        if ( cRoomId >= 0 && cRoomId <= 4 )
        {
            g_pMultiplayer->SetInteriorFurnitureEnabled ( cRoomId, bEnabled );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetRainLevel ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pGame->GetWeather ()->GetAmountOfRain ());
    return 1;
}

int CLuaFunctionDefs::SetRainLevel ( lua_State* luaVM )
{
//  bool setRainLevel ( float amount )
    float fRainLevel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fRainLevel );

    if ( !argStream.HasErrors () )
    {
        // Clamp amount of rain to avoid game freezing/crash
        fRainLevel = Clamp( 0.0f, fRainLevel, 10.0f );

        g_pGame->GetWeather ()->SetAmountOfRain ( fRainLevel );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::ResetRainLevel ( lua_State* luaVM )
{
    g_pGame->GetWeather ()->ResetAmountOfRain ( );

    lua_pushboolean ( luaVM, true );
    return 1;
}

int CLuaFunctionDefs::GetFarClipDistance ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pMultiplayer->GetFarClipDistance ( ) );
    return 1;
}

int CLuaFunctionDefs::SetFarClipDistance ( lua_State* luaVM )
{
//  bool setFarClipDistance ( float distance )
    float fDistance;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fDistance );

    if ( !argStream.HasErrors () )
    {
        g_pMultiplayer->SetFarClipDistance ( fDistance );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetPedTargetingMarkerEnabled ( lua_State* luaVM )
{
//  bool setPedTargetingMarkerEnabled ( enabled )
    bool bEnabled;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadBool ( bEnabled );

    if ( !argStream.HasErrors () )
    {
        g_pMultiplayer->SetPedTargetingMarkerEnabled ( bEnabled );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsPedTargetingMarkerEnabled ( lua_State* luaVM )
{
//  bool isPedTargetingMarkerEnabled ( )
    lua_pushboolean ( luaVM, g_pMultiplayer->IsPedTargetingMarkerEnabled() );
    return 1;
}

int CLuaFunctionDefs::ResetFarClipDistance ( lua_State* luaVM )
{
    g_pMultiplayer->RestoreFarClipDistance ();

    lua_pushboolean ( luaVM, true );
    return 1;
}

int CLuaFunctionDefs::GetNearClipDistance ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pMultiplayer->GetNearClipDistance ( ) );
    return 1;
}

int CLuaFunctionDefs::SetNearClipDistance ( lua_State* luaVM )
{
//  bool setNearClipDistance ( float distance )
    float fDistance;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fDistance );

    if ( !argStream.HasErrors () )
    {
        g_pMultiplayer->SetNearClipDistance ( fDistance );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::ResetNearClipDistance ( lua_State* luaVM )
{
    g_pMultiplayer->RestoreNearClipDistance ();

    lua_pushboolean ( luaVM, true );
    return 1;
}

int CLuaFunctionDefs::GetVehiclesLODDistance ( lua_State* luaVM )
{
//  float float getVehiclesLODDistance ( )
    float fVehiclesDistance, fTrainsPlanesDistance;

    g_pGame->GetSettings()->GetVehiclesLODDistance ( fVehiclesDistance, fTrainsPlanesDistance );
    lua_pushnumber ( luaVM, fVehiclesDistance );
    lua_pushnumber ( luaVM, fTrainsPlanesDistance );
    return 2;
}

int CLuaFunctionDefs::SetVehiclesLODDistance ( lua_State* luaVM )
{
//  bool setVehiclesLODDistance ( float vehiclesDistance, float trainsAndPlanesDistance = vehiclesDistance * 2.14 )
    float fVehiclesDistance, fTrainsPlanesDistance;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fVehiclesDistance );
    fVehiclesDistance = Clamp ( 0.0f, fVehiclesDistance, 500.0f );

    // Default train distance is 2.14 times bigger than normal vehicles
    argStream.ReadNumber ( fTrainsPlanesDistance, Clamp ( 0.0f, fVehiclesDistance * 2.14f, 500.0f ) );

    if ( !argStream.HasErrors () )
    {
        g_pGame->GetSettings()->SetVehiclesLODDistance ( fVehiclesDistance, fTrainsPlanesDistance );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::ResetVehiclesLODDistance ( lua_State* luaVM )
{
    g_pGame->GetSettings()->ResetVehiclesLODDistance ();
    lua_pushboolean ( luaVM, true );
    return 1;
}

int CLuaFunctionDefs::GetFogDistance ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pMultiplayer->GetFogDistance());
    return 1;
}

int CLuaFunctionDefs::SetFogDistance ( lua_State* luaVM )
{
//  bool setFogDistance ( float distance )
    float fDistance ;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fDistance );

    if ( !argStream.HasErrors () )
    {
        g_pMultiplayer->SetFogDistance ( fDistance );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::ResetFogDistance ( lua_State* luaVM )
{
    g_pMultiplayer->RestoreFogDistance ( );

    lua_pushboolean ( luaVM, true );
    return 1;
}

int CLuaFunctionDefs::GetSunColor ( lua_State* luaVM )
{
    unsigned char ucCoreRed, ucCoreGreen, ucCoreBlue, ucCoronaRed, ucCoronaGreen, ucCoronaBlue;
    
    g_pMultiplayer->GetSunColor ( ucCoreRed, ucCoreGreen, ucCoreBlue, ucCoronaRed, ucCoronaGreen, ucCoronaBlue );

    lua_pushnumber ( luaVM, ucCoreRed );
    lua_pushnumber ( luaVM, ucCoreGreen );
    lua_pushnumber ( luaVM, ucCoreBlue );
    lua_pushnumber ( luaVM, ucCoronaRed );
    lua_pushnumber ( luaVM, ucCoronaGreen );
    lua_pushnumber ( luaVM, ucCoronaBlue );

    return 6;
}

int CLuaFunctionDefs::SetSunColor ( lua_State* luaVM )
{
//  bool setSunColor ( int coreRed, int coreGreen, int coreBlue, int coronaRed, int coronaGreen, int coronaBlue )
    int iCoreRed; int iCoreGreen; int iCoreBlue; int iCoronaRed; int iCoronaGreen; int iCoronaBlue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iCoreRed );
    argStream.ReadNumber ( iCoreGreen );
    argStream.ReadNumber ( iCoreBlue );
    argStream.ReadNumber ( iCoronaRed, iCoreRed );
    argStream.ReadNumber ( iCoronaGreen, iCoreGreen );
    argStream.ReadNumber ( iCoronaBlue, iCoreBlue );

    if ( !argStream.HasErrors () )
    {
        g_pMultiplayer->SetSunColor ( iCoreRed, iCoreGreen, iCoreBlue, iCoronaRed, iCoronaGreen, iCoronaBlue );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, true );
    return 1;
}

int CLuaFunctionDefs::ResetSunColor ( lua_State* luaVM )
{
    g_pMultiplayer->ResetSunColor ( );

    lua_pushboolean ( luaVM, true );
    return 1;
}

int CLuaFunctionDefs::GetSunSize ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pMultiplayer->GetSunSize ( ) );
    return 1;
}

int CLuaFunctionDefs::SetSunSize ( lua_State* luaVM )
{
//  bool setSunSize ( float size )
    float fSize;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fSize );

    if ( !argStream.HasErrors () )
    {
        g_pMultiplayer->SetSunSize ( fSize );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::ResetSunSize ( lua_State* luaVM )
{
    g_pMultiplayer->ResetSunSize ();

    lua_pushboolean ( luaVM, true );
    return 1;
}

int CLuaFunctionDefs::CreateSWATRope ( lua_State* luaVM )
{
    CVector vecPosition;
    DWORD dwDuration = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( dwDuration, 4000 );

    if ( !argStream.HasErrors () )
    {
        // Returns a Rope ID?
        if ( CStaticFunctionDefinitions::CreateSWATRope ( vecPosition, dwDuration ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetBirdsEnabled ( lua_State* luaVM )
{
    bool bEnabled = false;
    CScriptArgReader argStream ( luaVM );

    argStream.ReadBool( bEnabled );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetBirdsEnabled ( bEnabled ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetBirdsEnabled ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::GetBirdsEnabled ( ) )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetMoonSize ( lua_State* luaVM )
{
    int iSize;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber( iSize );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetMoonSize ( iSize ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetMoonSize ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pMultiplayer->GetMoonSize () );
    return 1;
}

int CLuaFunctionDefs::ResetMoonSize ( lua_State* luaVM )
{
    g_pMultiplayer->ResetMoonSize ();
    lua_pushboolean ( luaVM, true );
    return 1;
}

int CLuaFunctionDefs::SetFPSLimit ( lua_State* luaVM )
{
// bool setFPSLimit ( int fpsLimit )
    int iLimit;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber( iLimit );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetFPSLimit ( iLimit ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetFPSLimit ( lua_State* luaVM )
{
    int iLimit;
    if ( CStaticFunctionDefinitions::GetFPSLimit ( iLimit ) )
    {
        lua_pushnumber ( luaVM, iLimit );
        return 1;
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


// Call a function on a remote server
int CLuaFunctionDefs::FetchRemote ( lua_State* luaVM )
{
//  bool fetchRemote ( string URL [, string queueName ][, int connectionAttempts = 10, int connectTimeout = 10000 ], callback callbackFunction, [ string postData, bool bPostBinary, arguments... ] )
//  bool fetchRemote ( string URL [, table options ], callback callbackFunction[, table callbackArguments ] )
    CScriptArgReader argStream ( luaVM );
    SString strURL; SHttpRequestOptions httpRequestOptions; SString strQueueName; CLuaFunctionRef iLuaFunction; CLuaArguments callbackArguments;

    argStream.ReadString ( strURL );
    if (!argStream.NextIsTable())
    {
        if ( argStream.NextIsString () )
            MinClientReqCheck ( argStream, MIN_CLIENT_REQ_CALLREMOTE_QUEUE_NAME, "'queue name' is being used" );
        argStream.ReadIfNextIsString ( strQueueName, CALL_REMOTE_DEFAULT_QUEUE_NAME );
        argStream.ReadIfNextIsNumber ( httpRequestOptions.uiConnectionAttempts, 10 );
        if ( argStream.NextIsNumber () )
            MinClientReqCheck ( argStream, MIN_CLIENT_REQ_FETCHREMOTE_CONNECT_TIMEOUT, "'connect timeout' is being used" );
        argStream.ReadIfNextIsNumber ( httpRequestOptions.uiConnectTimeoutMs, 10000 );
        argStream.ReadFunction ( iLuaFunction );
        argStream.ReadString ( httpRequestOptions.strPostData, "" );
        argStream.ReadBool ( httpRequestOptions.bPostBinary, false );
        argStream.ReadLuaArguments ( callbackArguments );
        argStream.ReadFunctionComplete ();

        if ( !argStream.HasErrors () )
        {
            CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( luaMain )
            {
                httpRequestOptions.bIsLegacy = true;
                g_pClientGame->GetRemoteCalls()->Call(strURL, &callbackArguments, luaMain, iLuaFunction, strQueueName, httpRequestOptions);
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
    {
        CStringMap optionsMap;

        argStream.ReadStringMap(optionsMap);
        argStream.ReadFunction(iLuaFunction);
        if ( argStream.NextIsTable() )
            argStream.ReadLuaArgumentsTable(callbackArguments);
        argStream.ReadFunctionComplete();

        optionsMap.ReadNumber("connectionAttempts", httpRequestOptions.uiConnectionAttempts, 10);
        optionsMap.ReadNumber("connectTimeout", httpRequestOptions.uiConnectTimeoutMs, 10000);
        optionsMap.ReadString("method", httpRequestOptions.strRequestMethod, "");
        optionsMap.ReadString("queueName", strQueueName, CALL_REMOTE_DEFAULT_QUEUE_NAME);
        optionsMap.ReadString("postData", httpRequestOptions.strPostData, "");
        optionsMap.ReadBool("postIsBinary", httpRequestOptions.bPostBinary, false);
        optionsMap.ReadNumber("maxRedirects", httpRequestOptions.uiMaxRedirects, 8);
        optionsMap.ReadString("username", httpRequestOptions.strUsername, "");
        optionsMap.ReadString("password", httpRequestOptions.strPassword, "");
        optionsMap.ReadStringMap("headers", httpRequestOptions.requestHeaders);
        optionsMap.ReadStringMap("formFields", httpRequestOptions.formFields);

        if (httpRequestOptions.formFields.empty())
            MinClientReqCheck(argStream, MIN_CLIENT_REQ_CALLREMOTE_OPTIONS_TABLE, "'options' table is being used");
        else
            MinClientReqCheck(argStream, MIN_CLIENT_REQ_CALLREMOTE_OPTIONS_FORMFIELDS, "'formFields' is being used");

        if (!argStream.HasErrors())
        {
            CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (luaMain)
            {
                g_pClientGame->GetRemoteCalls ()->Call(strURL, &callbackArguments, luaMain, iLuaFunction, strQueueName, httpRequestOptions);
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

