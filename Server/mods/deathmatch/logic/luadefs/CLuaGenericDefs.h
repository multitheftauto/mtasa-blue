/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaGenericDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CLuaDefs.h"

class CLuaGenericDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

private:
    static bool IsTransferBoxVisible();
    static bool ClearChatBox(std::optional<CElement*> element);

    static std::variant<bool, std::string, CLuaArguments> GetServerConfigSetting(std::string name);

    static bool                                           SetServerPassword(std::optional<std::string> rawPassword);
    static std::variant<std::nullptr_t, std::string_view> GetServerPassword();

    static unsigned int GetServerHttpPort();

    static std::string_view GetServerName();

    static bool OutputServerLog(std::string_view message);

    static bool OutputScriptDebugLog(lua_State* luaVM, std::string message, std::optional<unsigned char> rawLevel, std::optional<unsigned char> rawColorR,
                                     std::optional<unsigned char> rawColorG, std::optional<unsigned char> rawColorB);

    static bool OutputConsole(std::string message, std::optional<CElement*> rawElement);

    static bool OutputChatBox(lua_State* luaVM, std::string message, std::optional<std::variant<CElement*, std::vector<CPlayer*>>> rawVisibleTo,
                              std::optional<unsigned char> rawColorR, std::optional<unsigned char> rawColorG, std::optional<unsigned char> rawColorB,
                              std::optional<bool> rawColorCoded);

public:
    static bool OOP_OutputChatBox(lua_State* luaVM, CElement* element, std::string message, std::optional<unsigned char> rawColorR,
                                  std::optional<unsigned char> rawColorG, std::optional<unsigned char> rawColorB, std::optional<bool> rawColorCoded);
};
