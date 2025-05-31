/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaVoiceDefs.h
 *  PURPOSE:     Lua Voice function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaVoiceDefs.h"
#include "CScriptArgReader.h"

void CLuaVoiceDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"isVoiceEnabled", IsVoiceEnabled},
        {"setPlayerVoiceBroadcastTo", SetPlayerVoiceBroadcastTo},
        {"setPlayerVoiceIgnoreFrom", setPlayerVoiceIgnoreFrom},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

int CLuaVoiceDefs::IsVoiceEnabled(lua_State* luaVM)
{
    lua_pushboolean(luaVM, g_pGame->GetConfig()->IsVoiceEnabled());
    return 1;
}

int CLuaVoiceDefs::SetPlayerVoiceBroadcastTo(lua_State* luaVM)
{
    CPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        // Second argument can be an element, table or nil

        if (argStream.NextIsNil())
        {
            pPlayer->SetVoiceBroadcastTo(NULL);

            lua_pushboolean(luaVM, true);
            return 1;
        }
        else if (argStream.NextIsTable())
        {
            std::list<CElement*> lstElements;
            for (lua_pushnil(luaVM); lua_next(luaVM, 2); lua_pop(luaVM, 1))
            {
                CElement* pListedElement = lua_toelement(luaVM, -1);
                if (pListedElement)
                    lstElements.push_back(pListedElement);
            }

            pPlayer->SetVoiceBroadcastTo(lstElements);

            lua_pushboolean(luaVM, true);
            return 1;
        }
        else if (argStream.NextIsUserData())
        {
            CElement* pElement;
            argStream.ReadUserData(pElement);

            pPlayer->SetVoiceBroadcastTo(pElement);

            lua_pushboolean(luaVM, true);
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer(luaVM, "broadcastTo", 2);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVoiceDefs::setPlayerVoiceIgnoreFrom(lua_State* luaVM)
{
    CPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        // Second argument can be an element, table or nil

        if (argStream.NextIsNil())
        {
            pPlayer->SetVoiceIgnoredElement(NULL);

            lua_pushboolean(luaVM, true);
            return 1;
        }
        else if (argStream.NextIsTable())
        {
            std::list<CElement*> lstElements;
            for (lua_pushnil(luaVM); lua_next(luaVM, 2); lua_pop(luaVM, 1))
            {
                CElement* pListedElement = lua_toelement(luaVM, -1);
                if (pListedElement)
                    lstElements.push_back(pListedElement);
            }

            pPlayer->SetVoiceIgnoredList(lstElements);

            lua_pushboolean(luaVM, true);
            return 1;
        }
        else if (argStream.NextIsUserData())
        {
            CElement* pElement;
            argStream.ReadUserData(pElement);

            pPlayer->SetVoiceIgnoredElement(pElement);

            lua_pushboolean(luaVM, true);
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer(luaVM, "ignoreFrom", 2);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
