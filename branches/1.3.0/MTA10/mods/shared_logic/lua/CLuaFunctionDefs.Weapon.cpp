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
        argStream.ReadNumber ( targetBone, 255 );
        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetWeaponTarget ( pWeapon, pTarget, targetBone ) )
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
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "getWeaponOwner", *argStream.GetErrorMessage () ) );

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
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setWeaponOwner", *argStream.GetErrorMessage () ) );

    lua_pushnil ( luaVM );
    return 1;
}
int CLuaFunctionDefs::SetWeaponFlags ( lua_State* luaVM )
{
    CClientWeapon * pWeapon = NULL;
    SLineOfSightFlags flags;
    eWeaponFlags flag;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    argStream.ReadEnumString ( flag );
    if ( !argStream.HasErrors() )
    {
        if ( flag != eWeaponFlags::FLAGS )
        {
            bool bData;
            argStream.ReadBool ( bData );
            if ( CStaticFunctionDefinitions::GetWeaponFlags( pWeapon, flag, bData ) )
            {
                lua_pushboolean( luaVM, bData );
                return 1;
            }
        }
        else
        {
            argStream.ReadBool ( flags.bCheckBuildings );
            argStream.ReadBool ( flags.bCheckCarTires );
            argStream.ReadBool ( flags.bCheckDummies );
            argStream.ReadBool ( flags.bCheckObjects );
            argStream.ReadBool ( flags.bCheckPeds );
            argStream.ReadBool ( flags.bCheckVehicles );
            argStream.ReadBool ( flags.bSeeThroughStuff );
            argStream.ReadBool ( flags.bShootThroughStuff );
            if ( !argStream.HasErrors() )
            {
                if ( CStaticFunctionDefinitions::SetWeaponFlags( pWeapon, flags ) )
                {
                    lua_pushboolean( luaVM, true );
                }
            }
            else
                m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setWeaponFlags", *argStream.GetErrorMessage () ) );

        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setWeaponFlags", *argStream.GetErrorMessage () ) );

    lua_pushboolean( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetWeaponFlags ( lua_State* luaVM )
{
    CClientWeapon * pWeapon = NULL;
    SLineOfSightFlags flags;
    eWeaponFlags flag;
    bool bData;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    argStream.ReadEnumString ( flag );
    if ( !argStream.HasErrors() )
    {
        if ( flag != eWeaponFlags::FLAGS )
        {
            if ( CStaticFunctionDefinitions::GetWeaponFlags( pWeapon, flag, bData ) )
            {
                lua_pushboolean( luaVM, bData );
                return 1;
            }
        }
        else
        {
            if ( CStaticFunctionDefinitions::GetWeaponFlags( pWeapon, flags ) )
            {
                lua_pushboolean( luaVM, flags.bCheckBuildings );
                lua_pushboolean( luaVM, flags.bCheckCarTires );
                lua_pushboolean( luaVM, flags.bCheckDummies );
                lua_pushboolean( luaVM, flags.bCheckObjects );
                lua_pushboolean( luaVM, flags.bCheckPeds );
                lua_pushboolean( luaVM, flags.bCheckVehicles );
                lua_pushboolean( luaVM, flags.bSeeThroughStuff );
                lua_pushboolean( luaVM, flags.bShootThroughStuff );
                return 8;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "getWeaponFlags", *argStream.GetErrorMessage () ) );

    lua_pushboolean( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWeaponFiringRate ( lua_State* luaVM )
{
    CClientWeapon * pWeapon = NULL;
    int iFiringRate = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    argStream.ReadNumber ( iFiringRate );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::SetWeaponFiringRate( pWeapon, iFiringRate ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setWeaponFiringRate", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetWeaponFiringRate ( lua_State* luaVM )
{
    CClientWeapon * pWeapon = NULL;
    int iFiringRate = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::GetWeaponFiringRate( pWeapon, iFiringRate ) )
        {
            lua_pushnumber ( luaVM, iFiringRate );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "getWeaponFiringRate", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::ResetWeaponFiringRate ( lua_State* luaVM )
{
    CClientWeapon * pWeapon = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::ResetWeaponFiringRate( pWeapon ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "resetWeaponFiringRate", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetWeaponClipAmmo ( lua_State* luaVM )
{
    CClientWeapon * pWeapon = NULL;
    int iClipAmmo = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::GetWeaponClipAmmo( pWeapon, iClipAmmo ) )
        {
            lua_pushnumber ( luaVM, iClipAmmo );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "getWeaponClipAmmo", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetWeaponAmmo ( lua_State* luaVM )
{
    CClientWeapon * pWeapon = NULL;
    int iAmmo = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::GetWeaponAmmo( pWeapon, iAmmo ) )
        {
            lua_pushnumber ( luaVM, iAmmo );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "getWeaponAmmo", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWeaponAmmo ( lua_State* luaVM )
{
    CClientWeapon * pWeapon = NULL;
    int iAmmo = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    argStream.ReadNumber ( iAmmo );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::SetWeaponAmmo( pWeapon, iAmmo ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setWeaponAmmo", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWeaponClipAmmo ( lua_State* luaVM )
{
    CClientWeapon * pWeapon = NULL;
    int iAmmo = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    argStream.ReadNumber ( iAmmo );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::SetWeaponClipAmmo( pWeapon, iAmmo ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setWeaponClipAmmo", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}
