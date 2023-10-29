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
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Transferbox get funcs
        {"isTransferBoxVisible", ArgumentParser<IsTransferBoxVisible>},
        {"isTransferBoxAlwaysVisible", ArgumentParser<IsTransferBoxAlwaysVisible>},
        {"isChatVisible", ArgumentParserWarn<false, IsChatVisible>},
        {"isChatInputBlocked", ArgumentParser<IsChatInputBlocked>},
        {"isMTAWindowFocused", ArgumentParser<IsMTAWindowFocused>},
        {"isCapsLockEnabled", ArgumentParser<IsCapsLockEnabled>}

        // Transferbox set funcs
        {"setTransferBoxVisible", ArgumentParser<SetTransferBoxVisible>},

        // Transferbox dev funcs
        {"showChat", ArgumentParserWarn<false, ShowChat>},
        {"clearDebugBox", ArgumentParser<ClearDebug>},
    };

    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

bool CLuaClientDefs::SetTransferBoxVisible(bool visible)
{
    //  bool setTransferBoxVisible ( bool visible )
    return g_pClientGame->GetTransferBox()->SetClientVisibility(visible);
}

bool CLuaClientDefs::IsTransferBoxVisible()
{
    //  bool isTransferBoxVisible ( )
    return g_pClientGame->GetTransferBox()->IsVisible();
}

bool CLuaClientDefs::IsTransferBoxAlwaysVisible()
{
    //  bool isTransferBoxAlwaysVisible ( )
    return g_pClientGame->GetTransferBox()->IsAlwaysVisible();
}

bool CLuaClientDefs::ShowChat(bool bVisible, std::optional<bool> optInputBlocked)
{
    //  bool showChat ( bool show [, bool inputBlocked ] )

    // Keep old behaviour: input is blocked when chat is hidden
    bool bInputBlocked = !bVisible;
    if (optInputBlocked.has_value())
        bInputBlocked = optInputBlocked.value();

    g_pCore->SetChatVisible(bVisible, bInputBlocked);
    return true;
}

bool CLuaClientDefs::IsChatVisible()
{
    //  bool isChatVisible ( )
    return g_pCore->IsChatVisible();
}

bool CLuaClientDefs::IsChatInputBlocked()
{
    //  bool isChatInputBlocked ( )
    return g_pCore->IsChatInputBlocked();
}

bool CLuaClientDefs::ClearDebug()
{
    g_pCore->DebugClear();
    return true;
}

bool CLuaClientDefs::IsMTAWindowFocused()
{
    //  bool isMTAWindowFocused ( )
    return m_pClientGame->IsWindowFocused();
}

bool CLuaClientDefs::IsCapsLockEnabled()
{
    //  bool isCapsLockEnabled ( )
    return ((::GetKeyState(VK_CAPITAL) & 0x0001) != 0);
}
