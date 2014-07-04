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
    static int      fileRename                          ( lua_State* luaVM );
    static int      fileCopy                            ( lua_State* luaVM );
};

#endif
