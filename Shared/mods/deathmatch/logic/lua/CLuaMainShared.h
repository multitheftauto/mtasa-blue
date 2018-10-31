/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaMainShared.h
 *
 *****************************************************************************/
#pragma once

#include "lua/LuaCommon.h"
#include "lua/CLuaMain.h"

//class CLuaMain;

class CLuaMainShared
{
public:
    static void InitPackageStorage(CLuaMain* pLuaMain, lua_State* L);            // Create a psuedo package.loaded table

};
