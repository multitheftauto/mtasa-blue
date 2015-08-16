/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Team.cpp
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


int CLuaFunctionDefs::SetPlayerTeam ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    CTeam* pTeam;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadUserData ( pTeam, NULL );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerTeam ( pPlayer, pTeam ) )
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

int CLuaFunctionDefs::CreateTeam ( lua_State* luaVM )
{
    SString strName;
    unsigned char ucRed;
    unsigned char ucGreen;
    unsigned char ucBlue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadNumber ( ucRed, 235 );
    argStream.ReadNumber ( ucGreen, 221 );
    argStream.ReadNumber ( ucBlue, 178 );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                CTeam* pTeam = CStaticFunctionDefinitions::CreateTeam ( pResource, strName, ucRed, ucGreen, ucBlue );
                if ( pTeam )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup ();
                    if ( pGroup )
                    {
                        pGroup->Add ( pTeam );
                    }
                    lua_pushelement ( luaVM, pTeam );
                    return 1;
                }
            }
        }


    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPlayersInTeam ( lua_State* luaVM )
{
    CTeam *pTeam;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTeam );

    if ( !argStream.HasErrors () )
    {
        lua_newtable ( luaVM );

        pTeam->GetPlayers ( luaVM );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTeamFromName ( lua_State* luaVM )
{
    SString strName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );

    if ( !argStream.HasErrors () )
    {
        CTeam* pTeam = CStaticFunctionDefinitions::GetTeamFromName ( strName );
        if ( pTeam )
        {
            lua_pushelement ( luaVM, pTeam );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetTeamName ( lua_State* luaVM )
{
    CTeam *pTeam;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTeam );

    if ( !argStream.HasErrors () )
    {
        SString strTeamName;
        if ( CStaticFunctionDefinitions::GetTeamName ( pTeam, strTeamName ) )
        {
            lua_pushstring ( luaVM, strTeamName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetTeamColor ( lua_State* luaVM )
{
    CTeam *pTeam;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTeam );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucRed, ucGreen, ucBlue;
        if ( CStaticFunctionDefinitions::GetTeamColor ( pTeam, ucRed, ucGreen, ucBlue ) )
        {
            lua_pushnumber ( luaVM, ucRed );
            lua_pushnumber ( luaVM, ucGreen );
            lua_pushnumber ( luaVM, ucBlue );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTeamFriendlyFire ( lua_State* luaVM )
{
    CTeam *pTeam;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTeam );

    if ( !argStream.HasErrors () )
    {
        bool bFriendlyFire;
        if ( CStaticFunctionDefinitions::GetTeamFriendlyFire ( pTeam, bFriendlyFire ) )
        {
            lua_pushboolean ( luaVM, bFriendlyFire );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefs::CountPlayersInTeam ( lua_State* luaVM )
{
    CTeam *pTeam;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTeam );

    if ( !argStream.HasErrors () )
    {
        unsigned int uiCount;
        if ( CStaticFunctionDefinitions::CountPlayersInTeam ( pTeam, uiCount ) )
        {
            lua_pushnumber ( luaVM, uiCount );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetTeamName ( lua_State* luaVM )
{
    CTeam* pElement;
    SString strName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strName );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTeamName ( pElement, strName ) )
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


int CLuaFunctionDefs::SetTeamColor ( lua_State* luaVM )
{
    CTeam* pElement;
    unsigned char ucRed;
    unsigned char ucGreen;
    unsigned char ucBlue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucRed );
    argStream.ReadNumber ( ucGreen );
    argStream.ReadNumber ( ucBlue );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTeamColor ( pElement, ucRed, ucGreen, ucBlue ) )
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


int CLuaFunctionDefs::SetTeamFriendlyFire ( lua_State* luaVM )
{
    CTeam* pElement;
    bool bFriendlyFire;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bFriendlyFire );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTeamFriendlyFire ( pElement, bFriendlyFire ) )
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
