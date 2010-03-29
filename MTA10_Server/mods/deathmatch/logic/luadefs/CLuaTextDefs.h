/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaTextDefs.h
*  PURPOSE:     Lua text display function definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Peter <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUATEXTDEFS_H
#define __CLUATEXTDEFS_H

#include "CLuaDefs.h"

class CLuaTextDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );

    static int      textCreateDisplay                   ( lua_State* luaVM );
    static int      textDestroyDisplay                  ( lua_State* luaVM );
    static int      textCreateTextItem                  ( lua_State* luaVM );
    static int      textDestroyTextItem                 ( lua_State* luaVM );

    static int      textDisplayAddText                  ( lua_State* luaVM );
    static int      textDisplayRemoveText               ( lua_State* luaVM );
    static int      textDisplayAddObserver              ( lua_State* luaVM );
    static int      textDisplayRemoveObserver           ( lua_State* luaVM );
    static int      textDisplayIsObserver               ( lua_State* luaVM );
    static int      textDisplayGetObservers             ( lua_State* luaVM );

    static int      textItemSetText                     ( lua_State* luaVM );
    static int      textItemGetText                     ( lua_State* luaVM );
    static int      textItemSetScale                    ( lua_State* luaVM );
    static int      textItemGetScale                    ( lua_State* luaVM );
    static int      textItemSetPosition                 ( lua_State* luaVM );
    static int      textItemGetPosition                 ( lua_State* luaVM );
    static int      textItemSetColor                    ( lua_State* luaVM );
    static int      textItemGetColor                    ( lua_State* luaVM );
    static int      textItemSetPriority                 ( lua_State* luaVM );
    static int      textItemGetPriority                 ( lua_State* luaVM );
};

#endif
