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
//  bool addEvent ( string eventName [, bool allowRemoteTrigger = false ] )
    SString strName; bool bAllowRemoteTrigger;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadBool ( bAllowRemoteTrigger, false );

    if ( !argStream.HasErrors () )
    {
        // Grab our virtual machine
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Do it
            if ( CStaticFunctionDefinitions::AddEvent ( *pLuaMain, strName, bAllowRemoteTrigger ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "addEvent", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::AddEventHandler ( lua_State* luaVM )
{
//  bool addEventHandler ( string eventName, element attachedTo, function handlerFunction, [bool getPropagated = true] )
    SString strName; CClientEntity* pEntity; CLuaFunctionRef iLuaFunction; bool bPropagated;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadUserData ( pEntity );
    argStream.ReadFunction ( iLuaFunction );
    argStream.ReadBool ( bPropagated, true );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        // Grab our virtual machine
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Check if the handle is in use
            if ( pEntity->GetEventManager()->HandleExists ( pLuaMain, strName, iLuaFunction ) )
            {
                m_pScriptDebugging->LogCustom ( luaVM, 255, 0, 0, "addEventHandler: '%s' with this function is already handled", *strName );
                lua_pushboolean ( luaVM, false );
                return 1;
            }

            // Do it
            if ( CStaticFunctionDefinitions::AddEventHandler ( *pLuaMain, strName, *pEntity, iLuaFunction, bPropagated ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "addEventHandler", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemoveEventHandler ( lua_State* luaVM )
{
//  bool removeEventHandler ( string eventName, element attachedTo, function functionVar )
    SString strName; CClientEntity* pEntity; CLuaFunctionRef iLuaFunction;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadUserData ( pEntity );
    argStream.ReadFunction ( iLuaFunction );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        // Grab our virtual machine
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Do it
            if ( CStaticFunctionDefinitions::RemoveEventHandler ( *pLuaMain, strName, *pEntity, iLuaFunction ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "removeEventHandler", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::TriggerEvent ( lua_State* luaVM )
{
//  bool triggerEvent ( string eventName, element baseElement, [ var argument1, ... ] )
    SString strName; CClientEntity* pEntity; CLuaArguments Arguments;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadUserData ( pEntity );
    argStream.ReadLuaArguments ( Arguments );

    if ( !argStream.HasErrors () )
    {
        // Trigger it
        bool bWasCancelled;
        if ( CStaticFunctionDefinitions::TriggerEvent ( strName, *pEntity, Arguments, bWasCancelled ) )
        {
            lua_pushboolean ( luaVM, !bWasCancelled );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "triggerEvent", *argStream.GetErrorMessage () ) );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::TriggerServerEvent ( lua_State* luaVM )
{
//  bool triggerServerEvent ( string event, element theElement, [arguments...] )
    SString strName; CClientEntity* pCallWithEntity; CLuaArguments Arguments;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadUserData ( pCallWithEntity );
    argStream.ReadLuaArguments ( Arguments );

    if ( !argStream.HasErrors () )
    {
        // Trigger it
        if ( CStaticFunctionDefinitions::TriggerServerEvent ( strName, *pCallWithEntity, Arguments ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "triggerServerEvent", *argStream.GetErrorMessage () ) );

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



