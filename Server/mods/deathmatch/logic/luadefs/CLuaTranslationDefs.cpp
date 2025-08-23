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
#include "CGame.h"

void CLuaTranslationDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"getTranslation", GetTranslation},
        {"getAvailableTranslations", GetAvailableTranslations},
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
                    m_pScriptDebugging->LogError(luaVM, "Translation system not initialized for resource '%s' when requesting '%s'", resource->GetName().c_str(), msgid.c_str());
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
                m_pScriptDebugging->LogError(luaVM, "Translation system not initialized for resource '%s'", resource->GetName().c_str());
            }
        }
    }

    lua_newtable(luaVM);
    return 1;
}
