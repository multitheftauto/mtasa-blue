/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPostfxDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaPostfxDefs.h"
#include <lua/CLuaFunctionParser.h>

void CLuaPostfxDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"getPostFXValue", ArgumentParser<GetPostFXValue>},
        {"getPostFXMode", ArgumentParser<GetPostFXMode>},
        {"isPostFXEnabled", ArgumentParser<IsPostFXEnabled>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

std::variant<float, bool> CLuaPostfxDefs::GetPostFXValue(int type)
{
    switch (static_cast<int>(type))
    {
        case 0: // Gamma
            return g_pCore->GetCVars()->GetValue<float>("borderless_gamma_power", 0.0f);
        case 1: // Brightness
            return g_pCore->GetCVars()->GetValue<float>("borderless_brightness_scale", 0.0f);
        case 2: // Contrast
            return g_pCore->GetCVars()->GetValue<float>("borderless_contrast_scale", 0.0f);
        case 3: // Saturation
            return g_pCore->GetCVars()->GetValue<float>("borderless_saturation_scale", 0.0f);
        default:
            return false;
    }
}

int CLuaPostfxDefs::GetPostFXMode()
{
    return g_pCore->GetCVars()->GetValue<bool>("borderless_apply_fullscreen", false) ? 1
         : g_pCore->GetCVars()->GetValue<bool>("borderless_apply_windowed", false) ? 2
         : 0;
}

bool CLuaPostfxDefs::IsPostFXEnabled(int type)
{
    switch (static_cast<int>(type))
    {
        case 0: // Gamma
            return g_pCore->GetCVars()->GetValue<bool>("borderless_gamma_enabled", false);
        case 1: // Brightness
            return g_pCore->GetCVars()->GetValue<bool>("borderless_brightness_enabled", false);
        case 2: // Contrast
            return g_pCore->GetCVars()->GetValue<bool>("borderless_contrast_enabled", false);
        case 3: // Saturation
            return g_pCore->GetCVars()->GetValue<bool>("borderless_saturation_enabled", false);
        default:
            return false;
    }
}
