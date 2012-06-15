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
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned char ucID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );

        SString strBuffer;
        if ( CStaticFunctionDefinitions::GetWeaponNameFromID ( ucID, strBuffer ) )
        {
            lua_pushstring ( luaVM, strBuffer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getWeaponNameFromID" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetSlotFromWeapon ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER || lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        unsigned char ucWeaponID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
        char cSlot = CWeaponNames::GetSlotFromWeapon ( ucWeaponID );
        if ( cSlot >= 0 )
            lua_pushnumber ( luaVM, cSlot );
        else
            lua_pushboolean ( luaVM, false );
        //lua_pushnumber ( luaVM, CWeaponNames::GetSlotFromWeapon ( ucWeaponID ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getSlotFromWeapon" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetWeaponIDFromName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char* szName = lua_tostring ( luaVM, 1 );
        unsigned char ucID;

        if ( CStaticFunctionDefinitions::GetWeaponIDFromName ( szName, ucID ) )
        {
            lua_pushnumber ( luaVM, ucID );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getWeaponIDFromName" );

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
        m_pScriptDebugging->LogBadType ( luaVM, "createWeapon" );

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
        m_pScriptDebugging->LogBadType ( luaVM, "fireWeapon" );

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
        if ( CStaticFunctionDefinitions::SetWeaponRotation ( pWeapon, vecPos ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWeaponAimPosition" );

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
        m_pScriptDebugging->LogBadType ( luaVM, "setWeaponState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

