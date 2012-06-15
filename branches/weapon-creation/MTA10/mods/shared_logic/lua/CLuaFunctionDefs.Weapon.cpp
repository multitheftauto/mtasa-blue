/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Weapon.cpp
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

int CLuaFunctionDefs::GetWeaponNameFromID ( lua_State* luaVM )
{
    unsigned char ucID = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber( ucID );

    if ( !argStream.HasErrors ( ) )
    {

        SString strBuffer;
        if ( CStaticFunctionDefinitions::GetWeaponNameFromID ( ucID, strBuffer ) )
        {
            lua_pushstring ( luaVM, strBuffer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "getWeaponNameFromID", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetSlotFromWeapon ( lua_State* luaVM )
{
    unsigned char ucWeaponID = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber( ucWeaponID );

    if ( !argStream.HasErrors ( ) )
    {
        char cSlot = CWeaponNames::GetSlotFromWeapon ( ucWeaponID );
        if ( cSlot >= 0 )
            lua_pushnumber ( luaVM, cSlot );
        else
            lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "getSlotFromWeapon", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetWeaponIDFromName ( lua_State* luaVM )
{
    SString strName = "";
    unsigned char ucID = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString( strName );
    argStream.ReadNumber ( ucID );
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::GetWeaponIDFromName ( strName, ucID ) )
        {
            lua_pushnumber ( luaVM, ucID );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "getWeaponIDFromName", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreateWeapon ( lua_State* luaVM )
{
    CVector vecPos;
    eWeaponType weaponType;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPos.fX );
    argStream.ReadNumber ( vecPos.fY );
    argStream.ReadNumber ( vecPos.fZ );
    argStream.ReadEnumString ( weaponType );

    if ( !argStream.HasErrors () )
    {
        CClientWeapon * pWeapon = CStaticFunctionDefinitions::CreateWeapon ( weaponType, vecPos );
        if ( pWeapon )
        {
            lua_pushelement ( luaVM, pWeapon );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "createWeapon", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}
int CLuaFunctionDefs::FireWeapon ( lua_State* luaVM )
{
    CClientWeapon * pWeapon;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::FireWeapon ( pWeapon ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fireWeapon", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWeaponAimPosition ( lua_State* luaVM )
{
    CClientWeapon * pWeapon;
    CVector vecPos;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    argStream.ReadNumber ( vecPos.fX );
    argStream.ReadNumber ( vecPos.fY );
    argStream.ReadNumber ( vecPos.fZ );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetAimPosition ( pWeapon, vecPos ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setWeaponAimPosition", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefs::SetWeaponState ( lua_State* luaVM )
{
    CClientWeapon * pWeapon;
    eWeaponState weaponState;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    argStream.ReadEnumString ( weaponState );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetWeaponState ( pWeapon, weaponState ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setWeaponState", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

