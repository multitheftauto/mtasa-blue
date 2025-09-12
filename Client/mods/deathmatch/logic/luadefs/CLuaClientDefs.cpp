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
                                                                             {"setCursorColor", ArgumentParser<SetCursorColor>},
                                                                             {"getCursorColor", ArgumentParser<GetCursorColor>}};

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

bool CLuaClientDefs::SetCursorColor(std::optional<float> r, std::optional<float> g, std::optional<float> b, std::optional<float> alpha) noexcept
{
    if (!g_pCore->IsMenuVisible()) 
        g_pCore->GetGUI()->SetCursorColor(r.value_or(255.0f), g.value_or(255.0f), b.value_or(255.0f), alpha.value_or(255.0f));
    else
        g_pCore->GetGUI()->ResetCursorColorVariables();            // Force variables to be updated when close the main menu it will set the new color

    return true;
}

CLuaMultiReturn<float, float, float, float> CLuaClientDefs::GetCursorColor() noexcept
{
    float r, g, b, alpha;
    g_pCore->GetGUI()->GetCursorColor(r, g, b, alpha);
    return {r, g, b, alpha};
}
