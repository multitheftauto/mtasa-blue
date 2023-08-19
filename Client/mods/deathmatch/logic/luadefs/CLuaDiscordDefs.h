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

    static bool ResetData();
    static bool SetAppID(std::string strAppID);
    static bool SetDetails(std::string strDetails);
    static bool SetState(std::string strState);
    static bool SetLargeAsset(std::string strAssetImage, std::string strAssetText);
    static bool SetSmallAsset(std::string strAssetImage, std::string strAssetText);
};
