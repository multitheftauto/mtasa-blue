/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/LuaCommon.cpp
*  PURPOSE:     Lua common functions
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

// Temporary until we change these funcs:
#include "../luadefs/CLuaDefs.h"
// End of temporary

// Prevent the warning issued when doing unsigned short -> void*
#pragma warning(disable:4312)

#pragma message(__LOC__"Use RTTI/dynamic_casting here for safety?")

void lua_pushelement ( lua_State* luaVM, CClientEntity* pElement )
{
    if ( pElement )
    {
        ElementID ID = pElement->GetID ();
        if ( ID != INVALID_ELEMENT_ID )
        {
            lua_pushlightuserdata ( luaVM, (void*) reinterpret_cast<unsigned int *>(ID.Value()) );
            return;
        }
    }

    lua_pushnil ( luaVM );
}

void lua_pushresource ( lua_State* luaVM, CResource* pResource )
{
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( pResource->GetScriptID () ) );
}

void lua_pushtimer ( lua_State* luaVM, CLuaTimer* pTimer )
{
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( pTimer->GetScriptID () ) );
}

void lua_pushxmlnode ( lua_State* luaVM, CXMLNode* pElement )
{
    unsigned long ulID = pElement->GetID ();
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( ulID ) );
}


// Just do a type check vs LUA_TNONE before calling this, or bant
const char* lua_makestring ( lua_State* luaVM, int iArgument )
{
    if ( lua_type ( luaVM, iArgument ) == LUA_TSTRING )
    {
        return lua_tostring ( luaVM, iArgument );
    }
    lua_pushvalue ( luaVM, iArgument );
    lua_getglobal ( luaVM, "tostring" );
    lua_pushvalue ( luaVM, -2 );
    lua_call ( luaVM, 1, 1 );

    const char* szString = lua_tostring ( luaVM, -1 );
    lua_pop ( luaVM, 2 );

    return szString;
}
