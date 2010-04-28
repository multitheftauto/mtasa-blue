/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaCameraDefs.h
*  PURPOSE:     Lua camera function definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUACAMERADEFS_H
#define __CLUACAMERADEFS_H

#include "CLuaDefs.h"

class CLuaCameraDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );

    // Get functions
    static int      getCameraMatrix                     ( lua_State* luaVM );
    static int      getCameraTarget                     ( lua_State* luaVM );
    static int      getCameraInterior                   ( lua_State* luaVM );

    // Set functions
    static int      setCameraMatrix                     ( lua_State* luaVM );
    static int      setCameraTarget                     ( lua_State* luaVM );
    static int      setCameraInterior                   ( lua_State* luaVM );
    static int      fadeCamera                          ( lua_State* luaVM );  
};

#endif
