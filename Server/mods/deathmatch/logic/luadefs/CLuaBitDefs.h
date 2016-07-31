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

#pragma once
#include "CLuaDefs.h"

class CLuaBitDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );

    // bitwise operators
    LUA_DECLARE ( bitAnd );
    LUA_DECLARE ( bitNot );
    LUA_DECLARE ( bitOr );
    LUA_DECLARE ( bitTest );
    LUA_DECLARE ( bitXor );

    LUA_DECLARE ( bitLRotate );
    LUA_DECLARE ( bitRRotate );
    LUA_DECLARE ( bitLShift );
    LUA_DECLARE ( bitRShift );
    LUA_DECLARE ( bitArShift );

    // utilities
    LUA_DECLARE ( bitExtract );
    LUA_DECLARE ( bitReplace );

};