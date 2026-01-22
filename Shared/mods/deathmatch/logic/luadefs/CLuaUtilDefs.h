/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaUtilDefs.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "luadefs/CLuaDefs.h"

class CLuaUtilDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    // Reserved functions
    LUA_DECLARE(DisabledFunction);

    // Util functions to make scripting easier for the end user
    // Some of these are based on standard mIRC script funcs as a lot of people will be used to them
    LUA_DECLARE(Dereference);
    LUA_DECLARE(Reference);
    LUA_DECLARE(GetTickCount_);            // Trailing underscore to prevent macro issues
    LUA_DECLARE(GetCTime);
    LUA_DECLARE(Split);
    LUA_DECLARE(IsOOPEnabled);
    LUA_DECLARE(GetUserdataType);
    LUA_DECLARE(luaB_print);
    LUA_DECLARE(GetColorFromString);

    // Utility vector math functions
    LUA_DECLARE(GetDistanceBetweenPoints2D);
    LUA_DECLARE(GetDistanceBetweenPoints3D);
    LUA_DECLARE(GetEasingValue);
    LUA_DECLARE(InterpolateBetween);

    // JSON funcs
    LUA_DECLARE(toJSON);
    LUA_DECLARE(fromJSON);

    // PCRE functions
    LUA_DECLARE(PregFind);
    LUA_DECLARE(PregReplace);
    LUA_DECLARE(PregMatch);

    // Debug functions
    LUA_DECLARE(DebugSleep);

    // Utility functions
    LUA_DECLARE(GetTok);
    LUA_DECLARE(tocolor);
};
