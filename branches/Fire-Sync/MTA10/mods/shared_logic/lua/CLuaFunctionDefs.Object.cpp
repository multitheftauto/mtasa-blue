/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Object.cpp
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

int CLuaFunctionDefs::CreateObject ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned short usModelID = static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) );

        if ( CClientObjectManager::IsValidModel ( usModelID ) )
        {
            int iArgument2 = lua_type ( luaVM, 2 );
            int iArgument3 = lua_type ( luaVM, 3 );
            int iArgument4 = lua_type ( luaVM, 4 );

            if ( ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
                ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
                ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
            {
                CVector vecPosition;
                vecPosition.fX = static_cast < float > ( atof ( lua_tostring ( luaVM, 2 ) ) );
                vecPosition.fY = static_cast < float > ( atof ( lua_tostring ( luaVM, 3 ) ) );
                vecPosition.fZ = static_cast < float > ( atof ( lua_tostring ( luaVM, 4 ) ) );

                CVector vecRotation;
                int iArgument5 = lua_type ( luaVM, 5 );

                if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
                {
                    vecRotation.fX = static_cast < float > ( atof ( lua_tostring ( luaVM, 5 ) ) );
                    int iArgument6 = lua_type ( luaVM, 6 );

                    if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
                    {
                        vecRotation.fY = static_cast < float > ( atof ( lua_tostring ( luaVM, 6 ) ) );
                        int iArgument7 = lua_type ( luaVM, 7 );

                        if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
                        {
                            vecRotation.fZ = static_cast < float > ( atof ( lua_tostring ( luaVM, 7 ) ) );
                        }
                    }
                }

                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
                if ( pLuaMain )
                {
                    CResource* pResource = pLuaMain->GetResource ();
                    if ( pResource )
                    {
                        CClientObject* pObject = CStaticFunctionDefinitions::CreateObject ( *pResource, usModelID, vecPosition, vecRotation );
                        if ( pObject )
                        {
                            CElementGroup * pGroup = pResource->GetElementGroup();
                            if ( pGroup )
                            {
                                pGroup->Add ( ( CClientEntity* ) pObject );
                            }

                            lua_pushelement ( luaVM, pObject );
                            return 1;
                        }
                    }
                }
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "createObject" );
        }
        else
            m_pScriptDebugging->LogWarning ( luaVM, "Bad model id passed to createObject" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createObject" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsObjectStatic ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientObject* pObject = lua_toobject ( luaVM, 1 );
        if ( pObject )
        {
            bool bStatic;
            if ( CStaticFunctionDefinitions::IsObjectStatic ( *pObject, bStatic ) )
            {
                lua_pushboolean ( luaVM, bStatic );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isObjectStatic", "object", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isObjectStatic" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::MoveObject ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );

    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CVector vecTargetPosition;
            CVector vecTargetRotation;
            unsigned long ulTime = static_cast < unsigned long > ( lua_tonumber ( luaVM, 2 ) );
            vecTargetPosition.fX = static_cast < float > ( atof ( lua_tostring ( luaVM, 3 ) ) );
            vecTargetPosition.fY = static_cast < float > ( atof ( lua_tostring ( luaVM, 4 ) ) );
            vecTargetPosition.fZ = static_cast < float > ( atof ( lua_tostring ( luaVM, 5 ) ) );

            int iArgument6 = lua_type ( luaVM, 6 );
            if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
            {
                vecTargetRotation.fX = static_cast < float > ( atof ( lua_tostring ( luaVM, 6 ) ) );

                int iArgument7 = lua_type ( luaVM, 7 );
                if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
                {
                    vecTargetRotation.fY = static_cast < float > ( atof ( lua_tostring ( luaVM, 7 ) ) );

                    int iArgument8 = lua_type ( luaVM, 8 );
                    if ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING )
                    {
                        vecTargetRotation.fZ = static_cast < float > ( atof ( lua_tostring ( luaVM, 8 ) ) );
                    }
                }
            }

            if ( CStaticFunctionDefinitions::MoveObject ( *pEntity, ulTime, vecTargetPosition, vecTargetRotation ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "moveObject", "object", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "moveObject" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::StopObject ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::StopObject ( *pEntity ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "stopObject", "object", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "stopObject" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetObjectScale ( lua_State* luaVM )
{
    int iArgumentType1 = lua_type ( luaVM, 1 );
    int iArgumentType2 = lua_type ( luaVM, 2 );

    if ( ( iArgumentType1 == LUA_TLIGHTUSERDATA ) &&
        ( iArgumentType2 == LUA_TNUMBER || iArgumentType2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            float fScale = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );

            if ( CStaticFunctionDefinitions::SetObjectScale ( *pEntity, fScale ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setObjectScale", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setObjectScale" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetObjectStatic ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
        lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            bool bStatic = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

            if ( CStaticFunctionDefinitions::SetObjectStatic ( *pEntity, bStatic ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setObjectStatic", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setObjectStatic" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


