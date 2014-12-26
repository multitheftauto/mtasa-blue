/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/oopdefs/CLuaOOPFunctionDefs.Vehicle.cpp
*  PURPOSE:     Lua OOP specific function definitions class
*  DEVELOPERS:  qaisjp <qaisjp@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaOOPDefs::DbQuery ( lua_State* luaVM )
{
    //  handle dbQuery ( [ function callbackFunction, [ table callbackArguments, ] ] element connection, string query, ... )
    CLuaFunctionRef iLuaFunction; CLuaArguments callbackArgs; CDatabaseConnectionElement* pElement; SString strQuery; CLuaArguments Args;

    CScriptArgReader argStream ( luaVM );

    argStream.ReadUserData ( pElement );
    if ( argStream.NextIsFunction () )
    {
        argStream.ReadFunction ( iLuaFunction );
        if ( argStream.NextIsTable () )
        {
            lua_pushnil ( luaVM );      // Loop through our table, beginning at the first key
            while ( lua_next ( luaVM, argStream.m_iIndex ) != 0 )
            {
                callbackArgs.ReadArgument ( luaVM, -1 );    // Ignore the index at -2, and just read the value
                lua_pop ( luaVM, 1 );                       // Remove the item and keep the key for the next iteration
            }
            argStream.m_iIndex++;
        }
    }
    argStream.ReadString ( strQuery );
    argStream.ReadLuaArguments ( Args );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        // Start async query
        CDbJobData* pJobData = g_pGame->GetDatabaseManager ()->QueryStart ( pElement->GetConnectionHandle (), strQuery, &Args );
        if ( !pJobData )
        {
            if ( !g_pGame->GetDatabaseManager ()->IsLastErrorSuppressed () )
                m_pScriptDebugging->LogWarning ( luaVM, "%s failed; %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *g_pGame->GetDatabaseManager ()->GetLastErrorMessage () );
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        // Make callback function if required
        if ( VERIFY_FUNCTION ( iLuaFunction ) )
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
                CLuaArguments Arguments;
                Arguments.PushDbQuery ( pJobData );
                Arguments.PushArguments ( callbackArgs );
                pJobData->SetCallback ( CLuaFunctionDefinitions::DbQueryCallback, g_pGame->GetLuaCallbackManager ()->CreateCallback ( pLuaMain, iLuaFunction, Arguments ) );
            }
        }
        // Add debug info incase query result does not get collected
        pJobData->SetLuaDebugInfo ( g_pGame->GetScriptDebugging ()->GetLuaDebugInfo ( luaVM ) );
        lua_pushquery ( luaVM, pJobData );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}