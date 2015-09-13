/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaBanDefs.cpp
*  PURPOSE:     Lua function definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaBanDefs::LoadFunctions ()
{
    CLuaCFunctions::AddFunction ( "addBan", AddBan );
    CLuaCFunctions::AddFunction ( "removeBan", RemoveBan );

    CLuaCFunctions::AddFunction ( "getBans", GetBans );
    CLuaCFunctions::AddFunction ( "reloadBans", ReloadBanList );

    CLuaCFunctions::AddFunction ( "getBanIP", GetBanIP );
    CLuaCFunctions::AddFunction ( "getBanSerial", GetBanSerial );
    CLuaCFunctions::AddFunction ( "getBanUsername", GetBanUsername );
    CLuaCFunctions::AddFunction ( "getBanNick", GetBanNick );
    CLuaCFunctions::AddFunction ( "getBanTime", GetBanTime );
    CLuaCFunctions::AddFunction ( "getUnbanTime", GetUnbanTime );
    CLuaCFunctions::AddFunction ( "getBanReason", GetBanReason );
    CLuaCFunctions::AddFunction ( "getBanAdmin", GetBanAdmin );

    CLuaCFunctions::AddFunction ( "setUnbanTime", SetUnbanTime );
    CLuaCFunctions::AddFunction ( "setBanReason", SetBanReason );
    CLuaCFunctions::AddFunction ( "setBanAdmin", SetBanAdmin );
    CLuaCFunctions::AddFunction ( "setBanNick", SetBanNick );
    CLuaCFunctions::AddFunction ( "isBan", IsBan );

}

// TODO: The "set" attribute of .admin, .reason and .unbanTime needs to be checked for syntax
void CLuaBanDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "addBan" );
    lua_classfunction ( luaVM, "remove", "removeBan" );
    lua_classfunction ( luaVM, "getList", "getBans" );
    lua_classvariable ( luaVM, "list", NULL, "getBans" );

    lua_classfunction ( luaVM, "getAdmin", "getBanAdmin" );
    lua_classfunction ( luaVM, "getIP", "getBanIP" );
    lua_classfunction ( luaVM, "getNick", "getBanNick" );
    lua_classfunction ( luaVM, "getReason", "getBanReason" );
    lua_classfunction ( luaVM, "getSerial", "getBanSerial" );
    lua_classfunction ( luaVM, "getTime", "getBanTime" );
    lua_classfunction ( luaVM, "getUnbanTime", "getUnbanTime" );

    lua_classvariable ( luaVM, "admin", "setBanAdmin", "getBanAdmin" );
    lua_classvariable ( luaVM, "IP", NULL, "getBanIP" );
    lua_classvariable ( luaVM, "nick", NULL, "getBanNick" );
    lua_classvariable ( luaVM, "serial", NULL, "getBanSerial" );
    lua_classvariable ( luaVM, "time", NULL, "getBanTime" );
    lua_classvariable ( luaVM, "unbanTime", NULL, "getUnbanTime" );
    lua_classvariable ( luaVM, "reason", "setBanReason", "getBanReason" );
    lua_classvariable ( luaVM, "nick", "setBanNick", "getBanNick" );

    lua_registerclass ( luaVM, "Ban" );
}


int CLuaBanDefs::AddBan ( lua_State* luaVM )
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


int CLuaBanDefs::RemoveBan ( lua_State* luaVM )
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


int CLuaBanDefs::GetBans ( lua_State* luaVM )
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


int CLuaBanDefs::ReloadBanList ( lua_State* luaVM )
{
    bool bSuccess = CStaticFunctionDefinitions::ReloadBanList ();
    if ( !bSuccess )
        m_pScriptDebugging->LogError ( luaVM, "%s: Ban List failed to reload, fix any errors and run again", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
    lua_pushboolean ( luaVM, bSuccess );
    return 1;
}


int CLuaBanDefs::GetBanIP ( lua_State* luaVM )
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

int CLuaBanDefs::GetBanSerial ( lua_State* luaVM )
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


int CLuaBanDefs::GetBanUsername ( lua_State* luaVM )
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



int CLuaBanDefs::GetBanNick ( lua_State* luaVM )
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


int CLuaBanDefs::GetBanTime ( lua_State* luaVM )
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

int CLuaBanDefs::GetUnbanTime ( lua_State* luaVM )
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


int CLuaBanDefs::GetBanReason ( lua_State* luaVM )
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


int CLuaBanDefs::GetBanAdmin ( lua_State* luaVM )
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

int CLuaBanDefs::SetUnbanTime ( lua_State* luaVM )
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

int CLuaBanDefs::SetBanReason ( lua_State* luaVM )
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

int CLuaBanDefs::SetBanAdmin ( lua_State* luaVM )
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

int CLuaBanDefs::SetBanNick ( lua_State* luaVM )
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

int CLuaBanDefs::IsBan ( lua_State* luaVM )
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
