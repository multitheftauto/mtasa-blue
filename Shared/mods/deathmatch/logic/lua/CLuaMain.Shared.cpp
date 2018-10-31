/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaMainShared.cpp
 *
 *****************************************************************************/

#include "StdInc.h"

///////////////////////////////////////////////////////////////
//
// CLuaMain::LoadLuaLib
//
// Load a Lua lib of a given name
//
///////////////////////////////////////////////////////////////
bool CLuaMain::LoadLuaLib(lua_State* L, SString strName, SString& strError)
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
    // Try <resource>/?.lua
    SString strFilePath = PathJoin(strResPath, strPath + ".lua");
    if (FileExists(strFilePath))
        FileLoad(strFilePath, buffer);
    else
    {
        // Don't use a format string for safety, so we construct the error by hand
        strError += "error loading module " + strName + " from file " + strFilePath + ":\n\t The specified module could not be found";

        // Try <resource>/?/init.lua
        strFilePath = PathJoin(strResPath, strPath, "init.lua");
        if (FileExists(strFilePath))
            FileLoad(strFilePath, buffer);
        else
        {
            strError += "\n";
            strError += "error loading module " + strName + " from file " + strFilePath + ":\n\t The specified module could not be found";
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
            strError += "error loading module " + strName + " from file " + strFilePath + ":\n\t Module didn't return a value";
            return false;
        }

        SetPackage(L, strName);            // Store our package into package.loaded
        return true;
    }
    else
        strError += "error loading module " + strName + " from file " + strFilePath + ":\n\t Error loading script file";

    return false;
}
