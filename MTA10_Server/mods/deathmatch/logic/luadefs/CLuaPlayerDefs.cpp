/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaPlayerDefs.cpp
*  PURPOSE:     Lua player definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaPlayerDefs::LoadFunctions ( void )
{
    // Player get funcs
    CLuaCFunctions::AddFunction ( "getPlayerCount", GetPlayerCount );
    CLuaCFunctions::AddFunction ( "getPlayerFromNick", GetPlayerFromName );
    CLuaCFunctions::AddFunction ( "getPlayerFromName", GetPlayerFromName );
    CLuaCFunctions::AddFunction ( "getPlayerMoney", GetPlayerMoney );
    CLuaCFunctions::AddFunction ( "getPlayerPing", GetPlayerPing );
    CLuaCFunctions::AddFunction ( "getRandomPlayer", GetRandomPlayer );
    CLuaCFunctions::AddFunction ( "isPlayerMuted", IsPlayerMuted );
    CLuaCFunctions::AddFunction ( "getPlayerTeam", GetPlayerTeam );
    CLuaCFunctions::AddFunction ( "getPlayerWantedLevel", GetPlayerWantedLevel );
    CLuaCFunctions::AddFunction ( "getAlivePlayers", GetAlivePlayers );
    CLuaCFunctions::AddFunction ( "getDeadPlayers", GetDeadPlayers );
    CLuaCFunctions::AddFunction ( "getPlayerIdleTime", GetPlayerIdleTime );
    CLuaCFunctions::AddFunction ( "isPlayerScoreboardForced", IsPlayerScoreboardForced );
    CLuaCFunctions::AddFunction ( "isPlayerMapForced", IsPlayerMapForced );
    CLuaCFunctions::AddFunction ( "getPlayerNametagText", GetPlayerNametagText );
    CLuaCFunctions::AddFunction ( "getPlayerNametagColor", GetPlayerNametagColor );
    CLuaCFunctions::AddFunction ( "isPlayerNametagShowing", IsPlayerNametagShowing );
    CLuaCFunctions::AddFunction ( "getPlayerSerial", GetPlayerSerial );
    CLuaCFunctions::AddFunction ( "getPlayerUserName", GetPlayerUserName );
    CLuaCFunctions::AddFunction ( "getPlayerCommunityID", GetPlayerCommunityID );
    CLuaCFunctions::AddFunction ( "getPlayerBlurLevel", GetPlayerBlurLevel );
    CLuaCFunctions::AddFunction ( "getPlayerName", GetPlayerName );
    CLuaCFunctions::AddFunction ( "getPlayerIP", GetPlayerIP );
    CLuaCFunctions::AddFunction ( "getPlayerAccount", GetPlayerAccount );
    CLuaCFunctions::AddFunction ( "getPlayerVersion", GetPlayerVersion );
    CLuaCFunctions::AddFunction ( "getPlayerACInfo", GetPlayerACInfo );
    CLuaCFunctions::AddFunction ( "resendPlayerModInfo", ResendPlayerModInfo );
    CLuaCFunctions::AddFunction ( "resendPlayerACInfo", ResendPlayerACInfo );

    // Player set funcs
    CLuaCFunctions::AddFunction ( "setPlayerMoney", SetPlayerMoney );
    CLuaCFunctions::AddFunction ( "setPlayerAmmo", SetPlayerAmmo );
    CLuaCFunctions::AddFunction ( "givePlayerMoney", GivePlayerMoney );
    CLuaCFunctions::AddFunction ( "takePlayerMoney", TakePlayerMoney );
    CLuaCFunctions::AddFunction ( "spawnPlayer", SpawnPlayer );
    CLuaCFunctions::AddFunction ( "showPlayerHudComponent", ShowPlayerHudComponent );
    CLuaCFunctions::AddFunction ( "setPlayerHudComponentVisible", ShowPlayerHudComponent );
    CLuaCFunctions::AddFunction ( "setPlayerWantedLevel", SetPlayerWantedLevel );
    CLuaCFunctions::AddFunction ( "forcePlayerMap", ForcePlayerMap );
    CLuaCFunctions::AddFunction ( "setPlayerNametagText", SetPlayerNametagText );
    CLuaCFunctions::AddFunction ( "setPlayerNametagColor", SetPlayerNametagColor );
    CLuaCFunctions::AddFunction ( "setPlayerNametagShowing", SetPlayerNametagShowing );
    CLuaCFunctions::AddFunction ( "setPlayerMuted", SetPlayerMuted );
    CLuaCFunctions::AddFunction ( "setPlayerBlurLevel", SetPlayerBlurLevel );
    CLuaCFunctions::AddFunction ( "redirectPlayer", RedirectPlayer );
    CLuaCFunctions::AddFunction ( "setPlayerName", SetPlayerName );
    CLuaCFunctions::AddFunction ( "detonateSatchels", DetonateSatchels );
    CLuaCFunctions::AddFunction ( "takePlayerScreenShot", TakePlayerScreenShot );

    // All seeing eye
    CLuaCFunctions::AddFunction ( "getPlayerAnnounceValue", GetPlayerAnnounceValue );
    CLuaCFunctions::AddFunction ( "setPlayerAnnounceValue", SetPlayerAnnounceValue );

    // Audio funcs
    //CLuaCFunctions::AddFunction ( "playMissionAudio", CLuaFunctionDefinitions::PlayMissionAudio );
    //CLuaCFunctions::AddFunction ( "preloadMissionAudio", CLuaFunctionDefinitions::PreloadMissionAudio );
    CLuaCFunctions::AddFunction ( "playSoundFrontEnd", PlaySoundFrontEnd );

    // Input funcs
    CLuaCFunctions::AddFunction ( "bindKey", BindKey );
    CLuaCFunctions::AddFunction ( "unbindKey", UnbindKey );
    CLuaCFunctions::AddFunction ( "isKeyBound", IsKeyBound );
    CLuaCFunctions::AddFunction ( "getFunctionsBoundToKey", GetFunctionsBoundToKey );
    CLuaCFunctions::AddFunction ( "getKeyBoundToFunction", GetKeyBoundToFunction );
    CLuaCFunctions::AddFunction ( "getControlState", GetControlState );
    CLuaCFunctions::AddFunction ( "isControlEnabled", IsControlEnabled );

    CLuaCFunctions::AddFunction ( "setControlState", SetControlState );
    CLuaCFunctions::AddFunction ( "toggleControl", ToggleControl );
    CLuaCFunctions::AddFunction ( "toggleAllControls", ToggleAllControls );

    // Cursor funcs
    CLuaCFunctions::AddFunction ( "isCursorShowing", IsCursorShowing );
    CLuaCFunctions::AddFunction ( "showCursor", ShowCursor );

    // Chat funcs
    CLuaCFunctions::AddFunction ( "showChat", ShowChat );

    // Admin functions
    CLuaCFunctions::AddFunction ( "kickPlayer", KickPlayer );
    CLuaCFunctions::AddFunction ( "banPlayer", BanPlayer );
}


void CLuaPlayerDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "getAllAlive", "getAlivePlayers" );
    lua_classfunction ( luaVM, "getAllDead", "getDeadPlayers" );
    lua_classfunction ( luaVM, "getRandom", "getRandomPlayer" );
    lua_classfunction ( luaVM, "getCount", "getPlayerCount" );
    lua_classfunction ( luaVM, "isVoiceEnabled", "isVoiceEnabled" );

    lua_classfunction ( luaVM, "create", "getPlayerFromName" );
    lua_classfunction ( luaVM, "logIn", "logIn" );
    lua_classfunction ( luaVM, "ban", "banPlayer" );
    lua_classfunction ( luaVM, "kick", "kickPlayer" );
    lua_classfunction ( luaVM, "redirect", "redirectPlayer" );
    lua_classfunction ( luaVM, "resendModInfo", "resendPlayerModInfo" );
    lua_classfunction ( luaVM, "resendACInfo", "resendPlayerACInfo" );
    lua_classfunction ( luaVM, "spawn", "spawnPlayer" );
    lua_classfunction ( luaVM, "takeMoney", "takePlayerMoney" );
    lua_classfunction ( luaVM, "takeScreenShot", "takePlayerScreenShot" );
    lua_classfunction ( luaVM, "giveMoney", "givePlayerMoney" );
    lua_classfunction ( luaVM, "showHudComponent", "showPlayerHudComponent" );
    lua_classfunction ( luaVM, "hasPermissionTo", "hasObjectPermissionTo" );
    lua_classfunction ( luaVM, "logOut", "logOut" );
    lua_classfunction ( luaVM, "toggleControl", "toggleControl" );
    lua_classfunction ( luaVM, "triggerEvent", "triggerClientEvent" );
    lua_classfunction ( luaVM, "outputChat", "outputChatBox", CLuaFunctionDefs::OOP_OutputChatBox);

    lua_classfunction ( luaVM, "forceMap", "forcePlayerMap" );
    lua_classfunction ( luaVM, "fadeCamera", "fadeCamera" );
    lua_classfunction ( luaVM, "setTeam", "setPlayerTeam" );
    lua_classfunction ( luaVM, "setMuted", "setPlayerMuted" );
    lua_classfunction ( luaVM, "setName", "setPlayerName" );
    lua_classfunction ( luaVM, "setBlurLevel", "setPlayerBlurLevel" );
    lua_classfunction ( luaVM, "setWantedLevel", "setPlayerWantedLevel" );
    lua_classfunction ( luaVM, "setMoney", "setPlayerMoney" );
    lua_classfunction ( luaVM, "setNametagText", "setPlayerNametagText" );
    lua_classfunction ( luaVM, "setNametagShowing", "setPlayerNametagShowing" );
    lua_classfunction ( luaVM, "setNametagColor", "setPlayerNametagColor" );
    lua_classfunction ( luaVM, "setAnnounceValue", "setPlayerAnnounceValue" );
    lua_classfunction ( luaVM, "setVoiceBroadcastTo", "setPlayerVoiceBroadcastTo" );
    lua_classfunction ( luaVM, "setVoiceIgnoreFrom", "setPlayerVoiceIgnoreFrom" );
    lua_classfunction ( luaVM, "setHudComponentVisible", "setPlayerHudComponentVisible" );
    lua_classfunction ( luaVM, "setCameraMatrix", "setCameraMatrix" );
    lua_classfunction ( luaVM, "setCameraInterior", "setCameraInterior" );
    lua_classfunction ( luaVM, "setCameraTarget", "setCameraTarget" );

    lua_classfunction ( luaVM, "isMapForced", "isPlayerMapForced" );
    lua_classfunction ( luaVM, "isMuted", "isPlayerMuted" );
    lua_classfunction ( luaVM, "isNametagShowing", "isPlayerNametagShowing" );
    lua_classfunction ( luaVM, "getNametagText", "getPlayerNametagText" );
    lua_classfunction ( luaVM, "getNametagColor", "getPlayerNametagColor" );
    lua_classfunction ( luaVM, "getName", "getPlayerName" );
    lua_classfunction ( luaVM, "getIdleTime", "getPlayerIdleTime" );
    lua_classfunction ( luaVM, "getPing", "getPlayerPing" );
    lua_classfunction ( luaVM, "getAccount", "getPlayerAccount" );
    lua_classfunction ( luaVM, "getWantedLevel", "getPlayerWantedLevel" );
    lua_classfunction ( luaVM, "getSerial", "getPlayerSerial" );
    lua_classfunction ( luaVM, "getIP", "getPlayerIP" );
    lua_classfunction ( luaVM, "getTeam", "getPlayerTeam" );
    lua_classfunction ( luaVM, "getBlurLevel", "getPlayerBlurLevel" );
    lua_classfunction ( luaVM, "getVersion", "getPlayerVersion" );
    lua_classfunction ( luaVM, "getMoney", "getPlayerMoney" );
    lua_classfunction ( luaVM, "getAnnounceValue", "getPlayerAnnounceValue" );
    lua_classfunction ( luaVM, "getACInfo", "getPlayerACInfo" );
    lua_classfunction ( luaVM, "getCameraInterior", "getCameraInterior" );
    lua_classfunction ( luaVM, "getCameraMatrix", "getCameraMatrix" );
    lua_classfunction ( luaVM, "getCameraTarget", "getCameraTarget" );

    lua_classvariable ( luaVM, "account", NULL, "getPlayerAccount" );
    lua_classvariable ( luaVM, "cameraInterior", "setCameraInterior", "getCameraInterior" );
    lua_classvariable ( luaVM, "cameraMatrix", "setCameraMatrix", "getCameraMatrix" );
    lua_classvariable ( luaVM, "cameraTarget", "setCameraTarget", "getCameraTarget" );
    lua_classvariable ( luaVM, "ACInfo", NULL, "getPlayerACInfo" );
    lua_classvariable ( luaVM, "voiceBroadcastTo", "setPlayerVoiceBroadcastTo", NULL );
    lua_classvariable ( luaVM, "voiceIgnoreFrom", "setPlayerVoiceIgnoreFrom", NULL );
    lua_classvariable ( luaVM, "money", "setPlayerMoney", "getPlayerMoney" );
    lua_classvariable ( luaVM, "version", NULL, "getPlayerVersion" );
    lua_classvariable ( luaVM, "wantedLevel", "setPlayerWantedLevel", "getPlayerWantedLevel" );
    lua_classvariable ( luaVM, "blurLevel", "setPlayerBlurLevel", "getPlayerBlurLevel" );
    lua_classvariable ( luaVM, "name", "setPlayerName", "getPlayerName" );
    lua_classvariable ( luaVM, "muted", "setPlayerMuted", "isPlayerMuted" );
    lua_classvariable ( luaVM, "idleTime", NULL, "getPlayerIdleTime" );
    lua_classvariable ( luaVM, "ping", NULL, "getPlayerPing" );
    lua_classvariable ( luaVM, "serial", NULL, "getPlayerSerial" );
    lua_classvariable ( luaVM, "ip", NULL, "getPlayerIP" );
    lua_classvariable ( luaVM, "team", "setPlayerTeam", "getPlayerTeam" );
    lua_classvariable ( luaVM, "mapForced", "forcePlayerMap", "isPlayerMapForced" );
    lua_classvariable ( luaVM, "nametagText", "setPlayerNametagText", "getPlayerNametagText" );
    lua_classvariable ( luaVM, "nametagShowing", "setPlayerNametagShowing", "isPlayerNametagShowing" );
    //lua_classvariable ( luaVM, "nametagColor", "setPlayerNametagColor", "getPlayerNametagColor", CLuaPlayerDefs::SetPlayerNametagColor, OOP_GetPlayerNametagColor );
    //lua_classvariable ( luaVM, "announceValue", "setPlayerAnnounceValue", "getPlayerAnnounceValue", CLuaPlayerDefs::SetPlayerAnnounceValue, OOP_GetPlayerAnnounceValue ); // .announceValue[key]=value
    //lua_classvariable ( luaVM, "hudComponent", "setHudComponentVisible", "", CLuaOOPDefs::SetHudComponentVisible, NULL ); .hudComponent["radar"]=true (a get function to needs to be implemented too)

    lua_registerclass ( luaVM, "Player", "Ped" );
}



