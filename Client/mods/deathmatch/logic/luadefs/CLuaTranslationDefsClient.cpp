/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/luadefs/CLuaTranslationDefsClient.cpp
 *  PURPOSE:     Client-only Lua translation definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaTranslationDefsClient.h"
#include "CScriptArgReader.h"
#include "CResourceTranslationManager.h"
#include "net/Packets.h"

void CLuaTranslationDefsClient::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"setCurrentTranslationLanguage", SetCurrentTranslationLanguage},
        {"getCurrentTranslationLanguage", GetCurrentTranslationLanguage},
        {"getTranslation", GetTranslation},
        {"getAvailableTranslations", GetAvailableTranslations},
    };

    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

int CLuaTranslationDefsClient::SetCurrentTranslationLanguage(lua_State* luaVM)
{
    SString language;
    
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(language);
    
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
                    std::string oldLanguage = resource->GetTranslationManager()->GetClientLanguage();
                    resource->GetTranslationManager()->SetClientLanguage(language);
                    std::string newLanguage = resource->GetTranslationManager()->GetClientLanguage();
                    
                    if (oldLanguage != newLanguage)
                    {
                        CLuaArguments args;
                        args.PushString(oldLanguage);
                        args.PushString(newLanguage);
                        
                        if (g_pClientGame->GetLocalPlayer())
                        {
                            g_pClientGame->GetLocalPlayer()->CallEvent("onClientTranslationLanguageChange", args, true);
                        }
                    }
                    
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
                else
                {
                    m_pScriptDebugging->LogError(luaVM, "Translation system not initialized for resource '%s'", resource->GetName());
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaTranslationDefsClient::GetCurrentTranslationLanguage(lua_State* luaVM)
{
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (luaMain)
    {
        CResource* resource = luaMain->GetResource();
        if (resource)
        {
            if (resource->GetTranslationManager())
            {
                std::string currentLanguage = resource->GetTranslationManager()->GetClientLanguage();
                lua_pushstring(luaVM, currentLanguage.c_str());
                return 1;
            }
            else
            {
                m_pScriptDebugging->LogError(luaVM, "Translation system not initialized for resource '%s'", resource->GetName());
            }
        }
    }

    lua_pushstring(luaVM, "");
    return 1;
}

int CLuaTranslationDefsClient::GetTranslation(lua_State* luaVM)
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
                    // If no language specified, use current client language
                    std::string targetLanguage = language.empty() ? 
                        resource->GetTranslationManager()->GetClientLanguage() : language;
                    std::string result = resource->GetTranslationManager()->GetTranslation(msgid, targetLanguage);
                    lua_pushstring(luaVM, result.c_str());
                    return 1;
                }
                else
                {
                    m_pScriptDebugging->LogError(luaVM, "Translation system not initialized for resource '%s' when requesting '%s'", resource->GetName(), msgid.c_str());
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushstring(luaVM, msgid.c_str());
    return 1;
}

int CLuaTranslationDefsClient::GetAvailableTranslations(lua_State* luaVM)
{
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (luaMain)
    {
        CResource* resource = luaMain->GetResource();
        if (resource)
        {
            if (resource->GetTranslationManager())
            {
                std::vector<std::string> languages = resource->GetTranslationManager()->GetAvailableLanguages();
                
                lua_newtable(luaVM);
                int index = 1;
                for (size_t i = 0; i < languages.size(); ++i)
                {
                    lua_pushinteger(luaVM, index++);
                    lua_pushstring(luaVM, languages[i].c_str());
                    lua_settable(luaVM, -3);
                }
                return 1;
            }
            else
            {
                m_pScriptDebugging->LogError(luaVM, "Translation system not initialized for resource '%s'", resource->GetName());
            }
        }
    }

    lua_newtable(luaVM);
    return 1;
}
