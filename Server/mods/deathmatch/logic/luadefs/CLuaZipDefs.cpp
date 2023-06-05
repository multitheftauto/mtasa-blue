/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaZipDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaZipDefs.h"
#include "CScriptArgReader.h"

void CLuaZipDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]
    {
        {"zipCreate", zipCreate},
        {"zipOpen", zipOpen},
        {"zipClose", ArgumentParser<zipClose>},
        {"zipGetFiles", zipGetFiles},
        {"zipSize", zipSize},
        {"zipExtract", ArgumentParser<zipExtract>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaZipDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "open", "zipOpen");
    lua_classfunction(luaVM, "create", "zipCreate");
    lua_classfunction(luaVM, "close", "zipClose");
    lua_classfunction(luaVM, "extract", "zipExtract");
    lua_classfunction(luaVM, "getFiles", "zipGetFiles");

    lua_classvariable(luaVM, "files", NULL, "zipGetFiles");

    lua_registerclass(luaVM, "ZIP");
}

int CLuaZipDefs::zipCreate(lua_State* luaVM)
{
    SString strInputPath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strInputPath);
    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!pLuaMain)
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    CResource* pResource = pLuaMain->GetResource();
    SString    strAbsPath;
    SString    strMetaPath;

    if (!CResourceManager::ParseResourcePathInput(strInputPath, pResource, &strAbsPath, &strMetaPath))
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    CheckCanModifyOtherResource(argStream, pResource, pResource);
    CheckCanAccessOtherResourceFile(argStream, pResource, pResource, strAbsPath);
    if (argStream.HasErrors())
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    auto zipMode = FileExists(strAbsPath) ? CZipMaker::Mode::APPEND : CZipMaker::Mode::WRITE;

    MakeSureDirExists(strAbsPath);

    CZipFile* pZip = new CZipFile(pResource->GetScriptID(), strAbsPath, zipMode);
    if (!pZip->IsValid())
    {
        // Delete the file again
        pZip->Close();
        delete pZip;

        // Output error
        argStream.SetCustomError(SString("unable to load zip file '%s'", *strInputPath));
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }
    lua_pushelement(luaVM, pZip);
    return 1;
}

int CLuaZipDefs::zipOpen(lua_State* luaVM)
{
    SString strInputPath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strInputPath);
    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!pLuaMain)
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    CResource* pResource = pLuaMain->GetResource();
    SString    strAbsPath;
    SString    strMetaPath;

    if (!CResourceManager::ParseResourcePathInput(strInputPath, pResource, &strAbsPath, &strMetaPath))
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    CheckCanModifyOtherResource(argStream, pResource, pResource);
    CheckCanAccessOtherResourceFile(argStream, pResource, pResource, strAbsPath);
    if (argStream.HasErrors())
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    if (!FileExists(strAbsPath))
    {
        argStream.SetCustomError(SString("zip file '%s' doesn't exist", *strInputPath));
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }

    CZipFile* pZip = new CZipFile(pResource->GetScriptID(), strAbsPath, CZipMaker::Mode::READ);
    if (!pZip->IsValid())
    {
        // Delete the file again
        pZip->Close();
        delete pZip;

        // Output error
        argStream.SetCustomError(SString("unable to load zip file '%s'", *strInputPath));
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }
    lua_pushelement(luaVM, pZip);
    return 1;
}

bool CLuaZipDefs::zipClose(CZipFile* pZip)
{
    if (!pZip || !pZip->IsValid())
        return false;

    bool bStatus = pZip->Close();
    m_pElementDeleter->Delete(pZip);
    return bStatus;
}

int CLuaZipDefs::zipGetFiles(lua_State* luaVM)
{
    CZipFile* pZip;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pZip);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }

    lua_newtable(luaVM);
    auto files = pZip->ListFiles();
    for (auto it = files.begin(); it != files.end(); ++it)
    {
        lua_createtable(luaVM, 0, 5);

        lua_pushstring(luaVM, (*it).name.c_str());
        lua_setfield(luaVM, -2, "name");

        lua_pushnumber(luaVM, (*it).sizeCompressed);
        lua_setfield(luaVM, -2, "sizeCompressed");

        lua_pushnumber(luaVM, (*it).sizeUncompressed);
        lua_setfield(luaVM, -2, "sizeUncompressed");

        lua_pushboolean(luaVM, (*it).isDir);
        lua_setfield(luaVM, -2, "directory");

        lua_pushnumber(luaVM, (*it).crc32);
        lua_setfield(luaVM, -2, "crc32");

        lua_rawseti(luaVM, -2, it - files.begin() + 1);
    }

    return 1;
}

int CLuaZipDefs::zipExtract(lua_State* luaVM)
{
    CZipFile* pZip;
    SString   destPath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pZip);
    argStream.ReadString(destPath);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }

    if (!pZip || !pZip->IsValid())
    {
        lua_pushboolean(luaVM, false);
        return false;
    }

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!pLuaMain)
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    CResource* pResource = pLuaMain->GetResource();

    SString strAbsPath;
    SString strMetaPath;

    if (!CResourceManager::ParseResourcePathInput(destPath, pResource, &strAbsPath, &strMetaPath))
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    lua_pushboolean(luaVM, pZip->Extract(strAbsPath));

    return 1;
}

int CLuaZipDefs::zipSize(lua_State* luaVM)
{
    CZipFile* pZip;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pZip);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }
    if (!pZip->IsValid())
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }
    
    lua_pushnumber(luaVM, pZip->ListFiles().size());
    return 1;
}
