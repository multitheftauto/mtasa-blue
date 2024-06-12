/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaDiscordDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

private:
    static bool        ResetData();
    static bool        SetState(std::string strState);
    static bool        SetAppID(lua_State* luaVM, std::string strAppID);
    static bool        SetDetails(std::string strDetails);
    static bool        SetAsset(std::string szAsset, std::string szAssetText, bool bIsLarge = false);
    static bool        SetButtons(uint8_t uiIndex, std::string szName, std::string szUrl);
    static bool        SetLargeAsset(std::string szAsset, std::string szAssetText);
    static bool        SetSmallAsset(std::string szAsset, std::string szAssetText);
    static bool        SetStartTime(int64_t iTime);
    static bool        SetEndTime(int64_t iTime);
    static bool        SetPartySize(int iMin, int iMax);
    static bool        IsDiscordRPCConnected();
    static std::string GetDiscordUserID();
};

