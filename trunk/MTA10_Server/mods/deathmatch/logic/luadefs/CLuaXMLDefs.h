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

#ifndef __CLUAXMLDEFS_H
#define __CLUAXMLDEFS_H

#include "CLuaDefs.h"

class CLuaXMLDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );

    static int      xmlCreateFile                       ( lua_State* luaVM );
    static int      xmlLoadFile                         ( lua_State* luaVM );
    static int      xmlCopyFile                         ( lua_State* luaVM );
    static int      xmlSaveFile                         ( lua_State* luaVM );
    static int      xmlUnloadFile                       ( lua_State* luaVM );

    static int		xmlCreateChild		    			( lua_State* luaVM );
    static int      xmlDestroyNode                      ( lua_State* luaVM );
    static int      xmlNodeFindChild                    ( lua_State* luaVM );
    static int      xmlNodeGetChildren                  ( lua_State* luaVM );
    static int      xmlNodeGetParent                    ( lua_State* luaVM );

    static int      xmlNodeGetValue                     ( lua_State* luaVM );
    static int      xmlNodeSetValue                     ( lua_State* luaVM );
	static int      xmlNodeGetAttributes                ( lua_State* luaVM );
    static int      xmlNodeGetAttribute                 ( lua_State* luaVM );
    static int      xmlNodeSetAttribute                 ( lua_State* luaVM );

	static int      xmlNodeGetName		                ( lua_State* luaVM );
	static int      xmlNodeSetName		                ( lua_State* luaVM );
};

#endif
