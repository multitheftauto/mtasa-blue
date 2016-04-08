/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Utility.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Ed Lyons <>
*               Oliver Brown <>
*               Jax <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


int CLuaFunctionDefs::GetColorFromString ( lua_State* luaVM )
{
    SString strColor;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strColor );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucColorRed, ucColorGreen, ucColorBlue, ucColorAlpha;
        if ( XMLColorToInt ( strColor, ucColorRed, ucColorGreen, ucColorBlue, ucColorAlpha ) )
        {
            lua_pushnumber ( luaVM, ucColorRed );
            lua_pushnumber ( luaVM, ucColorGreen );
            lua_pushnumber ( luaVM, ucColorBlue );
            lua_pushnumber ( luaVM, ucColorAlpha );
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::Reference ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );

    if ( !argStream.NextIsNil () && !argStream.NextIsNone () )
    {
        int iPointer = lua_ref ( luaVM, 1 );
        lua_pushnumber ( luaVM, iPointer );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::Dereference ( lua_State* luaVM )
{
    int iPointer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iPointer );

    if ( !argStream.HasErrors () )
    {
        lua_getref ( luaVM, iPointer );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}




int CLuaFunctionDefs::GetDistanceBetweenPoints2D ( lua_State* luaVM )
{
    CVector2D vecA, vecB;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector2D ( vecA );
    argStream.ReadVector2D ( vecB );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, DistanceBetweenPoints2D ( vecA, vecB ) );

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetDistanceBetweenPoints3D ( lua_State* luaVM )
{
    CVector vecA, vecB;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecA );
    argStream.ReadVector3D ( vecB );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, DistanceBetweenPoints3D ( vecA, vecB ) );

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetEasingValue ( lua_State* luaVM )
{
    //  float getEasingValue( float fProgress, string strEasingType [, float fEasingPeriod, float fEasingAmplitude, float fEasingOvershoot] )
    float fProgress; CEasingCurve::eType easingType; float fEasingPeriod; float fEasingAmplitude; float fEasingOvershoot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fProgress );
    argStream.ReadEnumString ( easingType );
    argStream.ReadNumber ( fEasingPeriod, 0.3f );
    argStream.ReadNumber ( fEasingAmplitude, 1.0f );
    argStream.ReadNumber ( fEasingOvershoot, 1.70158f );

    if ( argStream.HasErrors () )
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CEasingCurve easingCurve ( easingType );
    easingCurve.SetParams ( fEasingPeriod, fEasingAmplitude, fEasingOvershoot );
    lua_pushnumber ( luaVM, easingCurve.ValueForProgress ( fProgress ) );
    return 1;
}

int CLuaFunctionDefs::InterpolateBetween ( lua_State* luaVM )
{
    //  float float float interpolateBetween ( float x1, float y1, float z1, 
    //      float x2, float y2, float z2, 
    //      float fProgress, string strEasingType, 
    //      [ float fEasingPeriod, float fEasingAmplitude, float fEasingOvershoot ] )
    CVector vecPointA; CVector vecPointB;
    float fProgress; CEasingCurve::eType easingType;
    float fEasingPeriod; float fEasingAmplitude; float fEasingOvershoot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPointA );
    argStream.ReadVector3D ( vecPointB );
    argStream.ReadNumber ( fProgress );
    argStream.ReadEnumString ( easingType );
    argStream.ReadNumber ( fEasingPeriod, 0.3f );
    argStream.ReadNumber ( fEasingAmplitude, 1.0f );
    argStream.ReadNumber ( fEasingOvershoot, 1.70158f );

    if ( argStream.HasErrors () )
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CVector vecResult = TInterpolation < CVector >::Interpolate ( vecPointA, vecPointB, fProgress, easingType, fEasingPeriod, fEasingAmplitude, fEasingOvershoot );
    lua_pushnumber ( luaVM, vecResult.fX );
    lua_pushnumber ( luaVM, vecResult.fY );
    lua_pushnumber ( luaVM, vecResult.fZ );
    return 3;
}

int CLuaFunctionDefs::GetTickCount_ ( lua_State* luaVM )
{
    double dTime = static_cast < double > ( GetTickCount64_() );
    lua_pushnumber ( luaVM, dTime );
    return 1;
}

int CLuaFunctionDefs::GetCTime ( lua_State* luaVM )
{
    // table getRealTime( [int seconds = current], bool localTime = true )
    time_t timer;
    time ( &timer );
    bool bLocalTime = true;
    CScriptArgReader argStream ( luaVM );

    if ( argStream.NextCouldBeNumber () )
    {
        argStream.ReadNumber ( timer );
        if ( timer < 0 )
        {
            argStream.SetCustomError ( "seconds cannot be negative" );
        }
    }

    if ( argStream.NextIsBool () )
        argStream.ReadBool ( bLocalTime );

    tm * time;
    if ( bLocalTime )
        time = localtime ( &timer );
    else
        time = gmtime ( &timer );

    if ( time == NULL )
        argStream.SetCustomError ( "seconds is out of range" );

    if ( argStream.HasErrors () )
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CLuaArguments ret;
    ret.PushString ( "second" );
    ret.PushNumber ( time->tm_sec );
    ret.PushString ( "minute" );
    ret.PushNumber ( time->tm_min );
    ret.PushString ( "hour" );
    ret.PushNumber ( time->tm_hour );
    ret.PushString ( "monthday" );
    ret.PushNumber ( time->tm_mday );
    ret.PushString ( "month" );
    ret.PushNumber ( time->tm_mon );
    ret.PushString ( "year" );
    ret.PushNumber ( time->tm_year );
    ret.PushString ( "weekday" );
    ret.PushNumber ( time->tm_wday );
    ret.PushString ( "yearday" );
    ret.PushNumber ( time->tm_yday );
    ret.PushString ( "isdst" );
    ret.PushNumber ( time->tm_isdst );
    ret.PushString ( "timestamp" );
    ret.PushNumber ( (double) timer );

    ret.PushAsTable ( luaVM );

    return 1;
}


