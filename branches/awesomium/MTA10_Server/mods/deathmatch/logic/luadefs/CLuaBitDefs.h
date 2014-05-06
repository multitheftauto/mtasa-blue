/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaBitDefs.h
*  PURPOSE:     Lua bitwise operators definitions class
*  DEVELOPERS:  Jusonex <jusonex96@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUABITDEFS_H
#define __CLUABITDEFS_H

#include "CLuaDefs.h"

class CLuaBitDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );

    // bitwise operators
    static int      bitAnd                              ( lua_State* luaVM );
    static int      bitNot                              ( lua_State* luaVM );
    static int      bitOr                               ( lua_State* luaVM );
    static int      bitTest                             ( lua_State* luaVM );
    static int      bitXor                              ( lua_State* luaVM );

    static int      bitLRotate                          ( lua_State* luaVM );
    static int      bitRRotate                          ( lua_State* luaVM );
    static int      bitLShift                           ( lua_State* luaVM );
    static int      bitRShift                           ( lua_State* luaVM );
    static int      bitArShift                          ( lua_State* luaVM );

    // utilities
    static int      bitExtract                          ( lua_State* luaVM );
    static int      bitReplace                          ( lua_State* luaVM );

};

#endif
