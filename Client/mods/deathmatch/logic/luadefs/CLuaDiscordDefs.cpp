/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

void CLuaDiscordDefs::LoadFunctions()
{
    // Backwards compatibility functions
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"setDiscordApplicationID", ArgumentParser<SetAppID>},
        {"setDiscordRichPresenceDetails", ArgumentParser<SetDetails>},
        {"setDiscordRichPresenceState", ArgumentParser<SetState>},
        {"setDiscordRichPresenceAsset", ArgumentParser<SetLargeAsset>},
        {"setDiscordRichPresenceSmallAsset", ArgumentParser<SetSmallAsset>},
        {"setDiscordRichPresenceButton", ArgumentParser<SetButtons>},
        {"setDiscordRichPresenceStartTime", ArgumentParser<SetStartTime>},
        {"setDiscordRichPresenceEndTime", ArgumentParser<SetEndTime>},
        {"setDiscordRichPresencePartySize", ArgumentParser<SetPartySize>},
        {"resetDiscordRichPresenceData", ArgumentParser<ResetData>},
        {"isDiscordRichPresenceConnected", ArgumentParser <IsDiscordRPCConnected>},
        {"getDiscordRichPresenceUserID", ArgumentParser<GetDiscordUserID>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaDiscordDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "setApplication", "setDiscordApplicationID");
    lua_classfunction(luaVM, "setState", "setDiscordRichPresenceState");
    lua_classfunction(luaVM, "setDetails", "setDiscordRichPresenceDetails");
    lua_classfunction(luaVM, "setAsset", "setDiscordRichPresenceAsset");
    lua_classfunction(luaVM, "setSmallAsset", "setDiscordRichPresenceSmallAsset");
    lua_classfunction(luaVM, "setButton", "setDiscordRichPresenceButton");
    lua_classfunction(luaVM, "setStartTime", "setDiscordRichPresenceStartTime");
    lua_classfunction(luaVM, "setEndTime", "setDiscordRichPresenceEndTime");
    lua_classfunction(luaVM, "setPartySize", "setDiscordRichPresencePartySize");

    lua_classfunction(luaVM, "isConnected", "isDiscordRichPresenceConnected");
    //lua_classfunction(luaVM, "setAppID", "setDiscordRichPresenceAppID");
    //lua_classfunction(luaVM, "setAppID", "setDiscordRichPresenceAppID");

    lua_registerclass(luaVM, "DiscordRPC");
}

bool CLuaDiscordDefs::ResetData()
{
    const auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    if (discord->IsDiscordCustomDetailsDisallowed())
        return false;

    if (!discord->ResetDiscordData())
        return false;

    return true;
}

bool CLuaDiscordDefs::SetState(std::string strState)
{
    const auto stateLength = strState.length();

    if (stateLength > 128)
        throw std::invalid_argument("State must be less than/equal to 128");

    const auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    if (discord->IsDiscordCustomDetailsDisallowed())
        return false;

    if (!discord->SetPresenceState(strState.c_str(), true))
        return false;

    return true;
}

bool CLuaDiscordDefs::SetAppID(lua_State* luaVM, std::string strAppID)
{
    const auto appIDLength = strAppID.length();

    if (appIDLength < 1 || appIDLength > 32)
        throw std::invalid_argument("Application ID must be greater than 0, or less than/equal to 32");

    const auto discord = g_pCore->GetDiscord();

    const auto  pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    const char* resourceName = "";
    if (pLuaMain)
    {
        const auto pResource = pLuaMain->GetResource();
        if (pResource)
            resourceName = pResource->GetName();
    }

    if (!discord || !discord->IsDiscordRPCEnabled() || !discord->SetApplicationID(resourceName, strAppID.c_str()))
        return false;

    return true;
}

