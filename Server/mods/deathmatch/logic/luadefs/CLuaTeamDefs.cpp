/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaTeamDefs.cpp
*  PURPOSE:     Lua function definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaTeamDefs::LoadFunctions ()
{
    // Team create/destroy functions
    CLuaCFunctions::AddFunction ( "createTeam", CreateTeam );

    // Team get funcs
    CLuaCFunctions::AddFunction ( "getTeamFromName", GetTeamFromName );
    CLuaCFunctions::AddFunction ( "getTeamName", GetTeamName );
    CLuaCFunctions::AddFunction ( "getTeamColor", GetTeamColor );
    CLuaCFunctions::AddFunction ( "getTeamFriendlyFire", GetTeamFriendlyFire );
    CLuaCFunctions::AddFunction ( "getPlayersInTeam", GetPlayersInTeam );
    CLuaCFunctions::AddFunction ( "countPlayersInTeam", CountPlayersInTeam );

    // Team set funcs
    CLuaCFunctions::AddFunction ( "setPlayerTeam", SetPlayerTeam );
    CLuaCFunctions::AddFunction ( "setTeamName", SetTeamName );
    CLuaCFunctions::AddFunction ( "setTeamColor", SetTeamColor );
    CLuaCFunctions::AddFunction ( "setTeamFriendlyFire", SetTeamFriendlyFire );
}


void CLuaTeamDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createTeam" );
    lua_classfunction ( luaVM, "getFromName", "getTeamFromName" );
    lua_classfunction ( luaVM, "countPlayers", "countPlayersInTeam" );
    lua_classfunction ( luaVM, "getPlayers", "getPlayersInTeam" );
    lua_classfunction ( luaVM, "outputChat", "outputChatBox", CLuaFunctionDefs::OOP_OutputChatBox );

    lua_classfunction ( luaVM, "getFriendlyFire", "getTeamFriendlyFire" );
    lua_classfunction ( luaVM, "getName", "getTeamName" );
    lua_classfunction ( luaVM, "getColor", "getTeamColor" );

    lua_classfunction ( luaVM, "setName", "setTeamName" );
    lua_classfunction ( luaVM, "setColor", "setTeamColor" );
    lua_classfunction ( luaVM, "setFriendlyFire", "setTeamFriendlyFire" );

    lua_classvariable ( luaVM, "playerCount", NULL, "countPlayersInTeam" );
    lua_classvariable ( luaVM, "friendlyFire", "setTeamFriendlyFire", "getTeamFriendlyFire" );
    lua_classvariable ( luaVM, "players", NULL, "getPlayersInTeam" ); // todo: perhaps table.insert/nilvaluing?
    lua_classvariable ( luaVM, "name", "setTeamName", "getTeamName" );
    //lua_classvariable ( luaVM, "color", "setTeamColor", "getTeamColor" );

    lua_registerclass ( luaVM, "Team", "Element" );
}

int CLuaTeamDefs::SetPlayerTeam ( lua_State* luaVM )
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


int CLuaTeamDefs::CreateTeam ( lua_State* luaVM )
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

int CLuaTeamDefs::GetPlayersInTeam ( lua_State* luaVM )
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


int CLuaTeamDefs::GetTeamFromName ( lua_State* luaVM )
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

int CLuaTeamDefs::GetTeamName ( lua_State* luaVM )
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

int CLuaTeamDefs::GetTeamColor ( lua_State* luaVM )
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


int CLuaTeamDefs::GetTeamFriendlyFire ( lua_State* luaVM )
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



int CLuaTeamDefs::CountPlayersInTeam ( lua_State* luaVM )
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

int CLuaTeamDefs::SetTeamName ( lua_State* luaVM )
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


int CLuaTeamDefs::SetTeamColor ( lua_State* luaVM )
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


int CLuaTeamDefs::SetTeamFriendlyFire ( lua_State* luaVM )
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
