/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Event.cpp
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

int CLuaFunctionDefs::AddEvent ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Verify the arguments
        if ( lua_istype ( luaVM, 1, LUA_TSTRING )  )
        {
            // Grab the arguments
            const char* szName = lua_tostring ( luaVM, 1 );

            // Remote trigger?
            bool bAllowRemoteTrigger = false;
            if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
            {
                bAllowRemoteTrigger = lua_toboolean ( luaVM, 2 ) ? true:false;
            }

            // Do it
            if ( CStaticFunctionDefinitions::AddEvent ( *pLuaMain, szName, bAllowRemoteTrigger ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::AddEventHandler ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Verify the arguments
        if ( lua_istype ( luaVM, 1, LUA_TSTRING ) &&
            lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) &&
            lua_istype ( luaVM, 3, LUA_TFUNCTION ) )
        {
            // Grab propagated flag if any
            bool bPropagated = true;
            if ( lua_istype ( luaVM, 4, LUA_TBOOLEAN ) )
                bPropagated = ( lua_toboolean ( luaVM, 4 ) ) ? true:false;

            // Grab the arguments
            const char* szName = lua_tostring ( luaVM, 1 );
            CClientEntity* pEntity = lua_toelement ( luaVM, 2 );
            int iLuaFunction = luaM_toref ( luaVM, 3 );

            // Verify the element
            if ( pEntity )
            {
                // Verify the function
                if ( VERIFY_FUNCTION ( iLuaFunction ) )
                {
                    // Check if the handle is in use
                    if ( pEntity->GetEventManager()->HandleExists ( pLuaMain, szName, iLuaFunction ) )
                    {
                        m_pScriptDebugging->LogCustom ( luaVM, 255, 0, 0, "addEventHandler: %s is already handled", szName );
                        lua_pushboolean ( luaVM, false );
                        return 1;
                    }

                    // Do it
                    if ( CStaticFunctionDefinitions::AddEventHandler ( *pLuaMain, const_cast < char * > ( szName ), *pEntity, iLuaFunction, bPropagated ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "addEventHandler", "function", 3 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "addEventHandler", "element", 2 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "addEventHandler" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemoveEventHandler ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Verify the arguments
        if ( lua_istype ( luaVM, 1, LUA_TSTRING ) &&
            lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) &&
            lua_istype ( luaVM, 3, LUA_TFUNCTION ) )
        {
            // Grab the arguments
            const char* szName = lua_tostring ( luaVM, 1 );
            CClientEntity* pEntity = lua_toelement ( luaVM, 2 );
            int iLuaFunction = luaM_toref ( luaVM, 3 );

            // Verify the element
            if ( pEntity )
            {
                // Verify the function
                if ( VERIFY_FUNCTION ( iLuaFunction ) )
                {
                    // Do it
                    if ( CStaticFunctionDefinitions::RemoveEventHandler ( *pLuaMain, const_cast < char * > ( szName ), *pEntity, iLuaFunction ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "removeEventHandler", "function", 3 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "removeEventHandler", "element", 2 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "removeEventHandler" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::TriggerEvent ( lua_State* luaVM )
{
    // Verify the arguments
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) &&
        lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the name and the element
        const char* szName = lua_tostring ( luaVM, 1 );
        CClientEntity* pEntity = lua_toelement ( luaVM, 2 );

        // Read out the additional arguments to pass to the event
        CLuaArguments Arguments;
        Arguments.ReadArguments ( luaVM, 3 );

        // Verify the element
        if ( pEntity )
        {
            // Trigger it
            bool bWasCancelled;
            if ( CStaticFunctionDefinitions::TriggerEvent ( szName, *pEntity, Arguments, bWasCancelled ) )
            {
                lua_pushboolean ( luaVM, !bWasCancelled );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "triggerEvent", "element", 2 );
    }

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::TriggerServerEvent ( lua_State* luaVM )
{
    // Verify argument types
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) &&
        lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) )
    {
        // Grab arguments
        const char* szName = lua_tostring ( luaVM, 1 );
        CClientEntity* pCallWithEntity = lua_toelement ( luaVM, 2 );
        CLuaArguments Arguments;
        Arguments.ReadArguments ( luaVM, 3 );

        // Check entity
        if ( pCallWithEntity )
        {
            // Trigger it
            if ( CStaticFunctionDefinitions::TriggerServerEvent ( szName, *pCallWithEntity, Arguments ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "triggerServerEvent", "element", 2 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "triggerServerEvent" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CancelEvent ( lua_State* luaVM )
{
    // Cancel it
    if ( CStaticFunctionDefinitions::CancelEvent ( true ) )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::WasEventCancelled ( lua_State* luaVM )
{
    // Return whether the last event was cancelled or not
    lua_pushboolean ( luaVM, CStaticFunctionDefinitions::WasEventCancelled () );
    return 1;
}



