/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaGenericDefs.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaGenericDefs.h"
#include "CStaticFunctionDefinitions.h"
#include "CScriptArgReader.h"
#include "CMasterServerAnnouncer.h"

static auto GetServerIpFromMasterServer() -> std::string
{
    return g_pGame->GetMasterServerAnnouncer()->GetRemoteAddress();
}

void CLuaGenericDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"setTransferBoxVisible", ArgumentParser<CStaticFunctionDefinitions::SetClientTransferBoxVisible>},
        {"isTransferBoxVisible", ArgumentParser<IsTransferBoxVisible>},
        {"getMaxPlayers", ArgumentParserWarn<0, CStaticFunctionDefinitions::GetMaxPlayers>},
        {"setMaxPlayers", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetMaxPlayers>},
        {"outputChatBox", ArgumentParserWarn<false, OutputChatBox>},
        {"outputConsole", ArgumentParserWarn<false, OutputConsole>},
        {"outputDebugString", ArgumentParserWarn<false, OutputScriptDebugLog>},
        {"outputServerLog", ArgumentParserWarn<false, OutputServerLog>},
        {"getServerIpFromMasterServer", ArgumentParser<GetServerIpFromMasterServer>},
        {"getServerName", ArgumentParserWarn<nullptr, GetServerName>},
        {"getServerHttpPort", ArgumentParserWarn<nullptr, GetServerHttpPort>},
        {"getServerPassword", ArgumentParserWarn<nullptr, GetServerPassword>},
        {"setServerPassword", ArgumentParserWarn<false, SetServerPassword>},
        {"getServerConfigSetting", ArgumentParserWarn<false, GetServerConfigSetting>},
        {"clearChatBox", ArgumentParserWarn<false, ClearChatBox>},
    };

    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

bool CLuaGenericDefs::IsTransferBoxVisible()
{
    return g_pGame->IsClientTransferBoxVisible();
}

bool CLuaGenericDefs::ClearChatBox(std::optional<CElement*> element)
{
    CStaticFunctionDefinitions::ClearChatBox(element.value_or(m_pRootElement));
    return true;
}

std::variant<bool, std::string, CLuaArguments> CLuaGenericDefs::GetServerConfigSetting(std::string name)
{
    // Try as single setting
    if (SString value; g_pGame->GetConfig()->GetSetting(name, value))
    {
        return value;
    }

    // Try as multiple setting
    if (CLuaArguments result; g_pGame->GetConfig()->GetSettingTable(name, &result))
    {
        return result;
    }

    return false;
}

bool CLuaGenericDefs::SetServerPassword(std::optional<std::string> rawPassword)
{
    if (!CStaticFunctionDefinitions::SetServerPassword(rawPassword.value_or(""), true))
        throw std::invalid_argument("password must be shorter than 32 chars and just contain visible characters");
    return true;
}

std::variant<std::nullptr_t, std::string_view> CLuaGenericDefs::GetServerPassword()
{
    if (g_pGame->GetConfig()->HasPassword())
    {
        return std::string_view{g_pGame->GetConfig()->GetPassword()};
    }

    return nullptr;
}

unsigned int CLuaGenericDefs::GetServerHttpPort()
{
    return g_pGame->GetConfig()->GetHTTPPort();
}

std::string_view CLuaGenericDefs::GetServerName()
{
    return g_pGame->GetConfig()->GetServerName();
}

bool CLuaGenericDefs::OutputServerLog(std::string_view message)
{
    CLogger::LogPrintf(LOGLEVEL_MEDIUM, "%.*s", message.length(), message.data());
    CLogger::LogPrintNoStamp("\n");
    return true;
}

bool CLuaGenericDefs::OutputScriptDebugLog(lua_State* luaVM, std::string message, std::optional<unsigned char> rawLevel, std::optional<unsigned char> rawColorR,
                                           std::optional<unsigned char> rawColorG, std::optional<unsigned char> rawColorB)
{
    unsigned char level = rawLevel.value_or(3);

    if (level > 4)
        throw std::invalid_argument("level must be between 0 and 4");

    unsigned char colorR = rawColorR.value_or(255);
    unsigned char colorG = rawColorG.value_or(255);
    unsigned char colorB = rawColorB.value_or(255);

    switch (level)
    {
        case 0:
            m_pScriptDebugging->LogDebug(luaVM, colorR, colorG, colorB, "%.*s", message.length(), message.c_str());
            break;
        case 1:
            m_pScriptDebugging->LogError(luaVM, "%.*s", message.length(), message.c_str());
            break;
        case 2:
            m_pScriptDebugging->LogWarning(luaVM, "%.*s", message.length(), message.c_str());
            break;
        case 3:
            m_pScriptDebugging->LogInformation(luaVM, "%.*s", message.length(), message.c_str());
            break;
        case 4:
            m_pScriptDebugging->LogCustom(luaVM, colorR, colorG, colorB, "%.*s", message.length(), message.c_str());
            break;
    }

    return true;
}

bool CLuaGenericDefs::OutputConsole(std::string message, std::optional<CElement*> rawElement)
{
    CStaticFunctionDefinitions::OutputConsole(message.c_str(), rawElement.value_or(m_pRootElement));
    return true;
}

bool CLuaGenericDefs::OutputChatBox(lua_State* luaVM, std::string message, std::optional<std::variant<CElement*, std::vector<CPlayer*>>> rawVisibleTo,
                                    std::optional<unsigned char> rawColorR, std::optional<unsigned char> rawColorG, std::optional<unsigned char> rawColorB,
                                    std::optional<bool> rawColorCoded)
{
    unsigned char colorR = 231, colorG = 217, colorB = 176;
    bool          colorCoded = rawColorCoded.value_or(false);

    if (rawColorR.has_value() && rawColorG.has_value() && rawColorB.has_value())
    {
        colorR = rawColorR.value();
        colorG = rawColorG.value();
        colorB = rawColorB.value();
    }

    std::vector<CPlayer*> sendList;

    if (!rawVisibleTo.has_value() || std::holds_alternative<CElement*>(rawVisibleTo.value()))
    {
        CElement* element = (rawVisibleTo.has_value() ? std::get<CElement*>(rawVisibleTo.value()) : m_pRootElement);

        if (IS_TEAM(element))
        {
            auto team = static_cast<CTeam*>(element);
            sendList.reserve(team->CountPlayers());
            sendList.insert(sendList.begin(), team->PlayersBegin(), team->PlayersEnd());
        }
        else
        {
            CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            CStaticFunctionDefinitions::OutputChatBox(message.c_str(), element, colorR, colorG, colorB, colorCoded, luaMain);
            return true;
        }
    }
    else
    {
        sendList = std::move(std::get<std::vector<CPlayer*>>(rawVisibleTo.value()));
    }

    if (sendList.empty())
        return false;

    CStaticFunctionDefinitions::OutputChatBox(message.c_str(), sendList, colorR, colorG, colorB, colorCoded);
    return true;
}

bool CLuaGenericDefs::OOP_OutputChatBox(lua_State* luaVM, CElement* element, std::string message, std::optional<unsigned char> rawColorR,
                                        std::optional<unsigned char> rawColorG, std::optional<unsigned char> rawColorB, std::optional<bool> rawColorCoded)
{
    return OutputChatBox(luaVM, std::move(message), element, rawColorR, rawColorG, rawColorB, rawColorCoded);
}
