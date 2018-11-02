/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaMain.Shared.cpp
 *
 *****************************************************************************/

#include "StdInc.h"

///////////////////////////////////////////////////////////////
//
// CLuaMain::InitPackageStorage
//
// Create a table for storage of Lua packages - stored in the Lua VM
//
///////////////////////////////////////////////////////////////
void CLuaMain::InitPackageStorage(lua_State* L)
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
    m_iPackageLoadedRef = luaL_ref(L, LUA_REGISTRYINDEX);              // stack: [tbl_new,"loaded"]
    lua_rawgeti(L, LUA_REGISTRYINDEX, m_iPackageLoadedRef);            // stack: [tbl_new,"loaded",tbl_new2]

    // Finally, store our original table as global package.loaded
    lua_settable(L, -3);                    // stack: [tbl_new]
    lua_setglobal(L, "package");            // stack: []
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::SetPackage
//
// Set the top most value as a package (No pop)
//
///////////////////////////////////////////////////////////////
void CLuaMain::SetPackage(lua_State* L, SString& strName)
{
    if (m_iPackageLoadedRef < 0)
        return;
    // We set varPkg, which is already on the stack, into package.loaded.moduleName = varPkg.
    // stack: [varPkg]
    int iPkg = luaL_ref(L, LUA_REGISTRYINDEX);                         // stack: []
    lua_rawgeti(L, LUA_REGISTRYINDEX, m_iPackageLoadedRef);            // [tbl_loaded]

    lua_pushstring(L, strName.c_str());                 // stack: [tbl_loaded,"moduleName"]
    lua_rawgeti(L, LUA_REGISTRYINDEX, iPkg);            // stack: [tbl_loaded,"moduleName",varPkg]
    lua_rawset(L, -3);                                  // stack: [tbl_loaded]
    lua_pop(L, 2);                                      // stack: []
    lua_rawgeti(L, LUA_REGISTRYINDEX, iPkg);            // stack: [varPkg]

    // Cleanup our used registry entry, i.e. REGISTRY[i] = nil.
    lua_pushnil(L);                                     // stack: [varPkg,nil]
    lua_rawseti(L, LUA_REGISTRYINDEX, iPkg);            // stack: [varPkg]
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::GetPackage
//
// Push the value of a package of name to the stack
//
///////////////////////////////////////////////////////////////
void CLuaMain::GetPackage(lua_State* L, SString& strName)
{
    if (m_iPackageLoadedRef < 0)
        return;

    lua_rawgeti(L, LUA_REGISTRYINDEX, m_iPackageLoadedRef);            // stack: [tbl_loaded]
    lua_pushstring(L, strName.c_str());                                // stack: [tbl_loaded,"moduleName"]
    lua_rawget(L, -2);                                                 // stack: [tbl_loaded,varPkg]
    lua_remove(L, -2);                                                 // stack: [varPkg]
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::LoadLuaLib
//
// Load a Lua lib of a given name
//
///////////////////////////////////////////////////////////////
bool CLuaMain::LoadLuaLib(lua_State* L, SString strName, SString& strError, bool& bEmpty)
{
    SString strPath = strName;
    // Name format shouldn't include slashes.  Subdirs are dots.
    ReplaceOccurrencesInString(strPath, "\\", "");
    ReplaceOccurrencesInString(strPath, "/", "");
    ReplaceOccurrencesInString(strPath, ".", "/");

#ifdef MTA_CLIENT
    SString strResPath = m_pResource->GetResourceDirectoryPath(ACCESS_PUBLIC, "");
#else
    SString strResPath = m_pResource->IsResourceZip() ? m_pResource->GetResourceCacheDirectoryPath() : m_pResource->GetResourceDirectoryPath();
#endif

    std::vector<char> buffer;
    strError = "could not load module '" + strName + "'. Not found in locations:\n\t";
    // Try <resource>/?.lua
    SString strFilePath = PathJoin(strResPath, strPath + ".lua");
    if (FileExists(strFilePath))
        FileLoad(strFilePath, buffer);
    else
    {
        // Don't use a format string for safety, so we construct the error by hand
        strError += "#1: " + ConformPath(strFilePath, "resources/") + "\n\t";

        // Try <resource>/?/init.lua
        strFilePath = PathJoin(strResPath, strPath, "init.lua");
        if (FileExists(strFilePath))
            FileLoad(strFilePath, buffer);
        else
        {
            strError += "#2: " + ConformPath(strFilePath, "resources/") + "\n\t";
            return false;
        }
    }

    if (buffer.size() > 0)
    {
        int luaSavedTop = lua_gettop(L);
        LoadScriptFromBuffer(&buffer.at(0), buffer.size(), strFilePath.c_str(), false);
        // Only keep the first return value
        if (lua_gettop(L) > luaSavedTop)
            lua_settop(L, luaSavedTop + 1);

        if (lua_type(L, -1) == LUA_TNIL)
        {
            strError += "#3: " + ConformPath(strFilePath, "modules/") + "\n\t";
            return false;
        }

        SetPackage(L, strName);            // Store our package into package.loaded
        return true;
    }
    else
    {
        strError = "could not load module '" + strName + "' from file " + ConformPath(strFilePath, "resources/") + ": File is empty.";
        bEmpty = true;
    }

    return false;
}
