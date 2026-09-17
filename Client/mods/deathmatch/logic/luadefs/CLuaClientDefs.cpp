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
#include <gui/CGUI.h>

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
                                                                             {"getCursorPosition", ArgumentParserWarn<false, GetCursorPosition>},
                                                                             {"setCursorPosition", ArgumentParserWarn<false, SetCursorPosition>},
                                                                             {"isCursorShowing", ArgumentParserWarn<false, IsCursorShowing>},
                                                                             {"getCursorAlpha", ArgumentParserWarn<false, GetCursorAlpha>},
                                                                             {"setCursorAlpha", ArgumentParserWarn<false, SetCursorAlpha>},
                                                                             {"showCursor", ArgumentParserWarn<false, ShowCursor>}};

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

std::variant<CLuaMultiReturn<float, float, float, float, float>, bool> CLuaClientDefs::GetCursorPosition()
{
    CGUI* gui = g_pCore->GetGUI();
    if (!m_pClientGame->AreCursorEventsEnabled() && !gui->GetGUIInputEnabled() && !g_pCore->GetConsole()->IsVisible() && !g_pCore->IsChatInputEnabled())
        return false;

    tagPOINT point;
    GetCursorPos(&point);

    HWND hookedWindow = g_pCore->GetHookedWindow();

    tagPOINT windowPos = {0};
    ClientToScreen(hookedWindow, &windowPos);

    CVector2D resolution = gui->GetResolution();
    point.x -= windowPos.x;
    point.y -= windowPos.y;
    if (point.x < 0)
        point.x = 0;
    else if (point.x > (long)resolution.fX)
        point.x = (long)resolution.fX;
    if (point.y < 0)
        point.y = 0;
    else if (point.y > (long)resolution.fY)
        point.y = (long)resolution.fY;

    CVector2D cursor(((float)point.x) / resolution.fX, ((float)point.y) / resolution.fY);

    CVector screen((float)((int)point.x), (float)((int)point.y), 300.0f);
    CVector world;
    g_pCore->GetGraphics()->CalcWorldCoors(&screen, &world);

    return CLuaMultiReturn<float, float, float, float, float>{cursor.fX, cursor.fY, world.fX, world.fY, world.fZ};
}

bool CLuaClientDefs::SetCursorPosition(CVector2D position)
{
    HWND hookedWindow = g_pCore->GetHookedWindow();

    tagPOINT windowPos = {0};
    ClientToScreen(hookedWindow, &windowPos);

    CVector2D resolution = g_pCore->GetGUI()->GetResolution();

    if (position.fX < 0)
        position.fX = 0.0f;
    else if (position.fX > resolution.fX)
        position.fX = resolution.fX;
    if (position.fY < 0)
        position.fY = 0.0f;
    else if (position.fY > resolution.fY)
        position.fY = resolution.fY;

    g_pCore->CallSetCursorPos((int)position.fX + (int)windowPos.x, (int)position.fY + (int)windowPos.y);
    return true;
}

bool CLuaClientDefs::IsCursorShowing() noexcept
{
    return m_pClientGame->AreCursorEventsEnabled() || g_pCore->IsCursorForcedVisible();
}

int CLuaClientDefs::GetCursorAlpha() noexcept
{
    return Round(g_pCore->GetGUI()->GetCurrentServerCursorAlpha() * 255.f);
}

bool CLuaClientDefs::SetCursorAlpha(float alpha)
{
    const float normalizedAlpha = alpha / 255.f;
    if (normalizedAlpha < 0.0f || normalizedAlpha > 1.0f)
        return false;

    if (!g_pCore->IsMenuVisible() && !g_pCore->GetConsole()->IsVisible())
        g_pCore->GetGUI()->SetCursorAlpha(normalizedAlpha, true);
    else
        g_pCore->GetGUI()->SetCurrentServerCursorAlpha(normalizedAlpha);

    return true;
}

bool CLuaClientDefs::ShowCursor(lua_State* luaVM, bool show, std::optional<bool> toggleControls)
{
    CResource* resource = lua_getownercluamain(luaVM).GetResource();
    if (!resource)
        return false;

    resource->ShowCursor(show, toggleControls.value_or(true));
    return true;
}
