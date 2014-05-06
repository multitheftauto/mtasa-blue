/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaClasses.cpp
*  PURPOSE:     Lua general class functions
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// Don't whine bout my gotos, lua api is a bitch, i had to!
int CLuaClassDefs::Index ( lua_State* luaVM )
{
    CElement* pElement = lua_toelement ( luaVM, 1 );

    if ( pElement )
    {
        lua_pushvalue ( luaVM, lua_upvalueindex ( 1 ) );

        // First we look for a function
        lua_pushstring ( luaVM, "__class" );
        lua_rawget ( luaVM, -2 );

        if ( !lua_istable ( luaVM, -1 ) )
        {
            lua_remove ( luaVM, -2 );
            goto searchparent;
        }

        lua_pushvalue ( luaVM, 2 );
        lua_rawget ( luaVM, -2 );
        lua_remove ( luaVM, -2 );

        if ( lua_isfunction ( luaVM, -1 ) )
        { // Found the function, clean up and return
            lua_remove ( luaVM, -2 );
            return 1;
        }
        lua_pop ( luaVM, 1 );

        // Function not found, look for property
        lua_pushstring ( luaVM, "__get" );
        lua_rawget ( luaVM, -2 );

        if ( !lua_istable ( luaVM, -1 ) )
        {
            lua_remove ( luaVM, -2 );
            goto searchparent;
        }

        lua_pushvalue ( luaVM, 2 );
        lua_rawget ( luaVM, -2 );
        lua_remove ( luaVM, -2 );

        if ( lua_isfunction ( luaVM, -1 ) )
        { // Found the property,
            lua_remove ( luaVM, -2 );

            lua_pushvalue ( luaVM, 1 );
            lua_call ( luaVM, 1, 1 );

            return 1;
        }
        lua_pop ( luaVM, 1 );

searchparent:
        lua_pushstring ( luaVM, "__parent" );
        lua_rawget ( luaVM, -2 );
        if ( lua_istable ( luaVM, -1 ) )
        {
            lua_pushstring ( luaVM, "__index" );
            lua_rawget ( luaVM, -2 );
            if ( lua_isfunction ( luaVM, -1 ) )
            {
                lua_pushvalue ( luaVM, 1 );
                lua_pushvalue ( luaVM, 2 );

                lua_call ( luaVM, 2, 1 );

                lua_replace ( luaVM, -2 );
                lua_pop ( luaVM, 2 );
                return 1;
            }
            lua_pop ( luaVM, 1 );
        }
        lua_pop ( luaVM, 2 );
    }

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaClassDefs::NewIndex ( lua_State* luaVM )
{
    CElement* pElement = lua_toelement ( luaVM, 1 );

    if ( pElement )
    {
        lua_pushvalue ( luaVM, lua_upvalueindex ( 1 ) );

        lua_pushstring ( luaVM, "__set" );
        lua_rawget ( luaVM, -2 );

        if ( !lua_istable ( luaVM, -1 ) )
        {
            lua_remove ( luaVM, -2 );
            goto searchparent;
        }

        lua_pushvalue ( luaVM, 2 );
        lua_rawget ( luaVM, -2 );
        lua_remove ( luaVM, -2 );

        if ( lua_isfunction ( luaVM, -1 ) )
        { // Found the property
            lua_pushvalue ( luaVM, 1 );
            lua_pushvalue ( luaVM, 3 );

            lua_call ( luaVM, 2, 0 );
            return 0;
        }

searchparent:
        lua_pushstring ( luaVM, "__parent" );
        lua_rawget ( luaVM, -2 );
        if ( lua_istable ( luaVM, -1 ) )
        {
            lua_pushstring ( luaVM, "__newindex" );
            lua_rawget ( luaVM, -2 );
            if ( lua_isfunction ( luaVM, -1 ) )
            {
                lua_pushvalue ( luaVM, 1 );
                lua_pushvalue ( luaVM, 2 );
                lua_pushvalue ( luaVM, 3 );

                lua_call ( luaVM, 3, 0 );

                lua_pop ( luaVM, 2 );
                return 0;
            }
            lua_pop ( luaVM, 1 );
        }
        lua_pop ( luaVM, 2 );
    }

    return 0;
}


int CLuaClassDefs::ReadOnly ( lua_State* luaVM )
{
    m_pScriptDebugging->LogWarning ( luaVM, "Property %s is read-only", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaClassDefs::WriteOnly ( lua_State* luaVM )
{
    m_pScriptDebugging->LogWarning ( luaVM, "Property %s is write-only", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );

    return 0;
}