int CLuaFunctionDefs::Split ( lua_State* luaVM )
{
    SString strInput = "";
    unsigned int uiDelimiter = 0;
    SString strDelimiter;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strInput );

    if ( argStream.NextIsNumber () )
    {
        argStream.ReadNumber ( uiDelimiter );
        wchar_t wUNICODE[2] = { uiDelimiter, '\0' };
        strDelimiter = UTF16ToMbUTF8 ( wUNICODE );
    }
    else  // It's already a string
        argStream.ReadString ( strDelimiter );

    if ( !argStream.HasErrors () )
    {
        // Copy the string
        char* strText = new char[strInput.length () + 1];
        strcpy ( strText, strInput );

        unsigned int uiCount = 0;
        char* szToken = strtok ( strText, strDelimiter );

        // Create a new table
        lua_newtable ( luaVM );

        // Add our first token
        lua_pushnumber ( luaVM, ++uiCount );
        lua_pushstring ( luaVM, szToken );
        lua_settable ( luaVM, -3 );

        // strtok until we're out of tokens
        while ( ( szToken = strtok ( NULL, strDelimiter ) ) )
        {
            // Add the token to the table
            lua_pushnumber ( luaVM, ++uiCount );
            lua_pushstring ( luaVM, szToken );
            lua_settable ( luaVM, -3 );
        }

        // Delete the text
        delete[] strText;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    return 1;
}

int CLuaFunctionDefs::GetTok ( lua_State* luaVM )
{
    SString strInput = "";
    unsigned int uiToken = 0;
    unsigned int uiDelimiter = 0;
    SString strDelimiter;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strInput );
    argStream.ReadNumber ( uiToken );

    if ( argStream.NextIsNumber () )
    {
        argStream.ReadNumber ( uiDelimiter );
        wchar_t wUNICODE[2] = { uiDelimiter, '\0' };
        strDelimiter = UTF16ToMbUTF8 ( wUNICODE );
    }
    else  // It's already a string
        argStream.ReadString ( strDelimiter );

    if ( !argStream.HasErrors () )
    {
        if ( uiToken > 0 && uiToken < 1024 )
        {
            unsigned int uiCount = 1;
            char* szText = new char[strInput.length () + 1];
            strcpy ( szText, strInput );
            char* szToken = strtok ( szText, strDelimiter );

            // We're looking for the first part?
            if ( uiToken != 1 )
            {
                // strtok count number of times
                do
                {
                    uiCount++;
                    szToken = strtok ( NULL, strDelimiter );
                } while ( uiCount != uiToken );
            }

            // Found it?
            if ( szToken )
            {
                // Return it
                lua_pushstring ( luaVM, szToken );
                delete[] szText;
                return 1;
            }

            // Delete the text
            delete[] szText;
        }
        else
            m_pScriptDebugging->LogWarning ( luaVM, "Token parameter sent to split must be greater than 0 and smaller than 1024" );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::toJSON ( lua_State* luaVM )
{
    // Got a string argument?
    CScriptArgReader argStream ( luaVM );

    if ( !argStream.NextIsNil () )
    {
        int jsonFlags = 0;
        // Read the argument
        CLuaArguments JSON;
        JSON.ReadArgument ( luaVM, 1 );
        argStream.Skip ( 1 );

        bool bCompact;
        argStream.ReadBool ( bCompact, false );
        jsonFlags |= bCompact ? JSON_C_TO_STRING_PLAIN : JSON_C_TO_STRING_SPACED;
        
        eJSONPrettyType jsonPrettyType;
        argStream.ReadEnumString ( jsonPrettyType, JSONPRETTY_NONE );
        if ( jsonPrettyType != JSONPRETTY_NONE )
            jsonFlags |= jsonPrettyType;

        if ( !argStream.HasErrors () )
        {
            // Convert it to a JSON string
            std::string strJSON;
            if ( JSON.WriteToJSONString ( strJSON, false, jsonFlags ) )
            {
                // Return the JSON string
                lua_pushstring ( luaVM, strJSON.c_str () );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::fromJSON ( lua_State* luaVM )
{
    SString strJSON;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strJSON );

    if ( !argStream.HasErrors () )
    {
        // Read it into lua arguments
        CLuaArguments Converted;
        if ( Converted.ReadFromJSONString ( strJSON ) )
        {
            // Return it as data
            Converted.PushArguments ( luaVM );
            return Converted.Count ();
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}
