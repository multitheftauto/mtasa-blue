/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CLuaFileDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#ifndef MTA_CLIENT
    // NOTE: Must be included before ILuaModuleManager.h which defines its own CChecksum type.
    #include "CChecksum.h"
#endif

#include "CLuaPathDefs.h"
#include "CScriptFile.h"
#include "CScriptArgReader.h"
#include <lua/CLuaFunctionParser.h>

void CLuaPathDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"pathListDir", ArgumentParser<pathListDir>},
        {"pathIsFile", ArgumentParser<pathIsFile>},
        {"pathIsDirectory", ArgumentParser<pathIsDirectory>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaPathDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "listDir", "pathListDir");
    lua_classfunction(luaVM, "isFile", "pathIsFile");
    lua_classfunction(luaVM, "isDirectory", "pathIsDirectory");

    lua_registerclass(luaVM, "path");
}

std::optional<std::vector<std::string>> CLuaPathDefs::pathListDir(lua_State* luaVM, std::string path)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return std::nullopt;

    SString strAbsPath;

    CResource* pResource = pLuaMain->GetResource();
    if (!CResourceManager::ParseResourcePathInput(path, pResource, &strAbsPath))
        return std::nullopt;

    if (!DirectoryExists(strAbsPath))
        return std::nullopt;

    return SharedUtil::ListDir(strAbsPath);
}

bool CLuaPathDefs::pathIsFile(lua_State* luaVM, std::string path)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return false;

    SString strAbsPath;

    CResource* pResource = pLuaMain->GetResource();
    if (!CResourceManager::ParseResourcePathInput(path, pResource, &strAbsPath))
        return false;

    return SharedUtil::FileExists(strAbsPath);
}

bool CLuaPathDefs::pathIsDirectory(lua_State* luaVM, std::string path)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return false;

    SString strAbsPath;

    CResource* pResource = pLuaMain->GetResource();
    if (!CResourceManager::ParseResourcePathInput(path, pResource, &strAbsPath))
        return false;

    return SharedUtil::DirectoryExists(strAbsPath);
}
