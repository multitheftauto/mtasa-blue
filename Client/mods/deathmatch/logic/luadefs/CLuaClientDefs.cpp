/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaClientDefs.cpp
 *  PURPOSE:     Lua client definitions class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaClientDefs.h"
#include "lua/CLuaFunctionParser.h"
#include <SharedUtil.SysInfo.h>

void CLuaClientDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{{"setTransferBoxVisible", ArgumentParser<SetTransferBoxVisible>},
                                                                             {"isTransferBoxVisible", ArgumentParser<IsTransferBoxVisible>},
                                                                             {"isTransferBoxAlwaysVisible", ArgumentParser<IsTransferBoxAlwaysVisible>},
                                                                             {"showChat", ArgumentParserWarn<false, ShowChat>},
                                                                             {"isChatVisible", ArgumentParserWarn<false, IsChatVisible>},
                                                                             {"isChatInputBlocked", ArgumentParser<IsChatInputBlocked>},
                                                                             {"clearDebugBox", ArgumentParser<ClearDebug>},
                                                                             {"isMTAWindowFocused", ArgumentParser<IsMTAWindowFocused>},
                                                                             {"isCapsLockEnabled", ArgumentParser<IsCapsLockEnabled>},
                                                                             {"getCPUInfo", ArgumentParser<GetCPUInfo>}};

    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

bool CLuaClientDefs::SetTransferBoxVisible(bool visible)
{
    return g_pClientGame->GetTransferBox()->SetClientVisibility(visible);
}

bool CLuaClientDefs::IsTransferBoxVisible()
{
    return g_pClientGame->GetTransferBox()->IsVisible();
}

bool CLuaClientDefs::IsTransferBoxAlwaysVisible()
{
    return g_pClientGame->GetTransferBox()->IsAlwaysVisible();
}

bool CLuaClientDefs::ShowChat(bool bVisible, std::optional<bool> optInputBlocked)
{
    // Keep old behaviour: input is blocked when chat is hidden
    bool bInputBlocked = !bVisible;
    if (optInputBlocked.has_value())
        bInputBlocked = optInputBlocked.value();

    g_pCore->SetChatVisible(bVisible, bInputBlocked);
    return true;
}

bool CLuaClientDefs::IsChatVisible()
{
    return g_pCore->IsChatVisible();
}

bool CLuaClientDefs::IsChatInputBlocked()
{
    return g_pCore->IsChatInputBlocked();
}

bool CLuaClientDefs::ClearDebug()
{
    g_pCore->DebugClear();
    return true;
}

bool CLuaClientDefs::IsMTAWindowFocused()
{
    return m_pClientGame->IsWindowFocused();
}

bool CLuaClientDefs::IsCapsLockEnabled()
{
    return ((::GetKeyState(VK_CAPITAL) & 0x0001) != 0);
}

CLuaArguments CLuaClientDefs::GetCPUInfo()
{
    CLuaArguments table;
    bool          bAllowCPUInfo = true;
    g_pCore->GetCVars()->Get("allow_cpu_info", bAllowCPUInfo);

    table.PushString("AllowCPUInfo");
    table.PushBoolean(bAllowCPUInfo);

    // Opt-out must omit hardware fields so server scripts cannot recover them from a client event.
    if (!bAllowCPUInfo)
        return table;

    SharedUtil::SCPUInfo info;
    SharedUtil::GetCPUInfo(info);

    table.PushString("Name");
    table.PushString(info.strName);
    table.PushString("MaxClockSpeedMHz");
    table.PushNumber(info.uiMaxClockSpeedMHz);
    table.PushString("Cores");
    table.PushNumber(info.uiCores);
    table.PushString("Threads");
    table.PushNumber(info.uiThreads);
    table.PushString("L1CacheKB");
    table.PushNumber(info.uiL1CacheKB);
    table.PushString("L2CacheKB");
    table.PushNumber(info.uiL2CacheKB);
    table.PushString("L3CacheKB");
    table.PushNumber(info.uiL3CacheKB);
    return table;
}
