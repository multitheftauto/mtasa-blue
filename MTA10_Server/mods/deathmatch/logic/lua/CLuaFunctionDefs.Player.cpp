/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Player.cpp
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

// Someone decide what to do with this
int CLuaFunctionDefs::SetPlayerAmmo ( lua_State* luaVM )
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


int CLuaFunctionDefs::ExecuteCommandHandler ( lua_State* luaVM )
{
    //  bool executeCommandHandler ( string commandName, player thePlayer, [ string args ] )
    SString strKey; CElement* pElement; SString strArgs;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strArgs, "" );

    if ( !argStream.HasErrors () )
    {

        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CClient* pClient = NULL;
            if ( pElement->GetType () == CElement::PLAYER )
                pClient = static_cast <CClient*> ( static_cast <CPlayer*> ( pElement ) );

            if ( pClient )
            {

                // Call it
                if ( m_pRegisteredCommands->ProcessCommand ( strKey, strArgs, pClient ) )
                {
                    lua_pushboolean ( luaVM, true );
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

int CLuaFunctionDefs::KickPlayer ( lua_State* luaVM )
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


int CLuaFunctionDefs::IsCursorShowing ( lua_State* luaVM )
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


int CLuaFunctionDefs::ShowCursor ( lua_State* luaVM )
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


int CLuaFunctionDefs::ShowChat ( lua_State* luaVM )
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



int CLuaFunctionDefs::BanPlayer ( lua_State* luaVM )
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



int CLuaFunctionDefs::GetPlayerAnnounceValue ( lua_State* luaVM )
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


int CLuaFunctionDefs::SetPlayerAnnounceValue ( lua_State* luaVM )
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


int CLuaFunctionDefs::ResendPlayerModInfo ( lua_State* luaVM )
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


int CLuaFunctionDefs::ResendPlayerACInfo ( lua_State* luaVM )
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


int CLuaFunctionDefs::PlaySoundFrontEnd ( lua_State* luaVM )
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


int CLuaFunctionDefs::BindKey ( lua_State* luaVM )
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



int CLuaFunctionDefs::UnbindKey ( lua_State* luaVM )
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



int CLuaFunctionDefs::IsKeyBound ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetFunctionsBoundToKey ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetKeyBoundToFunction ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetControlState ( lua_State* luaVM )
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


int CLuaFunctionDefs::IsControlEnabled ( lua_State* luaVM )
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


int CLuaFunctionDefs::SetControlState ( lua_State* luaVM )
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

int CLuaFunctionDefs::ToggleControl ( lua_State* luaVM )
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


int CLuaFunctionDefs::ToggleAllControls ( lua_State* luaVM )
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



int CLuaFunctionDefs::CanPlayerUseFunction ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetPlayerName ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetPlayerIP ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetPlayerVersion ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetPlayerAccount ( lua_State* luaVM )
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


int CLuaFunctionDefs::SetPlayerName ( lua_State* luaVM )
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

int CLuaFunctionDefs::DetonateSatchels ( lua_State* luaVM )
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

int CLuaFunctionDefs::GetPlayerFromName ( lua_State* luaVM )
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

int CLuaFunctionDefs::GetPlayerMoney ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetPlayerPing ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetPlayerACInfo ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetPlayerWantedLevel ( lua_State* luaVM )
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

int CLuaFunctionDefs::GetAlivePlayers ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetDeadPlayers ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetPlayerIdleTime ( lua_State* luaVM )
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

int CLuaFunctionDefs::IsPlayerScoreboardForced ( lua_State* luaVM )
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


int CLuaFunctionDefs::IsPlayerMapForced ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetPlayerNametagText ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetPlayerNametagColor ( lua_State* luaVM )
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



int CLuaFunctionDefs::GetPlayerSerial ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetPlayerUserName ( lua_State* luaVM )
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

int CLuaFunctionDefs::GetPlayerCommunityID ( lua_State* luaVM )
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


int CLuaFunctionDefs::SetPlayerMoney ( lua_State* luaVM )
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



int CLuaFunctionDefs::GivePlayerMoney ( lua_State* luaVM )
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


int CLuaFunctionDefs::SpawnPlayer ( lua_State* luaVM )
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



int CLuaFunctionDefs::ShowPlayerHudComponent ( lua_State* luaVM )
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


int CLuaFunctionDefs::TakePlayerScreenShot ( lua_State* luaVM )
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

int CLuaFunctionDefs::SetPlayerWantedLevel ( lua_State* luaVM )
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

int CLuaFunctionDefs::ForcePlayerMap ( lua_State* luaVM )
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


int CLuaFunctionDefs::SetPlayerNametagText ( lua_State* luaVM )
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


int CLuaFunctionDefs::SetPlayerNametagColor ( lua_State* luaVM )
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

int CLuaFunctionDefs::SetPlayerNametagShowing ( lua_State* luaVM )
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


int CLuaFunctionDefs::SetPlayerMuted ( lua_State* luaVM )
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

int CLuaFunctionDefs::SetPlayerBlurLevel ( lua_State* luaVM )
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


int CLuaFunctionDefs::RedirectPlayer ( lua_State* luaVM )
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

int CLuaFunctionDefs::TakePlayerMoney ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetPlayerBlurLevel ( lua_State* luaVM )
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


int CLuaFunctionDefs::IsPlayerNametagShowing ( lua_State* luaVM )
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

int CLuaFunctionDefs::GetPlayerTeam ( lua_State* luaVM )
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



int CLuaFunctionDefs::IsPlayerMuted ( lua_State* luaVM )
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


int CLuaFunctionDefs::GetRandomPlayer ( lua_State* luaVM )
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

int CLuaFunctionDefs::GetPlayerCount ( lua_State* luaVM )
{
    unsigned int uiPlayerCount = CStaticFunctionDefinitions::GetPlayerCount ();

    lua_pushnumber ( luaVM, uiPlayerCount );
    return 1;
}