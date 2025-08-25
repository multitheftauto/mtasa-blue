/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CLuaTranslationDefs.cpp
 *  PURPOSE:     Shared Lua translation definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaTranslationDefs.h"
#include "CScriptArgReader.h"
#include "CResourceTranslationManager.h"
#include "CGlobalTranslationManager.h"
#include "CResource.h"
#include "CGame.h"

void CLuaTranslationDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"getTranslation", GetTranslation},
        {"getAvailableTranslations", GetAvailableTranslations},
        {"getGlobalTranslationProviders", GetGlobalTranslationProviders},
        {"isResourceGlobalTranslationProvider", IsResourceGlobalTranslationProvider},
        {"getResourceGlobalTranslationProviders", GetResourceGlobalTranslationProviders},
    };

    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}



int CLuaTranslationDefs::GetTranslation(lua_State* luaVM)
{
    SString msgid;
    SString language;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(msgid);
    argStream.ReadString(language, "");

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            CResource* resource = luaMain->GetResource();
            if (resource)
            {
                if (resource->GetTranslationManager())
                {
                    std::string result = resource->GetTranslationManager()->GetTranslation(msgid, language);
                    lua_pushstring(luaVM, result.c_str());
                    return 1;
                }
                else
                {
                    m_pScriptDebugging->LogWarning(luaVM, "Translation system not initialized for resource '%s' when requesting '%s'", resource->GetName().c_str(), msgid.c_str());
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushstring(luaVM, msgid.c_str());
    return 1;
}

int CLuaTranslationDefs::GetAvailableTranslations(lua_State* luaVM)
{
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
    {
        lua_newtable(luaVM);
        return 1;
    }

    CResource* resource = luaMain->GetResource();
    if (!resource || !resource->GetTranslationManager())
    {
        if (resource)
            m_pScriptDebugging->LogWarning(luaVM, "Translation system not initialized for resource '%s'", resource->GetName().c_str());
        
        lua_newtable(luaVM);
        return 1;
    }

    const std::vector<std::string> languages = resource->GetTranslationManager()->GetAvailableLanguages();
    
    lua_newtable(luaVM);
    for (size_t i = 0; i < languages.size(); ++i)
    {
        lua_pushinteger(luaVM, static_cast<int>(i + 1));
        lua_pushstring(luaVM, languages[i].c_str());
        lua_settable(luaVM, -3);
    }
    return 1;
}

int CLuaTranslationDefs::GetGlobalTranslationProviders(lua_State* luaVM)
{
    const std::vector<std::string> providers = CGlobalTranslationManager::GetSingleton().GetAvailableProviders();
    
    lua_newtable(luaVM);
    for (size_t i = 0; i < providers.size(); ++i)
    {
        lua_pushinteger(luaVM, static_cast<int>(i + 1));
        lua_pushstring(luaVM, providers[i].c_str());
        lua_settable(luaVM, -3);
    }
    
    return 1;
}

int CLuaTranslationDefs::IsResourceGlobalTranslationProvider(lua_State* luaVM)
{
    CResource* resource;
    
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);
    
    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }

    if (!resource || !resource->GetTranslationManager())
    {
        m_pScriptDebugging->LogWarning(luaVM, "Translation system not initialized for resource '%s'", resource ? resource->GetName().c_str() : "unknown");
        lua_pushboolean(luaVM, false);
        return 1;
    }

    const bool isProvider = resource->GetTranslationManager()->IsGlobalProvider();
    lua_pushboolean(luaVM, isProvider);
    return 1;
}

int CLuaTranslationDefs::GetResourceGlobalTranslationProviders(lua_State* luaVM)
{
    CResource* resource = nullptr;
    
    CScriptArgReader argStream(luaVM);
    
    if (argStream.NextIsUserData())
    {
        argStream.ReadUserData(resource);
    }
    else
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
            resource = luaMain->GetResource();
    }
    
    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_newtable(luaVM);
        return 1;
    }

    if (!resource || !resource->GetTranslationManager())
    {
        m_pScriptDebugging->LogWarning(luaVM, "Translation system not initialized for resource '%s'", resource ? resource->GetName().c_str() : "unknown");
        lua_newtable(luaVM);
        return 1;
    }

    const std::vector<std::string> providers = resource->GetTranslationManager()->GetGlobalProviders();
    
    lua_newtable(luaVM);
    for (size_t i = 0; i < providers.size(); ++i)
    {
        lua_pushinteger(luaVM, static_cast<int>(i + 1));
        lua_pushstring(luaVM, providers[i].c_str());
        lua_settable(luaVM, -3);
    }
    
    return 1;
}
