/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaSharedXMLDefs.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "luadefs/CLuaDefs.h"
#include "../CZipFile.h"

class CLuaZipDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(zipCreate);
    LUA_DECLARE(zipOpen);
    static bool zipClose(CZipFile* pFile);
    LUA_DECLARE(zipGetFiles);
    LUA_DECLARE(zipExtract);
    LUA_DECLARE(zipSize);
};
