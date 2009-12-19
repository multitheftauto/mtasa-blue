/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Projectile.cpp
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

int CLuaFunctionDefs::CreateProjectile ( lua_State* luaVM )
{
    if ( ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) ) &&
        ( lua_istype ( luaVM, 2, LUA_TNUMBER ) || lua_istype ( luaVM, 2, LUA_TSTRING ) ) )
    {
        CClientEntity* pCreator = lua_toelement ( luaVM, 1 );
        if ( pCreator )
        {
            unsigned char ucWeaponType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            CVector vecOrigin;
            pCreator->GetPosition ( vecOrigin );
            float fForce = 1.0f;
            CClientEntity* pTarget = NULL;
            CVector *pvecRotation = NULL, *pvecMoveSpeed = NULL;
            unsigned short usModel = 0;
            if ( ( lua_istype ( luaVM, 3, LUA_TNUMBER ) || lua_istype ( luaVM, 3, LUA_TSTRING ) ) &&
                ( lua_istype ( luaVM, 4, LUA_TNUMBER ) || lua_istype ( luaVM, 4, LUA_TSTRING ) ) &&
                ( lua_istype ( luaVM, 5, LUA_TNUMBER ) || lua_istype ( luaVM, 5, LUA_TSTRING ) ) )
            {
                vecOrigin = CVector ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                    static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                    static_cast < float > ( lua_tonumber ( luaVM, 5 ) ) );

                if ( lua_istype ( luaVM, 6, LUA_TNUMBER ) || lua_istype ( luaVM, 6, LUA_TSTRING ) )
                {
                    fForce = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );

                    if ( lua_istype ( luaVM, 7, LUA_TLIGHTUSERDATA ) )
                    {
                        CClientEntity* pTemp = lua_toelement ( luaVM, 7 );
                        if ( pTemp )
                        {
                            pTarget = pTemp;
                        }
                        else
                            m_pScriptDebugging->LogBadPointer ( luaVM, "createProjectile", "element", 7 );
                    }

                    int iArgument8 = lua_type ( luaVM, 8 );
                    int iArgument9 = lua_type ( luaVM, 9 );
                    int iArgument10 = lua_type ( luaVM, 10 );
                    if ( ( iArgument8 == LUA_TSTRING || iArgument8 == LUA_TNUMBER ) &&
                        ( iArgument9 == LUA_TSTRING || iArgument9 == LUA_TNUMBER ) &&
                        ( iArgument10 == LUA_TSTRING || iArgument10 == LUA_TNUMBER ) )
                    {
                        pvecRotation = new CVector ( static_cast < float > ( lua_tonumber ( luaVM, 8 ) ),
                            static_cast < float > ( lua_tonumber ( luaVM, 9 ) ),
                            static_cast < float > ( lua_tonumber ( luaVM, 10 ) ) );
                    }
                    int iArgument11 = lua_type ( luaVM, 11 );
                    int iArgument12 = lua_type ( luaVM, 12 );
                    int iArgument13 = lua_type ( luaVM, 13 );
                    if ( ( iArgument11 == LUA_TSTRING || iArgument11 == LUA_TNUMBER ) &&
                        ( iArgument12 == LUA_TSTRING || iArgument12 == LUA_TNUMBER ) &&
                        ( iArgument13 == LUA_TSTRING || iArgument13 == LUA_TNUMBER ) )
                    {
                        pvecMoveSpeed = new CVector ( static_cast < float > ( lua_tonumber ( luaVM, 11 ) ),
                            static_cast < float > ( lua_tonumber ( luaVM, 12 ) ),
                            static_cast < float > ( lua_tonumber ( luaVM, 13 ) ) );

                        int iArgument14 = lua_type ( luaVM, 14 );
                        if ( iArgument14 == LUA_TSTRING || iArgument14 == LUA_TNUMBER )
                        {
                            usModel = static_cast < unsigned short > ( lua_tonumber ( luaVM, 14 ) );
                        }
                    }
                }
            }

            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
                CResource * pResource = pLuaMain->GetResource();
                if ( pResource )
                {
                    CClientProjectile * pProjectile = CStaticFunctionDefinitions::CreateProjectile ( *pResource, *pCreator, ucWeaponType, vecOrigin, fForce, pTarget, pvecRotation, pvecMoveSpeed, usModel );
                    if ( pProjectile )
                    {
                        CElementGroup * pGroup = pResource->GetElementGroup();
                        if ( pGroup )
                        {
                            pGroup->Add ( ( CClientEntity* ) pProjectile );
                        }

                        if ( pvecRotation )
                        {
                            delete pvecRotation;
                            pvecRotation = NULL;
                        }
                        if ( pvecMoveSpeed )
                        {
                            delete pvecMoveSpeed;
                            pvecMoveSpeed = NULL;
                        }

                        lua_pushelement ( luaVM, pProjectile );
                        return 1;
                    }
                }
            }
            if ( pvecRotation )
            {
                delete pvecRotation;
                pvecRotation = NULL;
            }
            if ( pvecMoveSpeed )
            {
                delete pvecMoveSpeed;
                pvecMoveSpeed = NULL;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "createProjectile", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createProjectile" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetProjectileType ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientProjectile* pProjectile = lua_toprojectile ( luaVM, 1 );
        if ( pProjectile )
        {
            unsigned char ucWeapon = pProjectile->GetWeaponType();
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucWeapon ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getProjectileType", "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getProjectileType" );

    lua_pushboolean ( luaVM, false );
    return 1;
}




