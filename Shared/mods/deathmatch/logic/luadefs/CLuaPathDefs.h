/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CLuaFileDefs.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "luadefs/CLuaDefs.h"

class CLuaPathDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

private:
    static std::optional<std::vector<std::string>> pathListDir(lua_State* luaVM, std::string path);
    
    static bool pathIsFile(lua_State* luaVM, std::string path);
    static bool pathIsDirectory(lua_State* luaVM, std::string path);
};
