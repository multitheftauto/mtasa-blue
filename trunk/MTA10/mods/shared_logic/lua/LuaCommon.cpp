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
            CResource * pResource = g_pClientGame->GetResourceManager ( )->GetResourceFromLuaState ( luaVM );
            if ( pResource->IsOOPEnabled ( ) )
            {
                switch ( pElement->GetType() )
                {
                case CCLIENTPLAYER:
                    lua_pushuserdata ( luaVM, "Player", (void*) reinterpret_cast<unsigned int *>(ID.Value()) );
                    break;
                case CCLIENTPED:
                    lua_pushuserdata ( luaVM, "Ped", (void*) reinterpret_cast<unsigned int *>(ID.Value()) );
                    break;
                case CCLIENTVEHICLE:
                    lua_pushuserdata ( luaVM, "Vehicle", (void*) reinterpret_cast<unsigned int *>(ID.Value()) );
                    break;
                default:
                    lua_pushuserdata ( luaVM, "Element", (void*) reinterpret_cast<unsigned int *>(ID.Value()) );
                    break;
                }
            }
            else
            {
                lua_pushlightuserdata ( luaVM, (void*) reinterpret_cast<unsigned int *>(ID.Value()) );
            }
            return;
        }
    }

    lua_pushnil ( luaVM );
}

CClientEntity* lua_toelement ( lua_State* luaVM, int iArgument )
{
    if ( lua_type ( luaVM, iArgument ) == LUA_TLIGHTUSERDATA )
    {
        ElementID ID = TO_ELEMENTID ( lua_touserdata ( luaVM, iArgument ) );
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        if ( !pEntity || pEntity->IsBeingDeleted () )
            return NULL;
        return pEntity;
    }
    else if ( lua_type ( luaVM, iArgument ) == LUA_TUSERDATA )
    {
        ElementID ID = TO_ELEMENTID ( * ( ( void ** ) lua_touserdata ( luaVM, iArgument ) ) );
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        if ( !pEntity || pEntity->IsBeingDeleted () )
            return NULL;
        return pEntity;
    }

    return NULL;
}

void lua_pushresource ( lua_State* luaVM, CResource* pResource )
{
    CResource * pSourceResource = g_pClientGame->GetResourceManager ( )->GetResourceFromLuaState ( luaVM );
    if ( pSourceResource->IsOOPEnabled ( ) )
    {
        lua_pushuserdata ( luaVM, "Unknown",  reinterpret_cast < void* > ( pResource->GetScriptID () ) );
    }
    else
    {
        lua_pushlightuserdata ( luaVM, (void*) reinterpret_cast<unsigned int *>( pResource->GetScriptID () ) );
    }
}

void lua_pushtimer ( lua_State* luaVM, CLuaTimer* pTimer )
{
    CResource * pResource = g_pClientGame->GetResourceManager ( )->GetResourceFromLuaState ( luaVM );
    if ( pResource->IsOOPEnabled ( ) )
    {
        lua_pushuserdata ( luaVM, "Unknown",  reinterpret_cast < void* > ( pTimer->GetScriptID () ) );
    }
    else
    {
        lua_pushlightuserdata ( luaVM, (void*) reinterpret_cast<unsigned int *>( pTimer->GetScriptID () ) );
    }
}

void lua_pushxmlnode ( lua_State* luaVM, CXMLNode* pElement )
{
    unsigned long ulID = pElement->GetID ();
    CResource * pResource = g_pClientGame->GetResourceManager ( )->GetResourceFromLuaState ( luaVM );
    if ( pResource->IsOOPEnabled ( ) )
    {
        lua_pushuserdata ( luaVM, "Unknown",  reinterpret_cast < void* > ( ulID ) );
    }
    else
    {
        lua_pushlightuserdata ( luaVM, (void*) reinterpret_cast<unsigned int *>( ulID ) );
    }
}

