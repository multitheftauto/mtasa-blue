/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaPlayerDefs.cpp
*  PURPOSE:     Lua player definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaPlayerDefs::LoadFunctions ( void )
{
    // Player get funcs
    CLuaCFunctions::AddFunction ( "getLocalPlayer", GetLocalPlayer );
    CLuaCFunctions::AddFunction ( "getPlayerName", GetPlayerName );
    CLuaCFunctions::AddFunction ( "getPlayerNametagText", GetPlayerNametagText );
    CLuaCFunctions::AddFunction ( "getPlayerNametagColor", GetPlayerNametagColor );
    CLuaCFunctions::AddFunction ( "isPlayerNametagShowing", IsPlayerNametagShowing );
    CLuaCFunctions::AddFunction ( "getPlayerPing", GetPlayerPing );
    CLuaCFunctions::AddFunction ( "getPlayerTeam", GetPlayerTeam );
    CLuaCFunctions::AddFunction ( "getPlayerFromNick", GetPlayerFromName );
    CLuaCFunctions::AddFunction ( "getPlayerFromName", GetPlayerFromName );
    CLuaCFunctions::AddFunction ( "isPlayerHudComponentVisible", IsPlayerHudComponentVisible );
    CLuaCFunctions::AddFunction ( "getPlayerMoney", GetPlayerMoney );
    CLuaCFunctions::AddFunction ( "getPlayerWantedLevel", GetPlayerWantedLevel );

    // Player set funcs
    CLuaCFunctions::AddFunction ( "showPlayerHudComponent", ShowPlayerHudComponent );
    CLuaCFunctions::AddFunction ( "setPlayerHudComponentVisible", ShowPlayerHudComponent );
    CLuaCFunctions::AddFunction ( "setPlayerMoney", SetPlayerMoney );
    CLuaCFunctions::AddFunction ( "givePlayerMoney", GivePlayerMoney );
    CLuaCFunctions::AddFunction ( "takePlayerMoney", TakePlayerMoney );
    CLuaCFunctions::AddFunction ( "setPlayerNametagText", SetPlayerNametagText );
    CLuaCFunctions::AddFunction ( "setPlayerNametagColor", SetPlayerNametagColor );
    CLuaCFunctions::AddFunction ( "setPlayerNametagShowing", SetPlayerNametagShowing );

    // Community funcs
    CLuaCFunctions::AddFunction ( "getPlayerUserName", GetPlayerUserName );
    CLuaCFunctions::AddFunction ( "getPlayerSerial", GetPlayerSerial );

    // Map funcs
    CLuaCFunctions::AddFunction ( "forcePlayerMap", ForcePlayerMap );
    CLuaCFunctions::AddFunction ( "isPlayerMapForced", IsPlayerMapForced );
    CLuaCFunctions::AddFunction ( "isPlayerMapVisible", IsPlayerMapVisible );
    CLuaCFunctions::AddFunction ( "getPlayerMapBoundingBox", GetPlayerMapBoundingBox );
}


void CLuaPlayerDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    // localPlayer only
    lua_classfunction ( luaVM, "giveMoney", "givePlayerMoney" );
    lua_classfunction ( luaVM, "takeMoney", "takePlayerMoney" );
    lua_classfunction ( luaVM, "showHudComponent", "showPlayerHudComponent" );
    lua_classfunction ( luaVM, "setMoney", "setPlayerMoney" );
    lua_classfunction ( luaVM, "getMoney", "getPlayerMoney" );
    lua_classfunction ( luaVM, "getSerial", "getPlayerSerial" );
    lua_classfunction ( luaVM, "getWantedLevel", "getPlayerWantedLevel" );
    lua_classfunction ( luaVM, "getMapBoundingBox", "getPlayerMapBoundingBox" );
    lua_classfunction ( luaVM, "forceMap", "forcePlayerMap" );
    lua_classfunction ( luaVM, "isMapForced", "isPlayerMapForced" );
    lua_classfunction ( luaVM, "isMapVisible", "isPlayerMapVisible" );
    lua_classfunction ( luaVM, "isHudComponentVisible", "isPlayerHudComponentVisible" );
    lua_classfunction ( luaVM, "toggleControl", "toggleControl" );

    lua_classfunction ( luaVM, "create", "getPlayerFromName" );

    lua_classfunction ( luaVM, "setNametagText", "setPlayerNametagText" );
    lua_classfunction ( luaVM, "setNametagShowing", "setPlayerNametagShowing" );
    lua_classfunction ( luaVM, "setNametagColor", "setPlayerNametagColor" );

    lua_classfunction ( luaVM, "getPing", "getPlayerPing" );
    lua_classfunction ( luaVM, "getName", "getPlayerName" );
    lua_classfunction ( luaVM, "getTeam", "getPlayerTeam" );
    lua_classfunction ( luaVM, "getNametagText", "getPlayerNametagText" );
    lua_classfunction ( luaVM, "getNametagColor", "getPlayerNametagColor" );

    lua_classfunction ( luaVM, "isNametagShowing", "isPlayerNametagShowing" );

    lua_classvariable ( luaVM, "ping", NULL, "getPlayerPing" );
    lua_classvariable ( luaVM, "name", NULL, "getPlayerName" );
    lua_classvariable ( luaVM, "team", NULL, "getPlayerTeam" );
    lua_classvariable ( luaVM, "nametagText", "setPlayerNametagText", "getPlayerNametagText" );
    lua_classvariable ( luaVM, "nametagShowing", "setPlayerNametagShowing", "isPlayerNametagShowing" );


    // Static class variables or local only variable
    /*
    lua_classvariable ( luaVM, "blurLevel", "setPlayerBlurLevel", "getPlayerBlurLevel" );
    lua_classvariable ( luaVM, "mapForced", NULL, "isPlayerMapForced" );
    lua_classvariable ( luaVM, "mapVisible", NULL, "isPlayerMapVisible" );
    lua_classvariable ( luaVM, "money", "setPlayerMoney", "getPlayerMoney" );
    lua_classvariable ( luaVM, "serial", NULL, "getPlayerSerial" );
    lua_classvariable ( luaVM, "wantedLevel", NULL, "getPlayerWantedLevel" );*/

    lua_registerclass ( luaVM, "Player", "Ped" );
}


int CLuaPlayerDefs::GetLocalPlayer ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = CStaticFunctionDefinitions::GetLocalPlayer ();

    // Return the local player
    lua_pushelement ( luaVM, pPlayer );
    return 1;
}


int CLuaPlayerDefs::GetPlayerName ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Error
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPlayerDefs::GetPlayerFromName ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Doesn't exist
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPlayerDefs::GetPlayerNametagText ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetPlayerNametagColor ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPlayerDefs::IsPlayerNametagShowing ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetPlayerPing ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetPlayerTeam ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetPlayerMoney ( lua_State* luaVM )
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

int CLuaPlayerDefs::GetPlayerWantedLevel ( lua_State* luaVM )
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

int CLuaPlayerDefs::ShowPlayerHudComponent ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::IsPlayerHudComponentVisible ( lua_State* luaVM )
{
    //  bool isPlayerHudComponentVisible ( string componen )
    eHudComponent component;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumString ( component );

    if ( !argStream.HasErrors () )
    {
        bool bIsVisible;
        if ( CStaticFunctionDefinitions::IsPlayerHudComponentVisible ( component, bIsVisible ) )
        {
            lua_pushboolean ( luaVM, bIsVisible );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::SetPlayerMoney ( lua_State* luaVM )
{
    //  bool setPlayerMoney ( int amount, bool instant = false )
    int lMoney;
    bool bInstant;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( lMoney );
    argStream.ReadBool ( bInstant, false );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerMoney ( lMoney, bInstant ) )
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


int CLuaPlayerDefs::GivePlayerMoney ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::TakePlayerMoney ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::SetPlayerNametagText ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPlayerDefs::SetPlayerNametagColor ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPlayerDefs::SetPlayerNametagShowing ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

// Community

int CLuaPlayerDefs::GetPlayerUserName ( lua_State* luaVM )
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

int CLuaPlayerDefs::GetPlayerSerial ( lua_State* luaVM )
{
    char szSerial[64];
    g_pCore->GetNetwork ()->GetSerial ( szSerial, sizeof ( szSerial ) );

    lua_pushstring ( luaVM, szSerial );
    return 1;
}


// Player Map

int CLuaPlayerDefs::ForcePlayerMap ( lua_State* luaVM )
{
    bool bForced;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadBool ( bForced );

    if ( !argStream.HasErrors () )
    {
        // Force the map to open or close
        if ( CStaticFunctionDefinitions::ForcePlayerMap ( bForced ) )
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

int CLuaPlayerDefs::IsPlayerMapForced ( lua_State* luaVM )
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


int CLuaPlayerDefs::IsPlayerMapVisible ( lua_State* luaVM )
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

int CLuaPlayerDefs::GetPlayerMapBoundingBox ( lua_State* luaVM )
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