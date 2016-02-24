/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaXMLDefs.h
*  PURPOSE:     Lua XML function definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <>
*               Kevin Whiteside <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaXMLDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );
    static void     AddClass                            ( lua_State* luaVM );


    LUA_DECLARE ( xmlCreateFile );
    LUA_DECLARE ( xmlLoadFile );
    LUA_DECLARE ( xmlCopyFile );
    LUA_DECLARE ( xmlSaveFile );
    LUA_DECLARE ( xmlUnloadFile );

    LUA_DECLARE ( xmlCreateChild );
    LUA_DECLARE ( xmlDestroyNode );
    LUA_DECLARE ( xmlNodeFindChild );
    LUA_DECLARE ( xmlNodeGetChildren );
    LUA_DECLARE ( xmlNodeGetParent );

    LUA_DECLARE ( xmlNodeGetValue );
    LUA_DECLARE ( xmlNodeSetValue );
    LUA_DECLARE ( xmlNodeGetAttributes );
    LUA_DECLARE ( xmlNodeGetAttribute );
    LUA_DECLARE ( xmlNodeSetAttribute );

    LUA_DECLARE ( xmlNodeGetName );
    LUA_DECLARE ( xmlNodeSetName );
};