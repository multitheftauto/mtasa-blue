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
*  NOTES: This was going to be in .Server.cpp but we have the Ban entity
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::AddBan ( lua_State* luaVM )
{
    //  ban addBan ( [ string IP, string Username, string Serial, player responsibleElement, string reason, int seconds = 0 ] )
    SString strIP = "";
    SString strUsername = "";
    SString strSerial = "";
    SString strResponsible = "Console";
    CPlayer * pResponsible = NULL;
    SString strReason = "";
    time_t tUnban;


    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strIP, "" );
    argStream.ReadString ( strUsername, "" );
    argStream.ReadString ( strSerial, "" );
    if ( argStream.NextIsUserData () )
    {
        CElement* pResponsibleElement;
        argStream.ReadUserData ( pResponsibleElement );
        if ( ( pResponsible = dynamic_cast <CPlayer*> ( pResponsibleElement ) ) )
            strResponsible = pResponsible->GetNick ();
    }
    else
        argStream.ReadString ( strResponsible, "Console" );

    argStream.ReadString ( strReason, "" );

    if ( argStream.NextIsString () )
    {
        SString strTime;
        argStream.ReadString ( strTime );
        tUnban = atoi ( strTime );
    }
    else
        if ( argStream.NextIsNumber () )
            argStream.ReadNumber ( tUnban );
        else
            tUnban = 0;

    if ( tUnban > 0 )
        tUnban += time ( NULL );

    if ( !argStream.HasErrors () )
    {
        CBan* pBan = NULL;
        if ( ( pBan = CStaticFunctionDefinitions::AddBan ( strIP, strUsername, strSerial, pResponsible, strResponsible, strReason, tUnban ) ) )
        {
            lua_pushban ( luaVM, pBan );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemoveBan ( lua_State* luaVM )
{
    CBan* pBan;
    CPlayer* pResponsible = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );

    if ( argStream.NextIsUserData () )
    {
        CElement* pResponsibleElement;
        argStream.ReadUserData ( pResponsibleElement );
        pResponsible = dynamic_cast <CPlayer*> ( pResponsibleElement );
    }

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RemoveBan ( pBan, pResponsible ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBans ( lua_State* luaVM )
{
    // Grab its lua
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        lua_newtable ( luaVM );

        CStaticFunctionDefinitions::GetBans ( luaVM );

        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ReloadBanList ( lua_State* luaVM )
{
    bool bSuccess = CStaticFunctionDefinitions::ReloadBanList ();
    if ( !bSuccess )
        m_pScriptDebugging->LogError ( luaVM, "%s: Ban List failed to reload, fix any errors and run again", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
    lua_pushboolean ( luaVM, bSuccess );
    return 1;
}


int CLuaFunctionDefs::GetBanIP ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );

    if ( !argStream.HasErrors () )
    {
        SString strIP;
        if ( CStaticFunctionDefinitions::GetBanIP ( pBan, strIP ) )
        {
            lua_pushstring ( luaVM, strIP );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetBanSerial ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );

    if ( !argStream.HasErrors () )
    {
        SString strSerial;
        if ( CStaticFunctionDefinitions::GetBanSerial ( pBan, strSerial ) )
        {
            lua_pushstring ( luaVM, strSerial );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBanUsername ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );

    if ( !argStream.HasErrors () )
    {
        SString strUser;
        if ( CStaticFunctionDefinitions::GetBanUsername ( pBan, strUser ) )
        {
            lua_pushstring ( luaVM, strUser );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefs::GetBanNick ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );

    if ( !argStream.HasErrors () )
    {
        SString strNick;
        if ( CStaticFunctionDefinitions::GetBanNick ( pBan, strNick ) )
        {
            lua_pushstring ( luaVM, strNick );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBanTime ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );

    if ( !argStream.HasErrors () )
    {
        time_t tTime;
        if ( CStaticFunctionDefinitions::GetBanTime ( pBan, tTime ) )
        {
            lua_pushnumber ( luaVM, (double) tTime );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetUnbanTime ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );

    if ( !argStream.HasErrors () )
    {
        time_t tTime;
        if ( CStaticFunctionDefinitions::GetUnbanTime ( pBan, tTime ) )
        {
            lua_pushnumber ( luaVM, (double) tTime );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBanReason ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );

    if ( !argStream.HasErrors () )
    {
        SString strReason;
        if ( CStaticFunctionDefinitions::GetBanReason ( pBan, strReason ) )
        {
            lua_pushstring ( luaVM, strReason );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBanAdmin ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );

    if ( !argStream.HasErrors () )
    {
        SString strAdmin;
        if ( CStaticFunctionDefinitions::GetBanAdmin ( pBan, strAdmin ) )
        {
            lua_pushstring ( luaVM, strAdmin );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetUnbanTime ( lua_State* luaVM )
{
    CBan* pBan;
    time_t tUnbanTime;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );
    argStream.ReadNumber ( tUnbanTime );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetUnbanTime ( pBan, tUnbanTime ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetBanReason ( lua_State* luaVM )
{
    CBan* pBan;
    SString strReason;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );
    argStream.ReadString ( strReason );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetBanReason ( pBan, strReason ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetBanAdmin ( lua_State* luaVM )
{
    CBan* pBan;
    SString strAdminName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );
    argStream.ReadString ( strAdminName );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetBanAdmin ( pBan, strAdminName ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetBanNick ( lua_State* luaVM )
{
    CBan* pBan;
    SString strNick;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );
    argStream.ReadString ( strNick );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetBanNick ( pBan, strNick ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsBan ( lua_State* luaVM )
{
    CBan* pBan;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBan );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}
