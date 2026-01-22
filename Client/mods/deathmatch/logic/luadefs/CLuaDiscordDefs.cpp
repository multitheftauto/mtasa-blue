/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
    auto discord = g_pCore->GetDiscord();

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
    int stateLength = strState.length();

    if (stateLength > 128)
        throw std::invalid_argument("State must be less than/equal to 128");

    auto discord = g_pCore->GetDiscord();

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
    int appIDLength = strAppID.length();

    if (appIDLength < 1 || appIDLength > 32)
        throw std::invalid_argument("Application ID must be greater than 0, or less than/equal to 32");

    auto discord = g_pCore->GetDiscord();

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    const char* resourceName = "";
    if (pLuaMain)
    {
        CResource* pResource = pLuaMain->GetResource();
        if (pResource)
            resourceName = pResource->GetName();
    }

    if (!discord || !discord->IsDiscordRPCEnabled() || !discord->SetApplicationID(resourceName, strAppID.c_str()))
        return false;

    return true;
}

bool CLuaDiscordDefs::SetDetails(std::string strDetails)
{
    int detailsLength = strDetails.length();

    if (detailsLength > 128)
        throw std::invalid_argument("Details length must be less than/equal to 128");

    auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    if (discord->IsDiscordCustomDetailsDisallowed())
        return false;

    if (!discord->SetPresenceDetails(strDetails.c_str(), true))
        return false;

    return true;
}

bool CLuaDiscordDefs::SetStartTime(unsigned long ulTime)
{
    unsigned long ulSecondsSinceEpoch = time(nullptr) + ulTime;

    if (ulTime == 0)
        ulSecondsSinceEpoch = 0;

    auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    if (discord->IsDiscordCustomDetailsDisallowed())
        return false;

    discord->SetPresenceStartTimestamp(ulSecondsSinceEpoch);
    return true;
}

bool CLuaDiscordDefs::SetEndTime(unsigned long ulTime)
{
    unsigned long ulSecondsSinceEpoch = time(nullptr) + ulTime;

    if (ulTime == 0)
        ulSecondsSinceEpoch = 0;

    auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    if (discord->IsDiscordCustomDetailsDisallowed())
        return false;

    discord->SetPresenceEndTimestamp(ulSecondsSinceEpoch);
    return true;
}

bool CLuaDiscordDefs::SetPartySize(int iSize, int iMax)
{
    if (iSize > iMax)
        throw std::invalid_argument("Party size must be less than or equal to max party size");

    auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    if (discord->IsDiscordCustomDetailsDisallowed())
        return false;

    discord->SetPresencePartySize(iSize, iMax, true);
    return true;
}

bool CLuaDiscordDefs::SetAsset(std::string szAsset, std::string szAssetText, bool bIsLarge)
{
    int assetLength = szAsset.length();
    int assetTextLength = szAssetText.length();

    if (assetLength > 32)
        throw std::invalid_argument("Asset name length must be less than/equal to 32");
    if (assetTextLength > 128)
        throw std::invalid_argument("Asset text length must be less than/equal to 128");

    auto discord = g_pCore->GetDiscord();

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

bool CLuaDiscordDefs::SetButtons(unsigned short int iIndex, std::string szName, std::string szUrl)
{
    if (iIndex < 1 || iIndex > 2)
        return false;

    int nameLength = szName.length();
    int urlLength = szUrl.length();

    if (nameLength > 32)
        throw std::invalid_argument("Button name length must be less than/equal to 32");
    if (urlLength > 128)
        throw std::invalid_argument("Button URL length must be less than/equal to 128");

    if (szUrl.find("https://") != 0 && szUrl.find("mtasa://") != 0)
        throw std::invalid_argument("Button URL should include the https:// or mtasa:// link");

    auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    if (discord->IsDiscordCustomDetailsDisallowed())
        return false;

    if (!discord->SetPresenceButtons(iIndex, szName.c_str(), szUrl.c_str()))
        return false;

    return true;
}

bool CLuaDiscordDefs::IsDiscordRPCConnected()
{
    auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return false;

    return discord->IsDiscordClientConnected();
}

std::string CLuaDiscordDefs::GetDiscordUserID()
{
    auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->IsDiscordRPCEnabled())
        return {};

    return discord->GetDiscordUserID();
}
