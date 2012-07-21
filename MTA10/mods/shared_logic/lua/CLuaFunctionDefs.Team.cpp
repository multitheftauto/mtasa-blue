/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Team.cpp
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

using std::list;

int CLuaFunctionDefs::GetTeamFromName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char* szTeamName = lua_tostring ( luaVM, 1 );
        CClientTeam* pTeam = m_pTeamManager->GetTeam ( szTeamName );
        if ( pTeam )
        {
            lua_pushelement ( luaVM, pTeam );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTeamName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientTeam* pTeam = lua_toteam ( luaVM, 1 );
        if ( pTeam )
        {
            const char* szName = pTeam->GetTeamName ();
            if ( szName )
            {
                lua_pushstring ( luaVM, szName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "team", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTeamColor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientTeam* pTeam = lua_toteam ( luaVM, 1 );
        if ( pTeam )
        {
            unsigned char ucRed, ucGreen, ucBlue;
            pTeam->GetColor ( ucRed, ucGreen, ucBlue );

            lua_pushnumber ( luaVM, ucRed );
            lua_pushnumber ( luaVM, ucGreen );
            lua_pushnumber ( luaVM, ucBlue );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "team", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTeamFriendlyFire ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientTeam* pTeam = lua_toteam ( luaVM, 1 );
        if ( pTeam )
        {
            bool bFriendlyFire = pTeam->GetFriendlyFire ();
            lua_pushboolean ( luaVM, bFriendlyFire );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "team", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPlayersInTeam ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        {
            CClientTeam* pTeam = lua_toteam ( luaVM, 1 );
            if ( pTeam )
            {
                lua_newtable ( luaVM );

                unsigned int uiIndex = 0;

                list < CClientPlayer* > ::const_iterator iter = pTeam->IterBegin ();
                for ( ; iter != pTeam->IterEnd () ; iter++ )
                {
                    CClientPlayer* pPlayer = *iter;
                    lua_pushnumber ( luaVM, ++uiIndex );
                    lua_pushelement ( luaVM, pPlayer );
                    lua_settable ( luaVM, -3 );
                }

                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "team", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CountPlayersInTeam ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientTeam* pTeam = lua_toteam ( luaVM, 1 );
        if ( pTeam )
        {
            unsigned int uiCount = pTeam->CountPlayers ();
            lua_pushnumber ( luaVM, uiCount );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "team", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