int CLuaPlayerDefs::CanPlayerUseFunction ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strFunction;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strFunction );

    if ( !argStream.HasErrors () )
    {
        bool bCanUse;
        if ( CStaticFunctionDefinitions::CanPlayerUseFunction ( pPlayer, strFunction, bCanUse ) )
        {
            lua_pushboolean ( luaVM, bCanUse );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetPlayerName ( lua_State* luaVM )
{
    //  string getPlayerName ( player thePlayer )
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        SString strNick;
        if ( CStaticFunctionDefinitions::GetPlayerName ( pElement, strNick ) )
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


int CLuaPlayerDefs::GetPlayerIP ( lua_State* luaVM )
{
    //  string getPlayerIP ( player thePlayer )
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        SString strIP;
        if ( CStaticFunctionDefinitions::GetPlayerIP ( pElement, strIP ) )
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


int CLuaPlayerDefs::GetPlayerVersion ( lua_State* luaVM )
{
    //  string getPlayerVersion ( player thePlayer )
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        SString strPlayerVersion = CStaticFunctionDefinitions::GetPlayerVersion ( pPlayer );
        lua_pushstring ( luaVM, strPlayerVersion );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetPlayerAccount ( lua_State* luaVM )
{
    //  account getPlayerAccount ( player thePlayer )
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        CAccount* pAccount = CStaticFunctionDefinitions::GetPlayerAccount ( pElement );
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


int CLuaPlayerDefs::SetPlayerName ( lua_State* luaVM )
{
    //  bool setPlayerName ( player thePlayer, string newName )
    CElement* pElement; SString strName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strName );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerName ( pElement, strName ) )
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

int CLuaPlayerDefs::DetonateSatchels ( lua_State* luaVM )
{
    //  bool detonateSatchels ( element Player )
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::DetonateSatchels ( pElement ) )
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

int CLuaPlayerDefs::GetPlayerFromName ( lua_State* luaVM )
{
    SString strName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );

    if ( !argStream.HasErrors () )
    {
        CPlayer* pPlayer = CStaticFunctionDefinitions::GetPlayerFromName ( strName );
        if ( pPlayer )
        {
            // Return the player
            lua_pushelement ( luaVM, pPlayer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPlayerDefs::GetPlayerMoney ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        long lMoney;

        if ( CStaticFunctionDefinitions::GetPlayerMoney ( pPlayer, lMoney ) )
        {
            lua_pushnumber ( luaVM, lMoney );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetPlayerPing ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        unsigned int uiPing;
        if ( CStaticFunctionDefinitions::GetPlayerPing ( pPlayer, uiPing ) )
        {
            lua_pushnumber ( luaVM, uiPing );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetPlayerACInfo ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        lua_newtable ( luaVM );

        lua_pushstring ( luaVM, "DetectedAC" );
        lua_pushstring ( luaVM, pPlayer->m_strDetectedAC );
        lua_settable ( luaVM, -3 );

        lua_pushstring ( luaVM, "d3d9Size" );
        lua_pushnumber ( luaVM, pPlayer->m_uiD3d9Size );
        lua_settable ( luaVM, -3 );

        lua_pushstring ( luaVM, "d3d9MD5" );
        lua_pushstring ( luaVM, pPlayer->m_strD3d9Md5 );
        lua_settable ( luaVM, -3 );

        lua_pushstring ( luaVM, "d3d9SHA256" );
        lua_pushstring ( luaVM, pPlayer->m_strD3d9Sha256 );
        lua_settable ( luaVM, -3 );

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetPlayerWantedLevel ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        unsigned int uiWantedLevel;
        if ( CStaticFunctionDefinitions::GetPlayerWantedLevel ( pPlayer, uiWantedLevel ) )
        {
            lua_pushnumber ( luaVM, uiWantedLevel );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPlayerDefs::GetAlivePlayers ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Create a new table
        lua_newtable ( luaVM );

        // Add all alive players
        unsigned int uiIndex = 0;
        list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
        for ( ; iter != m_pPlayerManager->IterEnd (); ++iter )
        {
            if ( ( *iter )->IsSpawned () && !( *iter )->IsBeingDeleted () )
            {
                lua_pushnumber ( luaVM, ++uiIndex );
                lua_pushelement ( luaVM, *iter );
                lua_settable ( luaVM, -3 );
            }
        }
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetDeadPlayers ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Create a new table
        lua_newtable ( luaVM );

        // Add all alive players
        unsigned int uiIndex = 0;
        list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
        for ( ; iter != m_pPlayerManager->IterEnd (); ++iter )
        {
            if ( !( *iter )->IsSpawned () && !( *iter )->IsBeingDeleted () )
            {
                lua_pushnumber ( luaVM, ++uiIndex );
                lua_pushelement ( luaVM, *iter );
                lua_settable ( luaVM, -3 );
            }
        }
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetPlayerIdleTime ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        long long llLastPositionChanged = pPlayer->GetPositionLastChanged ();
        if ( llLastPositionChanged == 0 )
        {
            // DO NOT REMOVE THIS AND DEFAULT THE POSITION LAST CHANGED TO THE CURRENT TIME OR YOU WILL BREAK EVERYTHING.
            // He hasn't idled since he just joined so give them 0 idle time
            lua_pushnumber ( luaVM, 0.0 );
            return 1;
        }
        else
        {
            lua_pushnumber ( luaVM, static_cast <double> ( GetTickCount64_ () - llLastPositionChanged ) );
            return 1;
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPlayerDefs::IsPlayerScoreboardForced ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        bool bForced;
        if ( CStaticFunctionDefinitions::IsPlayerScoreboardForced ( pPlayer, bForced ) )
        {
            lua_pushboolean ( luaVM, bForced );
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
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        bool bForced;
        if ( CStaticFunctionDefinitions::IsPlayerMapForced ( pPlayer, bForced ) )
        {
            lua_pushboolean ( luaVM, bForced );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetPlayerNametagText ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        SString strText;
        if ( CStaticFunctionDefinitions::GetPlayerNametagText ( pPlayer, strText ) )
        {
            lua_pushstring ( luaVM, strText );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetPlayerNametagColor ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {

        unsigned char ucR, ucG, ucB;
        if ( CStaticFunctionDefinitions::GetPlayerNametagColor ( pPlayer, ucR, ucG, ucB ) )
        {
            lua_pushnumber ( luaVM, ucR );
            lua_pushnumber ( luaVM, ucG );
            lua_pushnumber ( luaVM, ucB );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaPlayerDefs::GetPlayerSerial ( lua_State* luaVM )
{
    CPlayer* pPlayer; uint uiIndex;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadNumber ( uiIndex, 0 );

    if ( !argStream.HasErrors () )
    {
        SString strSerial = CStaticFunctionDefinitions::GetPlayerSerial ( pPlayer, uiIndex );
        if ( !strSerial.empty () )
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


int CLuaPlayerDefs::GetPlayerUserName ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        SString strUser = CStaticFunctionDefinitions::GetPlayerUserName ( pPlayer );
        if ( !strUser.empty () )
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

int CLuaPlayerDefs::GetPlayerCommunityID ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        SString strID = CStaticFunctionDefinitions::GetPlayerCommunityID ( pPlayer );
        if ( !strID.empty () )
        {
            lua_pushstring ( luaVM, strID );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaPlayerDefs::SetPlayerMoney ( lua_State* luaVM )
{
    CElement* pElement;
    long lMoney;
    bool bInstant;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( lMoney );
    argStream.ReadBool ( bInstant, false );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerMoney ( pElement, lMoney, bInstant ) )
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
    CElement* pElement;
    long lMoney;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( lMoney );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GivePlayerMoney ( pElement, lMoney ) )
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


int CLuaPlayerDefs::SpawnPlayer ( lua_State* luaVM )
{
    CPlayer * pElement;
    CVector vecPosition;
    float fRotation;
    unsigned long ulModel;
    unsigned char ucInterior;
    unsigned short usDimension;
    CTeam * pTeam;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadVector3D ( vecPosition );

    if ( argStream.NextIsUserDataOfType < CLuaVector3D > () || argStream.NextIsUserDataOfType < CLuaVector4D > () )
    {
        CVector vecRotation;
        argStream.ReadVector3D ( vecRotation );
        fRotation = vecRotation.fZ;
    }
    else
        argStream.ReadNumber ( fRotation, 0.0f );

    argStream.ReadNumber ( ulModel, 0 );
    argStream.ReadNumber ( ucInterior, 0 );
    argStream.ReadNumber ( usDimension, 0 );
    argStream.ReadUserData ( pTeam, NULL );


    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SpawnPlayer ( pElement, vecPosition, fRotation, ulModel, ucInterior, usDimension, pTeam ) )
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



int CLuaPlayerDefs::ShowPlayerHudComponent ( lua_State* luaVM )
{
    //  bool showPlayerHudComponent ( player thePlayer, string component, bool show )
    CElement* pElement; eHudComponent component; bool bShow;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadEnumString ( component );
    argStream.ReadBool ( bShow );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ShowPlayerHudComponent ( pElement, component, bShow ) )
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


int CLuaPlayerDefs::TakePlayerScreenShot ( lua_State* luaVM )
{
    //  bool takePlayerScreenShot ( player thePlayer, int sizeX, int sizeY, string tag, int quality, int maxBandwidth, int maxPacketSize )
    CElement* pElement; uint sizeX; uint sizeY; SString tag; uint quality; uint maxBandwidth; uint maxPacketSize;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( sizeX );
    argStream.ReadNumber ( sizeY );
    argStream.ReadString ( tag, "" );
    argStream.ReadNumber ( quality, 30 );
    argStream.ReadNumber ( maxBandwidth, 5000 );
    argStream.ReadNumber ( maxPacketSize, 500 );

    if ( !argStream.HasErrors () )
    {
        CLuaMain * pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        CResource * pResource = pLuaMain ? pLuaMain->GetResource () : NULL;
        if ( pResource )
        {
            if ( CStaticFunctionDefinitions::TakePlayerScreenShot ( pElement, sizeX, sizeY, tag, quality, maxBandwidth, maxPacketSize, pResource ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPlayerDefs::SetPlayerWantedLevel ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned int uiWantedLevel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( uiWantedLevel );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerWantedLevel ( pElement, uiWantedLevel ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "Valid wanted levels are between 0 and 6 inclusive" );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPlayerDefs::ForcePlayerMap ( lua_State* luaVM )
{
    CElement* pElement;
    bool bForce;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bForce );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ForcePlayerMap ( pElement, bForce ) )
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
    CElement* pElement;
    SString strText;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strText );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerNametagText ( pElement, strText ) )
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
    CElement* pElement;
    unsigned char ucR;
    unsigned char ucG;
    unsigned char ucB;
    bool bRemoveOverride = false;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( argStream.NextIsBool () )
    {
        argStream.ReadBool ( bRemoveOverride );
        bRemoveOverride = !bRemoveOverride ? true : false;
    }
    else
    {
        argStream.ReadNumber ( ucR );
        argStream.ReadNumber ( ucG );
        argStream.ReadNumber ( ucB );
    }

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerNametagColor ( pElement, bRemoveOverride, ucR, ucG, ucB ) )
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

int CLuaPlayerDefs::SetPlayerNametagShowing ( lua_State* luaVM )
{
    CElement* pElement;
    bool bShowing;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bShowing );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerNametagShowing ( pElement, bShowing ) )
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


int CLuaPlayerDefs::SetPlayerMuted ( lua_State* luaVM )
{
    CElement* pElement;
    bool bMuted;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bMuted );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerMuted ( pElement, bMuted ) )
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

int CLuaPlayerDefs::SetPlayerBlurLevel ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucLevel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucLevel );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerBlurLevel ( pElement, ucLevel ) )
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


int CLuaPlayerDefs::RedirectPlayer ( lua_State* luaVM )
{
    CElement* pElement;
    SString strHost;
    unsigned short usPort;
    SString strPassword;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strHost );
    argStream.ReadNumber ( usPort );
    argStream.ReadString ( strPassword, "" );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RedirectPlayer ( pElement, strHost, usPort, strPassword.empty () ? NULL : strPassword.c_str () ) )
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
    CElement* pElement;
    long lMoney;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( lMoney );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::TakePlayerMoney ( pElement, lMoney ) )
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


int CLuaPlayerDefs::GetPlayerBlurLevel ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucLevel;
        if ( CStaticFunctionDefinitions::GetPlayerBlurLevel ( pPlayer, ucLevel ) )
        {
            lua_pushnumber ( luaVM, ucLevel );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::IsPlayerNametagShowing ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        bool bShowing;
        if ( CStaticFunctionDefinitions::IsPlayerNametagShowing ( pPlayer, bShowing ) )
        {
            lua_pushboolean ( luaVM, bShowing );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPlayerDefs::GetPlayerTeam ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        CTeam *pTeam = CStaticFunctionDefinitions::GetPlayerTeam ( pPlayer );
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



int CLuaPlayerDefs::IsPlayerMuted ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        bool bMuted;
        if ( CStaticFunctionDefinitions::IsPlayerMuted ( pPlayer, bMuted ) )
        {
            lua_pushboolean ( luaVM, bMuted );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetRandomPlayer ( lua_State* luaVM )
{
    CPlayer* pPlayer = CStaticFunctionDefinitions::GetRandomPlayer ();
    if ( pPlayer )
    {
        lua_pushelement ( luaVM, pPlayer );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPlayerDefs::GetPlayerCount ( lua_State* luaVM )
{
    unsigned int uiPlayerCount = CStaticFunctionDefinitions::GetPlayerCount ();

    lua_pushnumber ( luaVM, uiPlayerCount );
    return 1;
}


// Someone decide what to do with this
int CLuaPlayerDefs::SetPlayerAmmo ( lua_State* luaVM )
{
    CPlayer * pElement;
    unsigned char ucSlot;
    unsigned short usAmmo, usAmmoInClip;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( usAmmo );
    argStream.ReadNumber ( ucSlot, 0xFF );
    argStream.ReadNumber ( usAmmoInClip, 0 );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerAmmo ( pElement, ucSlot, usAmmo, usAmmoInClip ) )
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


int CLuaPlayerDefs::GetPlayerAnnounceValue ( lua_State* luaVM )
{
    // string getPlayerAnnounceValue ( element thePlayer, string key )
    CPlayer* pPlayer; SString strKey;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors () )
    {
        SString strValue;
        if ( CStaticFunctionDefinitions::GetPlayerAnnounceValue ( pPlayer, strKey, strValue ) )
        {
            lua_pushstring ( luaVM, strValue );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::SetPlayerAnnounceValue ( lua_State* luaVM )
{
    // bool setPlayerAnnounceValue ( element thePlayer, string key, string value )
    CPlayer* pPlayer; SString strKey; SString strValue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strValue );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerAnnounceValue ( pPlayer, strKey, strValue ) )
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


int CLuaPlayerDefs::ResendPlayerModInfo ( lua_State* luaVM )
{
    // bool resendPlayerModInfo ( player thePlayer )
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        g_pNetServer->ResendModPackets ( pPlayer->GetSocket () );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::ResendPlayerACInfo ( lua_State* luaVM )
{
    // bool resendPlayerACInfo ( player thePlayer )
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        g_pNetServer->ResendACPackets ( pPlayer->GetSocket () );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::BindKey ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strKey;
    SString strHitState;
    SString strCommand;
    SString strArguments;

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( !pLuaMain )
    {
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strHitState );

    if ( argStream.NextIsString () )
    {
        // bindKey ( player thePlayer, string key, string keyState, string commandName, [ string arguments ] )
        argStream.ReadString ( strCommand );
        argStream.ReadString ( strArguments, "" );
        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::BindKey ( pPlayer, strKey, strHitState, strCommand, strArguments, pLuaMain->GetResource ()->GetName ().c_str () ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else if ( !argStream.HasErrors () )
    {
        // bindKey ( player thePlayer, string key, string keyState, function handlerFunction,  [ var arguments, ... ] )
        CLuaFunctionRef iLuaFunction;
        CLuaArguments Arguments;
        argStream.ReadFunction ( iLuaFunction );
        argStream.ReadLuaArguments ( Arguments );
        argStream.ReadFunctionComplete ();
        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::BindKey ( pPlayer, strKey, strHitState, pLuaMain, iLuaFunction, Arguments ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaPlayerDefs::UnbindKey ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strKey;
    SString strHitState;
    SString strCommand;

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( !pLuaMain )
    {
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strKey );

    if ( argStream.NextIsString ( 1 ) ) // Check if has command
    {
        // bool unbindKey ( player thePlayer, string key, string keyState, string command )
        argStream.ReadString ( strHitState );
        argStream.ReadString ( strCommand );
        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::UnbindKey ( pPlayer, strKey, strHitState, strCommand, pLuaMain->GetResource ()->GetName ().c_str () ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else if ( !argStream.HasErrors () )
    {
        // bool unbindKey ( player thePlayer, string key, [ string keyState, function handler  ] )
        CLuaFunctionRef iLuaFunction;
        argStream.ReadString ( strHitState, "" );
        argStream.ReadFunction ( iLuaFunction, LUA_REFNIL );
        argStream.ReadFunctionComplete ();
        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::UnbindKey ( pPlayer, strKey, pLuaMain, strHitState, iLuaFunction ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaPlayerDefs::IsKeyBound ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strKey;
    SString strHitState;
    SString strCommand;
    SString strArguments;
    CLuaFunctionRef iLuaFunction;

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( !pLuaMain )
    {
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strHitState, "" );

    if ( lua_type ( luaVM, 4 ) )
        iLuaFunction = luaM_toref ( luaVM, 4 );

    if ( !argStream.HasErrors () )
    {
        bool bBound;
        if ( CStaticFunctionDefinitions::IsKeyBound ( pPlayer, strKey, pLuaMain, strHitState.empty () ? NULL : strHitState.c_str (), iLuaFunction, bBound ) )
        {
            lua_pushboolean ( luaVM, bBound );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetFunctionsBoundToKey ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strKey;
    SString strHitState;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strHitState, "" );

    if ( !argStream.HasErrors () )
    {
        bool bCheckHitState = false, bHitState = false;
        if ( strHitState == "down" )
        {
            bCheckHitState = true;
            bHitState = true;
        }
        else if ( strHitState == "up" )
        {
            bCheckHitState = true;
            bHitState = false;
        }

        // Create a new table
        lua_newtable ( luaVM );

        // Add all the bound functions to it
        unsigned int uiIndex = 0;
        list < CKeyBind* > ::iterator iter = pPlayer->GetKeyBinds ()->IterBegin ();
        for ( ; iter != pPlayer->GetKeyBinds ()->IterEnd (); ++iter )
        {
            CKeyBind* pKeyBind = *iter;
            if ( !pKeyBind->IsBeingDeleted () )
            {
                switch ( pKeyBind->GetType () )
                {
                case KEY_BIND_FUNCTION:
                {
                    CKeyFunctionBind* pBind = static_cast <CKeyFunctionBind*> ( pKeyBind );
                    if ( !bCheckHitState || pBind->bHitState == bHitState )
                    {
                        if ( strKey == pBind->boundKey->szKey )
                        {
                            lua_pushnumber ( luaVM, ++uiIndex );
                            lua_rawgeti ( luaVM, LUA_REGISTRYINDEX, pBind->m_iLuaFunction.ToInt () );
                            lua_settable ( luaVM, -3 );
                        }
                    }
                    break;
                }
                case KEY_BIND_CONTROL_FUNCTION:
                {
                    CControlFunctionBind* pBind = static_cast <CControlFunctionBind*> ( pKeyBind );
                    if ( !bCheckHitState || pBind->bHitState == bHitState )
                    {
                        if ( strKey == pBind->boundControl->szControl )
                        {
                            lua_pushnumber ( luaVM, ++uiIndex );
                            lua_rawgeti ( luaVM, LUA_REGISTRYINDEX, pBind->m_iLuaFunction.ToInt () );
                            lua_settable ( luaVM, -3 );
                        }
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetKeyBoundToFunction ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strKey;
    CLuaFunctionRef iLuaFunction;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        if ( lua_type ( luaVM, 2 ) != LUA_TFUNCTION )
        {
            m_pScriptDebugging->LogBadType ( luaVM );
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        iLuaFunction = luaM_toref ( luaVM, 2 );

        list < CKeyBind* > ::iterator iter = pPlayer->GetKeyBinds ()->IterBegin ();
        for ( ; iter != pPlayer->GetKeyBinds ()->IterEnd (); ++iter )
        {
            CKeyBind* pKeyBind = *iter;
            if ( !pKeyBind->IsBeingDeleted () )
            {
                switch ( pKeyBind->GetType () )
                {
                case KEY_BIND_FUNCTION:
                {
                    CKeyFunctionBind* pBind = static_cast <CKeyFunctionBind*> ( pKeyBind );
                    if ( iLuaFunction == pBind->m_iLuaFunction )
                    {
                        lua_pushstring ( luaVM, pBind->boundKey->szKey );
                        return 1;
                    }
                    break;
                }
                case KEY_BIND_CONTROL_FUNCTION:
                {
                    CControlFunctionBind* pBind = static_cast <CControlFunctionBind*> ( pKeyBind );
                    if ( iLuaFunction == pBind->m_iLuaFunction )
                    {
                        lua_pushstring ( luaVM, pBind->boundKey->szKey );
                        return 1;
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );


    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::GetControlState ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strControl;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strControl );

    if ( !argStream.HasErrors () )
    {
        bool bState;
        if ( CStaticFunctionDefinitions::GetControlState ( pPlayer, strControl, bState ) )
        {
            lua_pushboolean ( luaVM, bState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::IsControlEnabled ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strControl;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strControl );

    if ( !argStream.HasErrors () )
    {
        bool bState;
        if ( CStaticFunctionDefinitions::IsControlEnabled ( pPlayer, strControl, bState ) )
        {
            lua_pushboolean ( luaVM, bState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::SetControlState ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strControl;
    bool bState;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strControl );
    argStream.ReadBool ( bState );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetControlState ( pPlayer, strControl, bState ) )
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

int CLuaPlayerDefs::ToggleControl ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strControl;
    bool bState;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strControl );
    argStream.ReadBool ( bState );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ToggleControl ( pPlayer, strControl, bState ) )
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


int CLuaPlayerDefs::ToggleAllControls ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    bool bEnabled;
    bool bGTAControls;
    bool bMTAControls;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadBool ( bEnabled );
    argStream.ReadBool ( bGTAControls, true );
    argStream.ReadBool ( bMTAControls, true );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ToggleAllControls ( pPlayer, bGTAControls, bMTAControls, bEnabled ) )
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


int CLuaPlayerDefs::PlaySoundFrontEnd ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucSound;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucSound );

    if ( !argStream.HasErrors () )
    {
        if ( ucSound <= 101 )
        {
            if ( CStaticFunctionDefinitions::PlaySoundFrontEnd ( pElement, ucSound ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "Invalid sound ID specified. Valid sound IDs are 0 - 101." );

    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPlayerDefs::KickPlayer ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strResponsible;
    SString strReason;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( argStream.NextIsUserData () )
    {
        CElement* pResponsible;
        argStream.ReadUserData ( pResponsible );
        if ( CPlayer* pResponsiblePlayer = dynamic_cast <CPlayer*> ( pResponsible ) )
            strResponsible = pResponsiblePlayer->GetNick ();
        else
            strResponsible = "Console";

        argStream.ReadString ( strReason, "" );
    }
    else
    {
        if ( argStream.NextIsString ( 1 ) )
        {
            argStream.ReadString ( strResponsible );
            argStream.ReadString ( strReason );
        }
        else
        {
            argStream.ReadString ( strReason, "" );
            strResponsible = "";
        }
    }

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::KickPlayer ( pPlayer, strResponsible, strReason ) )
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


int CLuaPlayerDefs::IsCursorShowing ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        bool bShowing;
        if ( CStaticFunctionDefinitions::IsCursorShowing ( pPlayer, bShowing ) )
        {
            lua_pushboolean ( luaVM, bShowing );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPlayerDefs::ShowCursor ( lua_State* luaVM )
{
    CElement* pPlayer;
    bool bShow;
    bool bToggleControls;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadBool ( bShow );
    argStream.ReadBool ( bToggleControls, true );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain && CStaticFunctionDefinitions::ShowCursor ( pPlayer, pLuaMain, bShow, bToggleControls ) )
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


int CLuaPlayerDefs::ShowChat ( lua_State* luaVM )
{
    CElement* pPlayer;
    bool bShow;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadBool ( bShow );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ShowChat ( pPlayer, bShow ) )
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



int CLuaPlayerDefs::BanPlayer ( lua_State* luaVM )
{
    //  ban banPlayer ( player bannedPlayer, [ bool IP = true, bool Username = false, bool Serial = false, player responsiblePlayer = nil, string reason = nil, int seconds = 0 ] )
    CPlayer* pPlayer;
    SString strResponsible;
    SString strReason;
    CPlayer* pResponsible = NULL;

    bool bIP;
    bool bUsername;
    bool bSerial;
    time_t tUnban;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadBool ( bIP, true );
    argStream.ReadBool ( bUsername, false );
    argStream.ReadBool ( bSerial, false );
    if ( argStream.NextIsUserData () )
    {
        CElement* pResponsibleElement;
        argStream.ReadUserData ( pResponsibleElement );
        if ( ( pResponsible = dynamic_cast <CPlayer*> ( pResponsibleElement ) ) )
            strResponsible = pResponsible->GetNick ();
        else
            strResponsible = "Console";
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
        if ( ( pBan = CStaticFunctionDefinitions::BanPlayer ( pPlayer, bIP, bUsername, bSerial, pResponsible, strResponsible, strReason, tUnban ) ) )
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


int CLuaPlayerDefs::PlayMissionAudio ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecPosition;
    unsigned short usSlot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( usSlot );

    if ( argStream.NextCouldBeNumber () )
    {
        argStream.ReadVector3D ( vecPosition );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::PlayMissionAudio ( pElement, &vecPosition, usSlot ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    }
    else if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::PlayMissionAudio ( pElement, NULL, usSlot ) )
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

int CLuaPlayerDefs::PreloadMissionAudio ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned short usSound;
    unsigned short usSlot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( usSound );
    argStream.ReadNumber ( usSlot );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::PreloadMissionAudio ( pElement, usSound, usSlot ) )
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