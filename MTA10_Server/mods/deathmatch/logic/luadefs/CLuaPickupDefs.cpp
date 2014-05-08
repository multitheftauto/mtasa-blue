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
//  pickup createPickup ( float x, float y, float z, int theType, int amount/weapon/model, [ int respawnTime = 30000, int ammo = 50 ] )
    CVector vecPosition; unsigned char ucType; double dValue; unsigned long ulRespawnInterval; double dAmmo;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( ucType );
    argStream.ReadNumber ( dValue );
    argStream.ReadNumber ( ulRespawnInterval, 30000 );
    argStream.ReadNumber ( dAmmo, 50.0 );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                CPickup* pPickup = CStaticFunctionDefinitions::CreatePickup ( pResource, vecPosition, ucType, dValue, ulRespawnInterval, dAmmo );
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::getPickupType ( lua_State* luaVM )
{
//  int getPickupType ( pickup thePickup )
    CPickup* pPickup;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPickup );
    
    if ( !argStream.HasErrors () )
    {
        unsigned char ucType;
        if ( CStaticFunctionDefinitions::GetPickupType ( pPickup, ucType ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucType ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::getPickupWeapon ( lua_State* luaVM )
{
//  int getPickupWeapon ( pickup thePickup )
    CPickup* pPickup;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPickup );
    
    if ( !argStream.HasErrors () )
    {
        unsigned char ucWeapon;
        if ( CStaticFunctionDefinitions::GetPickupWeapon ( pPickup, ucWeapon ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucWeapon ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::getPickupAmount ( lua_State* luaVM )
{
//  int getPickupAmount ( pickup thePickup )
    CPickup* pPickup;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPickup );
    
    if ( !argStream.HasErrors () )
    {
        float fAmount;
        if ( CStaticFunctionDefinitions::GetPickupAmount ( pPickup, fAmount ) )
        {
            lua_pushnumber ( luaVM, fAmount );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::getPickupAmmo ( lua_State* luaVM )
{
//  int getPickupAmmo ( pickup thePickup )
    CPickup* pPickup;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPickup );
    
    if ( !argStream.HasErrors () )
    {
        unsigned short usAmmo;
        if ( CStaticFunctionDefinitions::GetPickupAmmo ( pPickup, usAmmo ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( usAmmo ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::getPickupRespawnInterval ( lua_State* luaVM )
{
//  int getPickupRespawnInterval ( pickup thePickup )
    CPickup* pPickup;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPickup );
    
    if ( !argStream.HasErrors () )
    {
        unsigned long ulInterval;
        if ( CStaticFunctionDefinitions::GetPickupRespawnInterval ( pPickup, ulInterval ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( ulInterval ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::isPickupSpawned ( lua_State* luaVM )
{
//  bool isPickupSpawned ( pickup thePickup )
    CPickup* pPickup;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPickup );
    
    if ( !argStream.HasErrors () )
    {
        bool bSpawned;
        if ( CStaticFunctionDefinitions::IsPickupSpawned ( pPickup, bSpawned ) )
        {
            lua_pushboolean ( luaVM, bSpawned );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPickupDefs::setPickupType ( lua_State* luaVM )
{
//  bool setPickupType ( pickup thePickup, int theType, int amount/weapon/model, [ int ammo ] )
    CElement* pElement; unsigned char ucType; double dValue; double dAmmo;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucType );
    argStream.ReadNumber ( dValue );
    argStream.ReadNumber ( dAmmo, 0.0 );
    
    if ( !argStream.HasErrors () )
    {
        // Do it
        if ( CStaticFunctionDefinitions::SetPickupType ( pElement, ucType, dValue, dAmmo ) )
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


int CLuaPickupDefs::setPickupRespawnInterval ( lua_State* luaVM )
{
//  bool setPickupRespawnInterval ( pickup thePickup, int ms )
    CElement* pElement; unsigned long ulInterval;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ulInterval );
    
    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPickupRespawnInterval ( pElement, ulInterval ) )
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


int CLuaPickupDefs::usePickup ( lua_State* luaVM )
{
//  bool usePickup ( pickup thePickup, player thePlayer )
    CElement* pElement; CPlayer* pPlayer;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadUserData ( pPlayer );
    
    if ( !argStream.HasErrors () )
    {
        // Do it
        if ( CStaticFunctionDefinitions::UsePickup ( pElement, pPlayer ) )
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
