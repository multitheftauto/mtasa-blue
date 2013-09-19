/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaTaskDefs.cpp
*  PURPOSE:     Lua task definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

void CLuaTaskDefs::LoadFunctions ( void )
{
    // ChrML: Disabled for dp3
    /*
    CLuaCFunctions::AddFunction ( "createTaskInstance", CLuaTaskDefs::createTaskInstance );

    CLuaCFunctions::AddFunction ( "getTaskName", CLuaTaskDefs::getTaskName );

    CLuaCFunctions::AddFunction ( "getTaskParameter", CLuaTaskDefs::getTaskParameter );
    CLuaCFunctions::AddFunction ( "getTaskParameters", CLuaTaskDefs::getTaskParameters );
    CLuaCFunctions::AddFunction ( "setTaskParameters", CLuaTaskDefs::setTaskParameters );
    CLuaCFunctions::AddFunction ( "clearTaskParameters", CLuaTaskDefs::clearTaskParameters );

    CLuaCFunctions::AddFunction ( "runTaskFunction", CLuaTaskDefs::runTaskFunction );

    CLuaCFunctions::AddFunction ( "setPlayerTask", CLuaTaskDefs::setPlayerTask );
    */
}


int CLuaTaskDefs::createTaskInstance ( lua_State* luaVM )
{
    // taskinstance createTaskInstance ( string taskname, table parameters )
    // returns taskinstance or false on failure

    // Verify types
    if ( argtype ( 1, LUA_TSTRING ) &&
         argtype ( 2, LUA_TTABLE ) )
    {
        // Grab the task name
        CClientTask Task ( m_pManager );
        const char* szTaskName = lua_tostring ( luaVM, 1 );
        Task.SetTaskName ( szTaskName );

        // Generate an unique identifier
        Task.SetUniqueIdentifier ( CClientTask::GenerateUniqueIdentifier () );

        // Read out the task parameters
        if ( Task.ReadParameters ( luaVM, 2, true ) )
        {
            // Just return the task data as a table
            lua_newtable ( luaVM );
            Task.Write ( luaVM, -1 );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTaskDefs::getTaskName ( lua_State* luaVM )
{
    // string getTaskName ( taskinstance task )
    // returns a string or false on failure

    // Verify types
    if ( argtype ( 1, LUA_TTABLE ) )
    {
        // Read out the task data
        CClientTask Task ( m_pManager );
        if ( Task.Read ( luaVM, 1, true ) )
        {
            // Return it
            lua_pushstring ( luaVM, Task.GetTaskName () );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTaskDefs::getTaskParameter ( lua_State* luaVM )
{
    // string getTaskParameter ( taskinstance task, string key )
    // returns a string or false on failure

    // Verify types
    if ( argtype ( 1, LUA_TTABLE ) &&
         argtype ( 2, LUA_TSTRING ) )
    {
        // Read out the task data
        CClientTask Task ( m_pManager );
        if ( Task.Read ( luaVM, 1, true ) )
        {
            // Read out the key string
            const char* szKey = lua_tostring ( luaVM, 2 );

            // Grab the parameter
            CLuaArgument* pValue = Task.GetParameter ( szKey );
            if ( pValue )
            {
                // Return it
                pValue->Push ( luaVM );
                return 1;
            }
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTaskDefs::getTaskParameters ( lua_State* luaVM )
{
    // table getTaskParameters ( taskinstance task )
    // returns a table or false on failure

    // Verify types
    if ( argtype ( 1, LUA_TTABLE ) )
    {
        // Read out the task data
        CClientTask Task ( m_pManager );
        if ( Task.Read ( luaVM, 1, true ) )
        {
            // Write the parameters and return
            lua_newtable ( luaVM );
            Task.WriteParameters ( luaVM, -1 );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTaskDefs::setTaskParameters ( lua_State* luaVM )
{
    // bool setTaskParameters ( taskinstance task, table newparameters )
    // returns true on success or false on failure

    // Verify types
    if ( argtype ( 1, LUA_TTABLE ) &&
         argtype ( 2, LUA_TTABLE ) )
    {
        // Read out the task data
        CClientTask Task ( m_pManager );
        if ( Task.Read ( luaVM, 1, true ) )
        {
            // Read the new parameters into it in addition to the old
            Task.ReadParameters ( luaVM, 2, false );

            // Write them back to the table
            Task.Write ( luaVM, 1 );

            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTaskDefs::clearTaskParameters ( lua_State* luaVM )
{
    // bool clearTaskParameters ( taskinstance task )
    // returns true on success or false on failure

    // Verify types
    if ( argtype ( 1, LUA_TTABLE ) )
    {
        // Read out the task data
        CClientTask Task ( m_pManager );
        if ( Task.Read ( luaVM, 1, true ) )
        {
            // Read the new parameters into it in addition to the old
            // and write them back into the table.
            Task.ClearParameters ();
            Task.Write ( luaVM, 1 );

            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTaskDefs::runTaskFunction ( lua_State* luaVM )
{
    return 0;
}


int CLuaTaskDefs::getPlayerTaskInstance ( lua_State* luaVM )
{
    return 0;
}


int CLuaTaskDefs::setPlayerTask ( lua_State* luaVM )
{
    // bool setPlayerTask ( ped thePed, taskinstance task )
    // returns true on success or false on failure

    // Verify types
    if ( argtype ( 1, LUA_TLIGHTUSERDATA ) &&
         argtype ( 2, LUA_TTABLE ) )
    {
        // Grab the player
        // TODO: Support peds too
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Player?
            if ( pEntity->GetType () == CCLIENTPLAYER )
            {
                // Grab the player
                CClientPlayer* pPlayer = static_cast < CClientPlayer* > ( pEntity );

                // Read out the task data
                CClientTask Task ( m_pManager );
                if ( Task.Read ( luaVM, 2, true ) )
                {
                    // Apply it on the player
                    bool bSuccess = Task.ApplyTask ( *pPlayer );

                    // Success
                    lua_pushboolean ( luaVM, bSuccess );
                    return 1;
                }
            }
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}
