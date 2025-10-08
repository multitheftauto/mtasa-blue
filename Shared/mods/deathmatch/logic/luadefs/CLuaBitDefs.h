/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CLuaBitDefs.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "luadefs/CLuaDefs.h"

class CLuaBitDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    // bitwise operators
    LUA_DECLARE(bitAnd);
    LUA_DECLARE(bitNot);
    LUA_DECLARE(bitOr);
    LUA_DECLARE(bitTest);
    LUA_DECLARE(bitXor);

    LUA_DECLARE(bitLRotate);
    LUA_DECLARE(bitRRotate);
    LUA_DECLARE(bitLShift);
    LUA_DECLARE(bitRShift);
    LUA_DECLARE(bitArShift);

    // utilities
    LUA_DECLARE(bitExtract);
    LUA_DECLARE(bitReplace);
};
