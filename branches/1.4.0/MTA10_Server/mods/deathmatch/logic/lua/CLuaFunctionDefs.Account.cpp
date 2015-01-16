/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Account.cpp
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


int CLuaFunctionDefs::GetAccountName ( lua_State* luaVM )
{
    //  string getAccountName ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );

    if ( !argStream.HasErrors () )
    {
        std::string strName = pAccount->GetName ();
        if ( !strName.empty () )
        {
            lua_pushstring ( luaVM, strName.c_str () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetAccountPlayer ( lua_State* luaVM )
{
    //  player getAccountPlayer ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );

    if ( !argStream.HasErrors () )
    {
        CClient* pClient = CStaticFunctionDefinitions::GetAccountPlayer ( pAccount );
        if ( pClient )
        {
            lua_pushelement ( luaVM, pClient->GetElement () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsGuestAccount ( lua_State* luaVM )
{
    //  bool isGuestAccount ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );

    if ( !argStream.HasErrors () )
    {
        bool bGuest;
        if ( CStaticFunctionDefinitions::IsGuestAccount ( pAccount, bGuest ) )
        {
            lua_pushboolean ( luaVM, bGuest );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetAccountData ( lua_State* luaVM )
{
    //  string getAccountData ( account theAccount, string key )
    CAccount* pAccount; SString strKey;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors () )
    {
        CLuaArgument * pArgument = CStaticFunctionDefinitions::GetAccountData ( pAccount, strKey );
        if ( pArgument )
        {
            pArgument->Push ( luaVM );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetAllAccountData ( lua_State* luaVM )
{
    //  string getAllAccountData ( account theAccount )
    CAccount* pAccount;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );
    if ( !argStream.HasErrors () )
    {
        lua_newtable ( luaVM );
        CStaticFunctionDefinitions::GetAllAccountData ( luaVM, pAccount );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetAccount ( lua_State* luaVM )
{
    //  account getAccount ( string username, [ string password ] )
    SString strName; SString strPassword;
    bool bUsePassword = false;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );

    if ( !argStream.NextIsNil () && !argStream.NextIsNone () )
    {
        argStream.ReadString ( strPassword );
        bUsePassword = true;
    }

    if ( !argStream.HasErrors () )
    {
        CAccount* pAccount = CStaticFunctionDefinitions::GetAccount ( strName, bUsePassword ? strPassword.c_str () : NULL );
        if ( pAccount )
        {
            lua_pushaccount ( luaVM, pAccount );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetAccounts ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        lua_newtable ( luaVM );

        CStaticFunctionDefinitions::GetAccounts ( pLuaMain );

        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetAccountSerial ( lua_State* luaVM )
{
    //  string getAccountSerial ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );

    if ( !argStream.HasErrors () )
    {
        SString strSerial;
        if ( CStaticFunctionDefinitions::GetAccountSerial ( pAccount, strSerial ) )
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

int CLuaFunctionDefs::GetAccountsBySerial ( lua_State* luaVM )
{
    //  table getAccountsBySerial ( string serial )
    SString strSerial;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strSerial );

    if ( !argStream.HasErrors () )
    {
        lua_newtable ( luaVM );
        std::vector<CAccount*> accounts;

        CStaticFunctionDefinitions::GetAccountsBySerial ( strSerial, accounts );
        for ( unsigned int i = 0; i < accounts.size (); ++i )
        {
            lua_pushnumber ( luaVM, i + 1 );
            lua_pushaccount ( luaVM, accounts[i] );
            lua_settable ( luaVM, -3 );
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::AddAccount ( lua_State* luaVM )
{
    //  account addAccount ( string name, string pass )
    SString strName; SString strPassword;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadString ( strPassword );

    if ( !argStream.HasErrors () )
    {
        CAccount* pAccount;
        if ( ( pAccount = CStaticFunctionDefinitions::AddAccount ( strName, strPassword ) ) )
        {
            lua_pushaccount ( luaVM, pAccount );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemoveAccount ( lua_State* luaVM )
{
    //  bool removeAccount ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RemoveAccount ( pAccount ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }

        CClient* pClient = pAccount->GetClient ();
        if ( pClient )
            argStream.SetCustomError ( "Unable to remove account as unable to log out client. (Maybe onPlayerLogout is cancelled)" );
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetAccountPassword ( lua_State* luaVM )
{
    //  bool setAccountPassword ( account theAccount, string password )
    CAccount* pAccount; SString strPassword;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );
    argStream.ReadString ( strPassword );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetAccountPassword ( pAccount, strPassword ) )
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


int CLuaFunctionDefs::SetAccountData ( lua_State* luaVM )
{
    //  bool setAccountData ( account theAccount, string key, string value )
    CAccount* pAccount; SString strKey; CLuaArgument Variable;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );
    argStream.ReadString ( strKey );
    argStream.ReadLuaArgument ( Variable );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetAccountData ( pAccount, strKey, &Variable ) )
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


int CLuaFunctionDefs::CopyAccountData ( lua_State* luaVM )
{
    //  bool copyAccountData ( account theAccount, account fromAccount )
    CAccount* pAccount; CAccount* pFromAccount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );
    argStream.ReadUserData ( pFromAccount );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::CopyAccountData ( pAccount, pFromAccount ) )
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


int CLuaFunctionDefs::LogIn ( lua_State* luaVM )
{
    //  bool logIn ( player thePlayer, account theAccount, string thePassword )
    CPlayer* pPlayer; CAccount* pAccount; SString strPassword;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadUserData ( pAccount );
    argStream.ReadString ( strPassword );

    if ( !argStream.HasErrors () )
    {
        // Log him in
        if ( CStaticFunctionDefinitions::LogIn ( pPlayer, pAccount, strPassword ) )
        {
            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::LogOut ( lua_State* luaVM )
{
    //  bool logOut ( player thePlayer )
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        // Log out
        if ( CStaticFunctionDefinitions::LogOut ( pPlayer ) )
        {
            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}
