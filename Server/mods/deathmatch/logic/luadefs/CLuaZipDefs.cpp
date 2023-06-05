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
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"zipCreate",   zipCreate},
        {"zipOpen",     zipOpen},
        {"zipClose",    ArgumentParser<zipClose>},
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

    //lua_classvariable(luaVM, "value", "xmlNodeSetValue", "xmlNodeGetValue");
    //lua_classvariable(luaVM, "attributes", NULL, "xmlNodeGetAttributes");

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

int CLuaZipDefs::zipClose(lua_State* luaVM)
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

    // Close the file and delete it
    pZip->Close();
    m_pElementDeleter->Delete(pZip);

    // Success. Return true
    lua_pushboolean(luaVM, true);
    return 1;
}

/*
bool CLuaZipDefs::zipClose(CZipFile* pZip)
{
    if (!pZip || !pZip->IsValid())
        return false;

    return pZip->Close();
}
*/
