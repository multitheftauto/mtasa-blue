/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Player.cpp
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

int CLuaFunctionDefs::GetLocalPlayer ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = CStaticFunctionDefinitions::GetLocalPlayer ();

    // Return the local player
    lua_pushelement ( luaVM, pPlayer );
    return 1;
}


int CLuaFunctionDefs::GetPlayerName ( lua_State* luaVM )
{
    // Right type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the player
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            // Grab his nametag text and return it
            const char* szName = pPlayer->GetNickPointer ();
            if ( szName )
            {
                lua_pushstring ( luaVM, szName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerName", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerName" );

    // Error
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetPlayerFromName ( lua_State* luaVM )
{
    // Valid player nick argument?
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
    {
        // Grab the nick
        const char* szNick = lua_tostring ( luaVM, 1 );

        // Grab the player with that nick
        CClientPlayer* pPlayer = CStaticFunctionDefinitions::GetPlayerFromName ( szNick );
        if ( pPlayer )
        {
            // Return the player
            lua_pushelement ( luaVM, pPlayer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerFromName" );

    // Doesn't exist
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetPlayerNametagText ( lua_State* luaVM )
{
    // Check the type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the player
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            // Grab his nametag text and return it
            const char* szName = pPlayer->GetNametagText ();
            if ( szName )
            {
                lua_pushstring ( luaVM, szName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerNametagText", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerNametagText" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPlayerNametagColor ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the player
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            // Grab his nametag color and return it
            unsigned char ucR, ucG, ucB;
            pPlayer->GetNametagColor ( ucR, ucG, ucB );

            lua_pushnumber ( luaVM, ucR );
            lua_pushnumber ( luaVM, ucG );
            lua_pushnumber ( luaVM, ucB );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerNametagColor", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerNametagColor" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsPlayerNametagShowing ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) )
    {
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            bool bIsNametagShowing = pPlayer->IsNametagShowing ();
            lua_pushboolean ( luaVM, bIsNametagShowing );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPlayerNametagShowing", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPlayerNametagShowing" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPlayerPing ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the player
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            // Grab his ping
            unsigned int uiPing = pPlayer->GetPing ();
            lua_pushnumber ( luaVM, uiPing );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerPing", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerPing" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPlayerTeam ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the player
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            // Grab his team and return it
            CClientTeam* pTeam = pPlayer->GetTeam ();
            if ( pTeam )
            {
                lua_pushelement ( luaVM, pTeam );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerTeam", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerTeam" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPlayerDead ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            // Grab his dead state and return it
            bool bDead = pPlayer->IsDead ();
            lua_pushboolean ( luaVM, bDead );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPlayerDead", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPlayerDead" );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}

int CLuaFunctionDefs::GetPlayerMoney ( lua_State* luaVM )
{
    long lMoney;
    if ( CStaticFunctionDefinitions::GetPlayerMoney ( lMoney ) )
    {
        lua_pushnumber ( luaVM, lMoney );
        return 1;
    }            

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetPlayerWantedLevel ( lua_State* luaVM )
{
    char cWanted;
    if ( CStaticFunctionDefinitions::GetPlayerWantedLevel ( cWanted ) )
    {
        lua_pushnumber ( luaVM, cWanted );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ShowPlayerHudComponent ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) &&
        lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        // Hud components (TODO: Move somewhere else)
        struct SHudComponent { char szName [ 15 ]; };
        static SHudComponent hudComponents [] = { { "ammo" }, { "weapon" }, { "health" },
        { "breath" }, { "armour" }, { "money" },
        { "vehicle_name" }, { "area_name" }, { "radar" },
        { "clock" }, { NULL } };

        // Grab the component name and the bool deciding whether to show it or not
        const char* szComponent = lua_tostring ( luaVM, 1 );
        bool bShow = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        // Any component specified?
        if ( szComponent && szComponent [ 0 ] )
        {
            // Loop through the components and find the correct one (matching name)
            unsigned char ucComponent = 0xFF;
            for ( int i = 0 ; hudComponents [ i ].szName [ 0 ] != NULL ; i++ )
            {
                if ( !stricmp ( szComponent, hudComponents [ i ].szName ) )
                {
                    ucComponent = static_cast < unsigned char > ( i );
                    break;
                }
            }

            // Found it?
            if ( ucComponent != 0xFF )
            {
                // Do it
                if ( CStaticFunctionDefinitions::ShowPlayerHudComponent ( ucComponent, bShow ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "showPlayerHudComponent", "component", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "showPlayerHudComponent" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPlayerMoney ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        long lMoney = static_cast < long > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetPlayerMoney ( lMoney ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }        
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerMoney" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GivePlayerMoney ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        long lMoney = static_cast < long > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::GivePlayerMoney ( lMoney ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }        
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "givelayerMoney" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::TakePlayerMoney ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        long lMoney = static_cast < long > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::TakePlayerMoney ( lMoney ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }        
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "takePlayerMoney" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPlayerNametagText ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TSTRING ) )
    {
        CClientPlayer * pPlayer = lua_toplayer ( luaVM, 1 );

        const char * szText = lua_tostring ( luaVM, 2 );
        if ( pPlayer && CStaticFunctionDefinitions::SetPlayerNametagText ( *pPlayer, const_cast < char * > ( szText ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerNametagText" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPlayerNametagColor ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        CClientPlayer * pPlayer = lua_toplayer ( luaVM, 1 );
        unsigned char ucR = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        unsigned char ucG = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
        unsigned char ucB = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
        if ( CStaticFunctionDefinitions::SetPlayerNametagColor ( *pPlayer, ucR, ucG, ucB ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerNametagColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetPlayerNametagShowing ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        // Grab the entity
        CClientPlayer * pPlayer = lua_toplayer ( luaVM, 1 );
        bool bShowing = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        // Valid pPlayer?
        if ( pPlayer )
        {
            // Set the new rotation
            if ( CStaticFunctionDefinitions::SetPlayerNametagShowing ( *pPlayer, bShowing ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerNametagShowing", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerNametagShowing" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

// Community

int CLuaFunctionDefs::GetPlayerUserName ( lua_State* luaVM )
{
    std::string strUser;

    g_pCore->GetCommunity ()->GetUsername ( strUser );
    if ( !strUser.empty () )
    {
        lua_pushstring ( luaVM, strUser.c_str () );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

// Player Map

int CLuaFunctionDefs::IsPlayerMapForced ( lua_State* luaVM )
{    
    bool bForced;
    if ( CStaticFunctionDefinitions::IsPlayerMapForced ( bForced ) )
    {
        lua_pushboolean ( luaVM, bForced );
        return 1;
    }    

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPlayerMapVisible ( lua_State* luaVM )
{    
    bool bForced;
    if ( CStaticFunctionDefinitions::IsPlayerMapVisible ( bForced ) )
    {
        lua_pushboolean ( luaVM, bForced );
        return 1;
    }    

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetPlayerMapBoundingBox ( lua_State* luaVM )
{    
    // Grab the bounding box and return it
    CVector vecMin, vecMax;
    if ( CStaticFunctionDefinitions::GetPlayerMapBoundingBox ( vecMin, vecMax ) )
    {
        lua_pushnumber ( luaVM, vecMin.fX );
        lua_pushnumber ( luaVM, vecMin.fY );
        lua_pushnumber ( luaVM, vecMax.fX );
        lua_pushnumber ( luaVM, vecMax.fY );
        return 4;
    }
    //The map is invisible
    lua_pushboolean ( luaVM, false );
    return 1;
}

