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
    argStream.ReadEnumString ( weaponType );
    argStream.ReadNumber ( vecPos.fX );
    argStream.ReadNumber ( vecPos.fY );
    argStream.ReadNumber ( vecPos.fZ );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                CClientWeapon * pWeapon = CStaticFunctionDefinitions::CreateWeapon ( *pResource, weaponType, vecPos );
                if ( pWeapon )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( ( CClientEntity* ) pWeapon );
                    }

                    lua_pushelement ( luaVM, pWeapon );
                    return 1;
                }
            }
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

int CLuaFunctionDefs::SetWeaponProperty ( lua_State* luaVM )
{
    CClientWeapon * pWeapon;
    short sDamage = 0;
    eWeaponProperty weaponProperty;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    argStream.ReadEnumString ( weaponProperty );
    argStream.ReadNumber ( sDamage );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetWeaponProperty ( pWeapon, weaponProperty, sDamage ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setWeaponProperty", *argStream.GetErrorMessage () ) );

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


int CLuaFunctionDefs::SetWeaponTarget ( lua_State* luaVM )
{
    CClientWeapon * pWeapon;
    CClientEntity * pTarget;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    if ( argStream.NextIsUserData() )
    {
        int targetBone;
        argStream.ReadUserData ( pTarget );
        argStream.ReadNumber ( targetBone, BONE_PELVIS );
        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetWeaponTarget ( pWeapon, pTarget, (eBone)targetBone ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setWeaponTarget", *argStream.GetErrorMessage () ) );
    }
    else if ( argStream.NextIsNumber() )
    {
        CVector vecTarget;
        argStream.ReadNumber( vecTarget.fX );
        argStream.ReadNumber( vecTarget.fY );
        argStream.ReadNumber( vecTarget.fZ );
        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetWeaponTarget ( pWeapon, vecTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setWeaponTarget", *argStream.GetErrorMessage () ) );
    }
    else if ( argStream.NextIsNil() )
    {
        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::ClearWeaponTarget ( pWeapon ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setWeaponTarget", *argStream.GetErrorMessage () ) );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetWeaponOwner ( lua_State* luaVM )
{
    CClientWeapon * pWeapon;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    if ( !argStream.HasErrors () )
    {
        CClientPlayer* pOwner = pWeapon->GetOwner();
        if ( pOwner )
        {
            lua_pushelement ( luaVM, pOwner );
            return 1;
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWeaponOwner ( lua_State* luaVM )
{
    CClientWeapon * pWeapon;
    CClientPlayer * pPlayer;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    if ( argStream.NextIsUserData() )
    {
        argStream.ReadUserData ( pPlayer );
        if ( !argStream.HasErrors () )
        {
            pWeapon->SetOwner( pPlayer );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else if ( argStream.NextIsNil() )
    {
        if ( !argStream.HasErrors () )
        {
            pWeapon->SetOwner( NULL );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    lua_pushnil ( luaVM );
    return 1;
}