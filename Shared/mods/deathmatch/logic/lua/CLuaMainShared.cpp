/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaMainShared.cpp
 *
 *****************************************************************************/

#include "CLuaMainShared.h"
#include "StdInc.h"

///////////////////////////////////////////////////////////////
//
// CLuaMain::InitPackageStorage
//
// Create a table for storage of Lua packages - stored in the Lua VM
//
///////////////////////////////////////////////////////////////
void CLuaMainShared::InitPackageStorage(CLuaMain* pLuaMain, lua_State* L)
{
    lua_newtable(L);                        // stack: [tbl_new]
    lua_pushstring(L, "loaded");            // stack: [tbl_new,"loaded"]
    lua_newtable(L);                        // stack: [tbl_new,"loaded",tbl_new2]

    // We push our default Lua libs are loaded packages
    std::vector<const char*> szDefaultLibs = {"math", "string", "table", "debug", "utf8"};
    for (auto it : szDefaultLibs)
    {
        lua_pushstring(L, it);            // stack: [tbl_new,"loaded",tbl_new2,"math"]
        lua_getglobal(L, it);             // stack: [tbl_new,"loaded",tbl_new2,"math",_G.math]
        lua_settable(L, -3);              // stack: [tbl_new,"loaded",tbl_new2]
    }

    // Keep our package.loaded table safely in registry
    int iPackageLoadedRef = luaL_ref(L, LUA_REGISTRYINDEX);              // stack: [tbl_new,"loaded"]
    lua_rawgeti(L, LUA_REGISTRYINDEX, iPackageLoadedRef);            // stack: [tbl_new,"loaded",tbl_new2]

    // Store our reference to the package object
    pLuaMain->m_iPackageLoadedRef = iPackageLoadedRef;

    // Finally, store our original table as global package.loaded
    lua_settable(L, -3);                    // stack: [tbl_new]
    lua_setglobal(L, "package");            // stack: []
}