void lua_pushuserdata ( lua_State* luaVM, const char* szClass, void* value )
{
    lua_pushstring ( luaVM, "ud" );
    lua_rawget ( luaVM, LUA_REGISTRYINDEX );

    // First we want to check if we have a userdata for this already
    lua_pushlightuserdata ( luaVM, value );
    lua_rawget ( luaVM, -2 );

    if ( lua_isnil ( luaVM, -1 ) )
    {
        lua_pop ( luaVM, 1 );

        // we don't have it, create it
        * ( void ** ) lua_newuserdata ( luaVM, sizeof ( void * ) ) = value;

        // save in ud table
        lua_pushlightuserdata ( luaVM, value );
        lua_pushvalue ( luaVM, -2 );
        lua_rawset ( luaVM, -4 );
    }

    // userdata is already on the stack, just remove the table
    lua_remove ( luaVM, -2 );

    // Assign the class metatable
    lua_pushstring ( luaVM, "mt" ); // element, "mt"
    lua_rawget ( luaVM, LUA_REGISTRYINDEX ); // element, mt

    lua_pushstring ( luaVM, szClass ); // element, mt, class name
    lua_rawget ( luaVM, -2 ); // element, mt, class

    lua_remove ( luaVM, -2 ); // element, class
    lua_setmetatable ( luaVM, -2 ); // element
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


void lua_newclass ( lua_State* luaVM )
{
    lua_newtable ( luaVM );

    lua_pushstring ( luaVM, "__class" );
    lua_newtable ( luaVM );
    lua_rawset ( luaVM, -3 );

    lua_pushstring ( luaVM, "__get" );
    lua_newtable ( luaVM );
    lua_rawset ( luaVM, -3 );

    lua_pushstring ( luaVM, "__set" );
    lua_newtable ( luaVM );
    lua_rawset ( luaVM, -3 );

    lua_pushstring ( luaVM, "__index" );
    lua_pushvalue ( luaVM, -2 );
    lua_pushcclosure ( luaVM, CLuaClassDefs::Index, 1 );
    lua_rawset ( luaVM, -3 );

    lua_pushstring ( luaVM, "__newindex" );
    lua_pushvalue ( luaVM, -2 );
    lua_pushcclosure ( luaVM, CLuaClassDefs::NewIndex, 1 );
    lua_rawset ( luaVM, -3 );
}


void lua_registerclass ( lua_State* luaVM, const char* szName, const char* szParent )
{
    if ( szParent != NULL )
    {
        lua_pushstring ( luaVM, "mt" ); // class table, "mt"
        lua_rawget ( luaVM, LUA_REGISTRYINDEX ); // class table, mt table
        lua_getfield ( luaVM, -1, szParent ); // class table, mt table, parent table

        assert ( lua_istable ( luaVM, -1 ) );

        lua_setfield ( luaVM, -3, "__parent" ); // class table, mt table

        lua_pop ( luaVM, 1 ); // class table
    }

    lua_pushstring ( luaVM, "mt" );
    lua_rawget ( luaVM, LUA_REGISTRYINDEX );

    // store in registry
    lua_pushvalue ( luaVM, -2 );
    lua_setfield ( luaVM, -2, szName );

    lua_pop ( luaVM, 1 );

    // register with environment
    lua_getfield ( luaVM, -1, "__class" );
    lua_setglobal ( luaVM, szName );

    lua_pop ( luaVM, 1 );
}

void lua_classfunction ( lua_State* luaVM, const char* szFunction, lua_CFunction fn )
{
    if ( fn )
    {
        lua_pushstring ( luaVM, "__class" );
        lua_rawget ( luaVM, -2 );

        lua_pushstring ( luaVM, szFunction );
        lua_pushstring ( luaVM, szFunction );
        lua_pushcclosure ( luaVM, fn, 1 );
        lua_rawset ( luaVM, -3 );

        lua_pop ( luaVM, 1 );
    }
}

void lua_classfunction ( lua_State* luaVM, const char* szFunction, const char* fn )
{
    CLuaCFunction* pFunction = CLuaCFunctions::GetFunction ( fn );
    if ( pFunction )
    {
        lua_classfunction ( luaVM, szFunction, pFunction->GetFunctionAddress () );
    }
}

void lua_classvariable ( lua_State* luaVM, const char* szVariable, lua_CFunction set, lua_CFunction get )
{
    lua_pushstring ( luaVM, "__set" );
    lua_rawget ( luaVM, -2 );

    if ( !set )
    {
        lua_pushstring ( luaVM, szVariable );
        lua_pushstring ( luaVM, szVariable );
        lua_pushcclosure ( luaVM, CLuaClassDefs::ReadOnly, 1 );
        lua_rawset ( luaVM, -3 );
    }
    else
    {
        lua_pushstring ( luaVM, szVariable );
        lua_pushstring ( luaVM, szVariable );
        lua_pushcclosure ( luaVM, set, 1 );
        lua_rawset ( luaVM, -3 );
    }
    lua_pop ( luaVM, 1 );

    // Get
    lua_pushstring ( luaVM, "__get" );
    lua_rawget ( luaVM, -2 );

    if ( !get )
    {
        lua_pushstring ( luaVM, szVariable );
        lua_pushstring ( luaVM, szVariable );
        lua_pushcclosure ( luaVM, CLuaClassDefs::WriteOnly, 1 );
        lua_rawset ( luaVM, -3 );
    }
    else
    {
        lua_pushstring ( luaVM, szVariable );
        lua_pushstring ( luaVM, szVariable );
        lua_pushcclosure ( luaVM, get, 1 );
        lua_rawset ( luaVM, -3 );
    }
    lua_pop ( luaVM, 1 );
}

void lua_classvariable ( lua_State* luaVM, const char* szVariable, const char* set, const char* get )
{
    lua_CFunction fnSet;
    lua_CFunction fnGet;

    if ( set )
        if ( CLuaCFunction* pSet = CLuaCFunctions::GetFunction ( set ) )
            fnSet = pSet->GetFunctionAddress ();

    if ( get )
        if ( CLuaCFunction* pGet = CLuaCFunctions::GetFunction ( get ) )
            fnGet = pGet->GetFunctionAddress ();

    if ( fnSet || fnGet )
        lua_classvariable ( luaVM, szVariable, fnSet, fnGet );
}