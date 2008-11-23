/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaPickupDefs.cpp
*  PURPOSE:     Lua pickup definitions class
*  DEVELOPERS:  Jax <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaPickupDefs::LoadFunctions ( void )
{
     // Create/destroy
    CLuaCFunctions::AddFunction ( "createPickup", CLuaPickupDefs::createPickup );

    // Get
    CLuaCFunctions::AddFunction ( "getPickupType", CLuaPickupDefs::getPickupType );
    CLuaCFunctions::AddFunction ( "getPickupWeapon", CLuaPickupDefs::getPickupWeapon );
    CLuaCFunctions::AddFunction ( "getPickupAmount", CLuaPickupDefs::getPickupAmount );
    CLuaCFunctions::AddFunction ( "getPickupAmmo", CLuaPickupDefs::getPickupAmmo );
    CLuaCFunctions::AddFunction ( "getPickupRespawnInterval", CLuaPickupDefs::getPickupRespawnInterval );
    CLuaCFunctions::AddFunction ( "isPickupSpawned", CLuaPickupDefs::isPickupSpawned );

    // Set
    CLuaCFunctions::AddFunction ( "setPickupType", CLuaPickupDefs::setPickupType );
    CLuaCFunctions::AddFunction ( "setPickupRespawnInterval", CLuaPickupDefs::setPickupRespawnInterval );
    CLuaCFunctions::AddFunction ( "usePickup", CLuaPickupDefs::usePickup );
}


int CLuaPickupDefs::createPickup ( lua_State* luaVM )
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
			CResource* pResource = pLuaMain->GetResource();
			if ( pResource )
			{
				CPickup* pPickup = CStaticFunctionDefinitions::CreatePickup ( 
										pResource,
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
						pGroup->Add ( pPickup );
					}
					// Return the handle
					lua_pushelement ( luaVM, pPickup );
					return 1;
				}
			}
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createPickup" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::getPickupType ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPickup* pPickup = lua_topickup ( luaVM, 1 );
        if ( pPickup )
        {
            unsigned char ucType;
            if ( CStaticFunctionDefinitions::GetPickupType ( pPickup, ucType ) )
            {
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucType ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPickupType", "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPickupType" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::getPickupWeapon ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPickup* pPickup = lua_topickup ( luaVM, 1 );
        if ( pPickup )
        {
            unsigned char ucWeapon;
            if ( CStaticFunctionDefinitions::GetPickupWeapon ( pPickup, ucWeapon ) )
            {
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucWeapon ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPickupWeapon", "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPickupWeapon" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::getPickupAmount ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPickup* pPickup = lua_topickup ( luaVM, 1 );
        if ( pPickup )
        {
            float fAmount;
            if ( CStaticFunctionDefinitions::GetPickupAmount ( pPickup, fAmount ) )
            {
                lua_pushnumber ( luaVM, fAmount );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPickupAmount", "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPickupAmount" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::getPickupAmmo ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPickup* pPickup = lua_topickup ( luaVM, 1 );
        if ( pPickup )
        {
            unsigned short usAmmo;
            if ( CStaticFunctionDefinitions::GetPickupAmmo ( pPickup, usAmmo ) )
            {
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( usAmmo ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPickupAmmo", "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPickupAmmo" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::getPickupRespawnInterval ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPickup* pPickup = lua_topickup ( luaVM, 1 );
        if ( pPickup )
        {
            unsigned long ulInterval;
            if ( CStaticFunctionDefinitions::GetPickupRespawnInterval ( pPickup, ulInterval ) )
            {
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( ulInterval ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPickupRespawnInterval", "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPickupRespawnInterval" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::isPickupSpawned ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPickup* pPickup = lua_topickup ( luaVM, 1 );
        if ( pPickup )
        {
            bool bSpawned;
            if ( CStaticFunctionDefinitions::IsPickupSpawned ( pPickup, bSpawned ) )
            {
                lua_pushboolean ( luaVM, bSpawned );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPickupSpawned", "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPickupSpawned" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::setPickupType ( lua_State* luaVM )
{
    // The first 2 are always numeric saying weapon/health/armor
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetPickupType ( pElement, static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) ), lua_tonumber ( luaVM, 3 ), lua_tonumber ( luaVM, 4 ) ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPickupType", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPickupType" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::setPickupRespawnInterval ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            unsigned long ulInterval = static_cast < unsigned long > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetPickupRespawnInterval ( pElement, ulInterval ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPickupRespawnInterval", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPickupRespawnInterval" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::usePickup ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CPlayer * pPlayer = lua_toplayer ( luaVM, 2 );
            if ( pPlayer )
            {
                // Do it
                if ( CStaticFunctionDefinitions::UsePickup ( pElement, pPlayer ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "usePickup", "player", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "usePickup", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "usePickup" );

    lua_pushboolean ( luaVM, false );
    return 1;
}