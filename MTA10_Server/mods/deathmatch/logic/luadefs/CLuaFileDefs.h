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

#pragma once
#include "CLuaDefs.h"

class CLuaFileDefs: public CLuaDefs
{
public:
    static void LoadFunctions ( void );
    static void AddClass      ( lua_State* luaVM );

    LUA_DECLARE ( fileCreate );
    LUA_DECLARE ( fileExists );
    LUA_DECLARE ( fileOpen );
    LUA_DECLARE ( fileIsEOF );
    LUA_DECLARE ( fileGetPos );
    LUA_DECLARE ( fileSetPos );
    LUA_DECLARE ( fileGetSize );
    LUA_DECLARE ( fileRead );
    LUA_DECLARE ( fileWrite );
    LUA_DECLARE ( fileFlush );
    LUA_DECLARE ( fileDelete );
    LUA_DECLARE ( fileRename );
    LUA_DECLARE ( fileCopy );
    LUA_DECLARE ( fileGetPath );
    
    LUA_DECLARE ( fileClose );
    LUA_DECLARE ( fileCloseGC );
};