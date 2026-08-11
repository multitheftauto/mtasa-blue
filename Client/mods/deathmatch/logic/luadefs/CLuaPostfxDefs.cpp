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

float CLuaPostfxDefs::GetPostFXValue(PostFXType type)
{
    switch (type)
    {
        case PostFXType::GAMMA:
            return g_pCore->GetCVars()->GetValue<float>("borderless_gamma_power", 0.0f);
        case PostFXType::BRIGHTNESS:
            return g_pCore->GetCVars()->GetValue<float>("borderless_brightness_scale", 0.0f);
        case PostFXType::CONTRAST:
            return g_pCore->GetCVars()->GetValue<float>("borderless_contrast_scale", 0.0f);
        case PostFXType::SATURATION:
            return g_pCore->GetCVars()->GetValue<float>("borderless_saturation_scale", 0.0f);

        default:
            throw std::invalid_argument("Invalid PostFX type");
    }
}

int CLuaPostfxDefs::GetPostFXMode()
{
    return g_pCore->GetCVars()->GetValue<bool>("borderless_apply_fullscreen", false) ? 1
           : g_pCore->GetCVars()->GetValue<bool>("borderless_apply_windowed", false) ? 2
                                                                                     : 0;
}

bool CLuaPostfxDefs::IsPostFXEnabled(PostFXType type)
{
    switch (type)
    {
        case PostFXType::GAMMA:
            return g_pCore->GetCVars()->GetValue<float>("borderless_gamma_enabled", false);
        case PostFXType::BRIGHTNESS:
            return g_pCore->GetCVars()->GetValue<float>("borderless_brightness_enabled", false);
        case PostFXType::CONTRAST:
            return g_pCore->GetCVars()->GetValue<float>("borderless_brightness_enabled", false);
        case PostFXType::SATURATION:
            return g_pCore->GetCVars()->GetValue<float>("borderless_saturation_enabled", false);

        default:
            throw std::invalid_argument("Invalid PostFX type");
    }
}