bool CLuaDiscordDefs::SetDetails(std::string strDetails)
{
    const auto detailsLength = strDetails.length();

    if (detailsLength > 128)
        throw std::invalid_argument("Details length must be less than/equal to 128");

    const auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    if (discord->IsDiscordCustomDetailsDisallowed())
        return false;

    if (!discord->SetPresenceDetails(strDetails.c_str(), true))
        return false;

    return true;
}

bool CLuaDiscordDefs::SetStartTime(int64_t iTime)
{
    int64_t iSecondsSinceEpoch = time(nullptr) + iTime;

    if (iTime == 0)
        iSecondsSinceEpoch = 0;

    const auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    if (discord->IsDiscordCustomDetailsDisallowed())
        return false;

    discord->SetPresenceStartTimestamp(iSecondsSinceEpoch);
    return true;
}

bool CLuaDiscordDefs::SetEndTime(int64_t iTime)
{
    int64_t iSecondsSinceEpoch = time(nullptr) + iTime;

    if (iTime == 0)
        iSecondsSinceEpoch = 0;

    const auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    if (discord->IsDiscordCustomDetailsDisallowed())
        return false;

    discord->SetPresenceEndTimestamp(iSecondsSinceEpoch);
    return true;
}

bool CLuaDiscordDefs::SetPartySize(int iSize, int iMax)
{
    if (iSize > iMax)
        throw std::invalid_argument("Party size must be less than or equal to max party size");

    const auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    if (discord->IsDiscordCustomDetailsDisallowed())
        return false;

    discord->SetPresencePartySize(iSize, iMax, true);
    return true;
}

bool CLuaDiscordDefs::SetAsset(std::string szAsset, std::string szAssetText, bool bIsLarge)
{
    const auto assetLength = szAsset.length();
    const auto assetTextLength = szAssetText.length();

    if (assetLength > 32)
        throw std::invalid_argument("Asset name length must be less than/equal to 32");
    if (assetTextLength > 128)
        throw std::invalid_argument("Asset text length must be less than/equal to 128");

    const auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    if (discord->IsDiscordCustomDetailsDisallowed())
        return false;

    if (bIsLarge)
        discord->SetAssetLargeData(szAsset.c_str(), szAssetText.c_str());
    else
        discord->SetAssetSmallData(szAsset.c_str(), szAssetText.c_str());

    return true;
}

bool CLuaDiscordDefs::SetLargeAsset(std::string szAsset, std::string szAssetText)
{
    return SetAsset(szAsset, szAssetText, true);
}

bool CLuaDiscordDefs::SetSmallAsset(std::string szAsset, std::string szAssetText)
{
    return SetAsset(szAsset, szAssetText, false);
}

bool CLuaDiscordDefs::SetButtons(uint8_t uiIndex, std::string szName, std::string szUrl)
{
    if (uiIndex < 1 || uiIndex > 2)
        return false;

    const auto nameLength = szName.length();
    const auto urlLength = szUrl.length();

    if (nameLength > 32)
        throw std::invalid_argument("Button name length must be less than/equal to 32");
    if (urlLength > 128)
        throw std::invalid_argument("Button URL length must be less than/equal to 128");

    if (szUrl.find("https://") != 0 && szUrl.find("mtasa://") != 0)
        throw std::invalid_argument("Button URL should include the https:// or mtasa:// link");

    const auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    if (discord->IsDiscordCustomDetailsDisallowed())
        return false;

    if (!discord->SetPresenceButtons(uiIndex, szName.c_str(), szUrl.c_str()))
        return false;

    return true;
}

bool CLuaDiscordDefs::IsDiscordRPCConnected()
{
    const auto discord = g_pCore->GetDiscord();
    return (!discord || !discord->IsDiscordRPCEnabled()) && discord->IsDiscordClientConnected();
}

std::string CLuaDiscordDefs::GetDiscordUserID()
{
    if (const auto discord = g_pCore->GetDiscord(); discord && discord->IsDiscordRPCEnabled())
    {
        return discord->GetDiscordUserID();
    }
    return {};
}
