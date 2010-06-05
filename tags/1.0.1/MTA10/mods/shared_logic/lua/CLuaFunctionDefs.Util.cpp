/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Util.cpp
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

int CLuaFunctionDefs::GetTok ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) != LUA_TSTRING ) || ( lua_type ( luaVM, 2 ) != LUA_TNUMBER ) || ( lua_type ( luaVM, 3 ) != LUA_TNUMBER ) )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "gettok" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }

    const char* szText = lua_tostring ( luaVM, 1 );
    int iToken = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );
    int iDelimiter = static_cast < int > ( lua_tonumber ( luaVM, 3 ) );

    unsigned int uiCount = 0;

    if ( iToken > 0 && iToken < 1024 )
    {
        // Copy the string
        char* strText = new char [ strlen ( szText ) + 1 ];
        strcpy ( strText, szText );

        SString strDelimiter ( "%c", iDelimiter );

        unsigned int uiCount = 1;
        char* szToken = strtok ( strText, strDelimiter );

        // We're looking for the first part?
        if ( iToken != 1 )
        {
            // strtok count number of times
            do
            {
                uiCount++;
                szToken = strtok ( NULL, strDelimiter );
            }
            while ( uiCount != iToken );
        }

        // Found it?
        if ( szToken )
        {
            // Return it
            lua_pushstring ( luaVM, szToken );
            delete [] strText;
            return 1;
        }

        // Delete the text
        delete [] strText;
    }
    else
        m_pScriptDebugging->LogWarning ( luaVM, "Token parameter sent to split must be greater than 0 and smaller than 1024" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::Split ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) != LUA_TSTRING ) || ( lua_type ( luaVM, 2 ) != LUA_TNUMBER ) )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "split" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }

    const char* szText = lua_tostring ( luaVM, 1 );
    int iDelimiter = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );

    // Copy the string
    char* strText = new char [ strlen ( szText ) + 1 ];
    strcpy ( strText, szText );

    SString strDelimiter ( "%c", iDelimiter );

    unsigned int uiCount = 0;
    char* szToken = strtok ( strText, strDelimiter );

    // Create a new table
    lua_newtable ( luaVM );

    // Add our first token
    lua_pushnumber ( luaVM, ++uiCount );
    lua_pushstring ( luaVM, szToken );
    lua_settable ( luaVM, -3 );

    // strtok until we're out of tokens
    while ( szToken = strtok ( NULL, strDelimiter ) )
    {
        // Add the token to the table
        lua_pushnumber ( luaVM, ++uiCount );
        lua_pushstring ( luaVM, szToken );
        lua_settable ( luaVM, -3 );
    }

    // Delete the text
    delete [] strText;

    return 1;
}


