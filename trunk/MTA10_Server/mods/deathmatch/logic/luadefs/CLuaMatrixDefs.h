/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CLuaVectorDefs.h
*  PURPOSE:     Lua general class functions
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAMATRIXDEFS_H
#define __CLUAMATRIXDEFS_H

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

class CLuaMatrixDefs: public CLuaDefs
{
public:
    static int      Create                          ( lua_State* luaVM );
    static int      Destroy                         ( lua_State* luaVM );

    static int      ToString                        ( lua_State* luaVM );

    static int      TransformDirection              ( lua_State* luaVM );
    static int      TransformPosition               ( lua_State* luaVM );
    static int      Inverse                         ( lua_State* luaVM );

    static int      GetPosition                     ( lua_State* luaVM );
    static int      GetRotation                     ( lua_State* luaVM );
    static int      GetForward                      ( lua_State* luaVM );
    static int      GetRight                        ( lua_State* luaVM );
    static int      GetUp                           ( lua_State* luaVM );

    static int      SetPosition                     ( lua_State* luaVM );
    static int      SetRotation                     ( lua_State* luaVM );
    static int      SetForward                      ( lua_State* luaVM );
    static int      SetRight                        ( lua_State* luaVM );
    static int      SetUp                           ( lua_State* luaVM );

    static int      Add                             ( lua_State* luaVM );
    static int      Sub                             ( lua_State* luaVM );
    static int      Mul                             ( lua_State* luaVM );
    static int      Div                             ( lua_State* luaVM );
};

#endif