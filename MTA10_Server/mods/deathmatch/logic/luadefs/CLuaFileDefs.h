/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaFileDefs.h
*  PURPOSE:     Lua file handling definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               lil_Toady <>
*               Cecill Etheredge <>
*               Florian Busse <flobu@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAFILEDEFS_H
#define __CLUAFILEDEFS_H

#include "CLuaDefs.h"

class CLuaFileDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );

    static int      fileCreate                          ( lua_State* luaVM );
    static int      fileExists                          ( lua_State* luaVM );
    static int      fileOpen                            ( lua_State* luaVM );
    static int      fileIsEOF                           ( lua_State* luaVM );
    static int      fileGetPos                          ( lua_State* luaVM );
    static int      fileSetPos                          ( lua_State* luaVM );
    static int      fileGetSize                         ( lua_State* luaVM );
    static int      fileRead                            ( lua_State* luaVM );
    static int      fileWrite                           ( lua_State* luaVM );
    static int      fileFlush                           ( lua_State* luaVM );
    static int      fileClose                           ( lua_State* luaVM );
	static int      fileDelete                          ( lua_State* luaVM );
};

#endif
