/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaBitDefs.h
*  PURPOSE:     Lua bit definitions class header
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaBitDefs : public CLuaDefs
{
public:
    static void LoadFunctions ( void );

    LUA_DECLARE ( BitAnd );
    LUA_DECLARE ( BitNot );
    LUA_DECLARE ( BitOr );
    LUA_DECLARE ( BitTest );
    LUA_DECLARE ( BitXor );
    LUA_DECLARE ( BitLRotate );
    LUA_DECLARE ( BitRRotate );
    LUA_DECLARE ( BitLShift );
    LUA_DECLARE ( BitRShift );
    LUA_DECLARE ( BitArShift );
    LUA_DECLARE ( BitExtract );
    LUA_DECLARE ( BitReplace );
};