/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaClasses.h
*  PURPOSE:     Lua general class functions
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __LUACLASSES_H
#define __LUACLASSES_H

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

class CLuaClassDefs: public CLuaDefs
{
public:
    static int			Index               ( lua_State* luaVM );
    static int			NewIndex            ( lua_State* luaVM );
    static int			Call                ( lua_State* luaVM );

    static int			ReadOnly            ( lua_State* luaVM );
    static int			WriteOnly           ( lua_State* luaVM );

    static int			ToString            ( lua_State* luaVM );

    static const char*	GetObjectClass	    ( void* pObject );
    static const char*	GetResourceClass	( CResource* pResource );
    static const char*	GetTimerClass		( CLuaTimer* pTimer );
    static const char*	GetXmlNodeClass		( CXMLNode* pXmlNode );
    static const char*	GetEntityClass		( CClientEntity* pEntity );
};

#endif