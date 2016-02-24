/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaFileDefs.h
*  PURPOSE:     Lua file definitions class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaFileDefs: public CLuaDefs
{
public:
    static void LoadFunctions ( void );
    static void AddClass ( lua_State* luaVM );

    LUA_DECLARE(fileCreate)
    LUA_DECLARE(fileExists)
    LUA_DECLARE(fileOpen)
    LUA_DECLARE(fileIsEOF)
    LUA_DECLARE(fileGetPos)
    LUA_DECLARE(fileSetPos)
    LUA_DECLARE(fileGetSize)
    LUA_DECLARE(fileRead)
    LUA_DECLARE(fileWrite)
    LUA_DECLARE(fileFlush)
    LUA_DECLARE(fileDelete)
    LUA_DECLARE(fileRename)
    LUA_DECLARE(fileCopy)
    LUA_DECLARE(fileGetPath)
    
    LUA_DECLARE ( fileClose );
    LUA_DECLARE ( fileCloseGC );
};