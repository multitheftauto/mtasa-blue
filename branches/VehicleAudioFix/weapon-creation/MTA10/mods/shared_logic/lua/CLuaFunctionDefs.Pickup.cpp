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
    // Grab all the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    int iArgument7 = lua_type ( luaVM, 7 );

    // The first 6 are always numeric saying position, type and weapon/health/armor
    // TODO: Check argument 7 incase type is weapon
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
        ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING || iArgument6 == LUA_TNONE ) &&
        ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING || iArgument7 == LUA_TNONE ) )
    {
        // Populate a position vector for it
        CVector vecPosition = CVector ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        // Is the type health or armor?
        unsigned long ulRespawnInterval = 30000;
        double dblAmmo = 50.0;
        if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
            ulRespawnInterval = static_cast < unsigned long > ( lua_tonumber ( luaVM, 6 ) );

        if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
            dblAmmo = lua_tonumber ( luaVM, 7 );

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                CClientPickup* pPickup = CStaticFunctionDefinitions::CreatePickup ( 
                    *pResource, 
                    vecPosition, 
                    static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) ), 
                    lua_tonumber ( luaVM, 5 ), 
                    ulRespawnInterval, 
                    dblAmmo );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPickupType ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientPickup* pPickup = lua_topickup ( luaVM, 1 );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPickupWeapon ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientPickup* pPickup = lua_topickup ( luaVM, 1 );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPickupAmount ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientPickup* pPickup = lua_topickup ( luaVM, 1 );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPickupAmmo ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientPickup* pPickup = lua_topickup ( luaVM, 1 );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPickupType ( lua_State* luaVM )
{
    // The first 2 are always numeric saying weapon/health/armor
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetPickupType ( *pEntity, static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) ), lua_tonumber ( luaVM, 3 ), lua_tonumber ( luaVM, 4 ) ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


