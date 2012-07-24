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
*               Sebas Lamers <sebasdevelopment@gmx.com>
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
//  string getPlayerName ( player thePlayer )
    CClientPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        // Grab his nametag text and return it
        const char* szName = pPlayer->GetNick ();
        if ( szName )
        {
            lua_pushstring ( luaVM, szName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    // Error
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetPlayerFromName ( lua_State* luaVM )
{
//  player getPlayerFromName ( string playerName )
    SString strNick;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strNick );

    if ( !argStream.HasErrors () )
    {
        // Grab the player with that nick
        CClientPlayer* pPlayer = CStaticFunctionDefinitions::GetPlayerFromName ( strNick );
        if ( pPlayer )
        {
            // Return the player
            lua_pushelement ( luaVM, pPlayer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    // Doesn't exist
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetPlayerNametagText ( lua_State* luaVM )
{
//  string getPlayerNametagText ( player thePlayer )
    CClientPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
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
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPlayerNametagColor ( lua_State* luaVM )
{
//  int, int, int getPlayerNametagColor ( player thePlayer )
    CClientPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
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
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsPlayerNametagShowing ( lua_State* luaVM )
{
//  bool isPlayerNametagShowing ( player thePlayer )
    CClientPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        bool bIsNametagShowing = pPlayer->IsNametagShowing ();
        lua_pushboolean ( luaVM, bIsNametagShowing );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPlayerPing ( lua_State* luaVM )
{
//  int getPlayerPing ( player thePlayer )
    CClientPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        // Grab his ping
        unsigned int uiPing = pPlayer->GetPing ();
        lua_pushnumber ( luaVM, uiPing );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPlayerTeam ( lua_State* luaVM )
{
//  team getPlayerTeam ( player thePlayer )
    CClientPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
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
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPlayerDead ( lua_State* luaVM )
{
//  bool isPlayerDead ( player thePlayer )
    CClientPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        // Grab his dead state and return it
        bool bDead = pPlayer->IsDead ();
        lua_pushboolean ( luaVM, bDead );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

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
//  bool showPlayerHudComponent ( string component, bool show )
    eHudComponent component; bool bShow;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumString ( component );
    argStream.ReadBool ( bShow );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ShowPlayerHudComponent ( component, bShow ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPlayerMoney ( lua_State* luaVM )
{
//  bool setPlayerMoney ( int amount )
    int lMoney;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( lMoney );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerMoney ( lMoney ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }        
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GivePlayerMoney ( lua_State* luaVM )
{
//  bool givePlayerMoney ( int amount )
    int lMoney;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( lMoney );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GivePlayerMoney ( lMoney ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }        
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::TakePlayerMoney ( lua_State* luaVM )
{
//  bool takePlayerMoney ( int amount )
    int lMoney;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( lMoney );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::TakePlayerMoney ( lMoney ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }        
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPlayerNametagText ( lua_State* luaVM )
{
//  bool setPlayerNametagText ( player thePlayer, string text )
    CClientEntity* pPlayer; SString strText;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strText );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerNametagText ( *pPlayer, strText ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetPlayerNametagColor ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    if ( !argStream.NextIsBool ( 1 ) )
    {
        // Call type 1
        //  bool setPlayerNametagColor ( player thePlayer, int r, int g, int b )
        CClientEntity* pPlayer; int iR; int iG; int iB;

        argStream.ReadUserData ( pPlayer );
        argStream.ReadNumber ( iR );
        argStream.ReadNumber ( iG );
        argStream.ReadNumber ( iB );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetPlayerNametagColor ( *pPlayer, false, iR, iG, iB ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
    {
        // Call type 2
        //  bool setPlayerNametagColor ( player thePlayer, false )
        CClientEntity* pPlayer; bool bFalse;

        argStream.ReadUserData ( pPlayer );
        argStream.ReadBool ( bFalse );

        if ( !argStream.HasErrors () )
        {
            if ( bFalse == false )
            {
                if ( CStaticFunctionDefinitions::SetPlayerNametagColor ( *pPlayer, true, 255, 255, 255 ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
    }

    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetPlayerNametagShowing ( lua_State* luaVM )
{
//  bool setPlayerNametagShowing ( player thePlayer, bool showing )
    CClientEntity* pPlayer; bool bShowing;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadBool ( bShowing );

    if ( !argStream.HasErrors () )
    {
        // Set the new rotation
        if ( CStaticFunctionDefinitions::SetPlayerNametagShowing ( *pPlayer, bShowing ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

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

int CLuaFunctionDefs::GetPlayerSerial ( lua_State* luaVM )
{
    char szSerial [ 64 ];
    g_pCore->GetNetwork ()->GetSerial ( szSerial, sizeof ( szSerial ) );

    if ( szSerial )
    {
        lua_pushstring ( luaVM, szSerial );
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
