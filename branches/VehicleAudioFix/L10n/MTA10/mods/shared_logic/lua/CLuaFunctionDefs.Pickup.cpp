/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Pickup.cpp
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

int CLuaFunctionDefs::CreatePickup ( lua_State* luaVM )
{
    CVector vecPosition;
    unsigned long ulRespawnInterval = 30000;
    double dblAmmo = 50.0;
    unsigned char ucType = 0;
    double dArgumentDependant = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( ucType );
    argStream.ReadNumber ( dArgumentDependant );
    argStream.ReadNumber ( ulRespawnInterval, 30000 );
    argStream.ReadNumber ( dblAmmo, 50.0 );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                CClientPickup* pPickup = CStaticFunctionDefinitions::CreatePickup ( *pResource, vecPosition, ucType, dArgumentDependant, ulRespawnInterval, dblAmmo );
                if ( pPickup )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( ( CClientEntity* ) pPickup );
                    }

                    // Return the handle
                    lua_pushelement ( luaVM, pPickup );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPickupType ( lua_State* luaVM )
{
    CClientPickup* pPickup = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPickup );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pPickup )
        {
            unsigned char ucType = pPickup->m_ucType;
            lua_pushnumber ( luaVM, ucType );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPickupWeapon ( lua_State* luaVM )
{
    CClientPickup* pPickup = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPickup );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pPickup )
        {
            unsigned char ucWeapon = pPickup->m_ucWeaponType;
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucWeapon ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPickupAmount ( lua_State* luaVM )
{
    CClientPickup* pPickup = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPickup );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pPickup )
        {
            float fAmount = pPickup->m_fAmount;
            lua_pushnumber ( luaVM, fAmount );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPickupAmmo ( lua_State* luaVM )
{
    CClientPickup* pPickup = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPickup );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pPickup )
        {
            unsigned short usAmmo = pPickup->m_usAmmo;
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( usAmmo ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPickupType ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    double dblAmmo = 50.0;
    unsigned char ucType = 0;
    double dArgumentDependant = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( ucType );
    argStream.ReadNumber ( dArgumentDependant );
    argStream.ReadNumber ( dblAmmo, 50.0 );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetPickupType ( *pEntity, ucType, dArgumentDependant, dblAmmo ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