int CLuaFunctionDefs::SetTimer ( lua_State* luaVM )
{
    CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TFUNCTION )
        {
            CLuaTimer* pLuaTimer = luaMain->GetTimerManager ()->AddTimer ( luaVM );
            if ( pLuaTimer )
            {
                lua_pushtimer ( luaVM, pLuaTimer );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "setTimer" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::KillTimer ( lua_State* luaVM )
{
    CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        CLuaTimer* pLuaTimer = lua_totimer ( luaVM, 1 );
        if ( pLuaTimer )
        {
            luaMain->GetTimerManager ()->RemoveTimer ( pLuaTimer );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "killTimer" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTimers ( lua_State* luaVM )
{
    // Find our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        unsigned long ulTime = 0;
        int iArgument1 = lua_type ( luaVM, 1 );
        if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
        {
            ulTime = static_cast < unsigned long > ( lua_tonumber ( luaVM, 1 ) );
        }            
        // Create a new table
        lua_newtable ( luaVM );

        // Add all the timers with less than ulTime left
        pLuaMain->GetTimerManager ()->GetTimers ( ulTime, pLuaMain );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsTimer ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        CLuaTimer* pLuaTimer = lua_totimer ( luaVM, 1 );
        if ( pLuaTimer )
        {
            lua_pushboolean ( luaVM, pLuaMain->GetTimerManager ()->Exists ( pLuaTimer ) );
            return 1;
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTickCount_ ( lua_State* luaVM )
{
    double dCount;
    if ( CStaticFunctionDefinitions::GetTickCount_ ( dCount ) )
    {
        lua_pushnumber ( luaVM, dCount );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetCTime ( lua_State* luaVM )
{
    time_t timer;
    time ( &timer );
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER || lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        timer = ( time_t ) lua_tonumber ( luaVM, 1 );
    }
    tm * time = localtime ( &timer );

    CLuaArguments ret;
    ret.PushString("second");
    ret.PushNumber(time->tm_sec);
    ret.PushString("minute");
    ret.PushNumber(time->tm_min);
    ret.PushString("hour");
    ret.PushNumber(time->tm_hour);
    ret.PushString("monthday");
    ret.PushNumber(time->tm_mday);
    ret.PushString("month");
    ret.PushNumber(time->tm_mon);
    ret.PushString("year");
    ret.PushNumber(time->tm_year);
    ret.PushString("weekday");
    ret.PushNumber(time->tm_wday);
    ret.PushString("yearday");
    ret.PushNumber(time->tm_yday);
    ret.PushString("isdst");
    ret.PushNumber(time->tm_isdst);

    ret.PushAsTable(luaVM);

    return 1;
}
int CLuaFunctionDefs::tocolor ( lua_State* luaVM )
{
    // Grab argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );

    // Got first argument?
    if ( iArgument1 == LUA_TNUMBER )
    {
        // Got second and third? (R,G,B)
        if ( iArgument2 == LUA_TNUMBER &&
            iArgument3 == LUA_TNUMBER )
        {
            // Read red, green and blue
            unsigned char ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
            unsigned char ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            unsigned char ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );

            // Got a fourth too? (R,G,B,A) Defaults to 255.
            unsigned char ucAlpha = 255;
            if ( iArgument4 == LUA_TNUMBER )
            {
                ucAlpha = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
            }

            // Make it into an unsigned long
            unsigned long ulColor = COLOR_RGBA ( ucRed, ucGreen, ucBlue, ucAlpha );
            lua_pushinteger ( luaVM, static_cast < lua_Integer > ( ulColor ) );
            return 1;
        }
    }

    // Make it black so funcs dont break
    unsigned long ulColor = COLOR_RGBA ( 0, 0, 0, 255 );
    lua_pushnumber ( luaVM, static_cast < lua_Number > ( ulColor ) );
    return 1;
}

int CLuaFunctionDefs::Reference ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TTABLE || iArgument1 == LUA_TFUNCTION ||
        iArgument1 == LUA_TUSERDATA || iArgument1 == LUA_TTHREAD ||
        iArgument1 == LUA_TLIGHTUSERDATA )
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
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        int iPointer = static_cast < int > ( lua_tonumber ( luaVM, 1 ) );
        lua_getref ( luaVM, iPointer );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetColorFromString ( lua_State* luaVM )
{
    unsigned char ucColorRed, ucColorGreen, ucColorBlue, ucColorAlpha;
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TSTRING )
    {
        const char* szColor = lua_tostring ( luaVM, 1 );

        if ( XMLColorToInt ( szColor, ucColorRed, ucColorGreen, ucColorBlue, ucColorAlpha ) )
        {
            lua_pushnumber ( luaVM, ucColorRed );
            lua_pushnumber ( luaVM, ucColorGreen );
            lua_pushnumber ( luaVM, ucColorBlue );
            lua_pushnumber ( luaVM, ucColorAlpha );
            return 4;
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetDistanceBetweenPoints2D ( lua_State* luaVM )
{
    // We got 6 valid float arguments?
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Put them into two vectors
        CVector vecPointA ( static_cast < float > ( atof ( lua_tostring ( luaVM, 1 ) ) ), static_cast < float > ( atof ( lua_tostring ( luaVM, 2 ) ) ), 0.0f );
        CVector vecPointB ( static_cast < float > ( atof ( lua_tostring ( luaVM, 3 ) ) ), static_cast < float > ( atof ( lua_tostring ( luaVM, 4 ) ) ), 0.0f );

        // Return the distance
        lua_pushnumber ( luaVM, DistanceBetweenPoints2D ( vecPointA, vecPointB ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getDistanceBetweenPoints2D" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetDistanceBetweenPoints3D ( lua_State* luaVM )
{
    // We got 6 valid float arguments?
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
        ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
    {
        // Put them into two vectors
        CVector vecPointA ( static_cast < float > ( atof ( lua_tostring ( luaVM, 1 ) ) ), static_cast < float > ( atof ( lua_tostring ( luaVM, 2 ) ) ), static_cast < float > ( atof ( lua_tostring ( luaVM, 3 ) ) ) );
        CVector vecPointB ( static_cast < float > ( atof ( lua_tostring ( luaVM, 4 ) ) ), static_cast < float > ( atof ( lua_tostring ( luaVM, 5 ) ) ), static_cast < float > ( atof ( lua_tostring ( luaVM, 6 ) ) ) );

        // Return the distance
        lua_pushnumber ( luaVM, DistanceBetweenPoints3D ( vecPointA, vecPointB ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getDistanceBetweenPoints3D" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::Md5 ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING || lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        MD5 md5bytes;
        char szResult[33];
        CMD5Hasher hasher;
        hasher.Calculate ( lua_tostring ( luaVM, 1 ), lua_objlen ( luaVM, 1 ), md5bytes );
        hasher.ConvertToHex ( md5bytes, szResult );
        lua_pushstring ( luaVM, szResult );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogBadType ( luaVM, "md5" );
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetPacketInfo ( lua_State* luaVM )
{
    unsigned long ulBits [ 256 ];
    unsigned long ulCount [ 256 ];
    g_pNet->GetPacketLogData ( ulBits, ulCount );
    lua_createtable ( luaVM, 256, 1 );

    for ( unsigned int i = 0; i < 256; ++i )
    {
        lua_createtable ( luaVM, 0, 2 );

        lua_pushstring ( luaVM, "bits" );
        lua_pushnumber ( luaVM, ulBits [ i ] );
        lua_rawset ( luaVM, -3 );

        lua_pushstring ( luaVM, "count" );
        lua_pushnumber ( luaVM, ulCount [ i ] );
        lua_rawset ( luaVM, -3 );

        lua_rawseti ( luaVM, -2, i );
    }

    return 1;
}


int CLuaFunctionDefs::GetVersion ( lua_State* luaVM )
{
    lua_createtable ( luaVM, 0, 6 );

    lua_pushstring ( luaVM, "number" );
    lua_pushnumber ( luaVM, CStaticFunctionDefinitions::GetVersion () );
    lua_settable   ( luaVM, -3 );

    lua_pushstring ( luaVM, "mta" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetVersionString () );
    lua_settable   ( luaVM, -3 );

    lua_pushstring ( luaVM, "name" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetVersionName () );
    lua_settable   ( luaVM, -3 );

    lua_pushstring ( luaVM, "netcode" );
    lua_pushnumber ( luaVM, CStaticFunctionDefinitions::GetNetcodeVersion () );
    lua_settable   ( luaVM, -3 );

    lua_pushstring ( luaVM, "os" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetOperatingSystemName () );
    lua_settable   ( luaVM, -3 );

    lua_pushstring ( luaVM, "type" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetVersionBuildType () );
    lua_settable   ( luaVM, -3 );

    return 1;
}
