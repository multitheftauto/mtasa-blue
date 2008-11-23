/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaHTTPDefs.h
*  PURPOSE:     Lua HTTP webserver definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAHTTPDEFS_H
#define __CLUAHTTPDEFS_H

#include "CLuaDefs.h"

// This class is for functions are only available to HTTPD scripts.

class CLuaHTTPDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( lua_State* luaVM );

    static int      httpWrite                           ( lua_State* luaVM );
	static int		httpSetResponseHeader				( lua_State* luaVM );
	static int		httpSetResponseCookie				( lua_State* luaVM );
	static int		httpSetResponseCode					( lua_State* luaVM );
	static int		httpClear							( lua_State* luaVM );
	static int		httpRequestLogin					( lua_State* luaVM );
};

#endif
