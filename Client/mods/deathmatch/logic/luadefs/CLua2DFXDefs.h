/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLua2DFXDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <lua/CLuaMultiReturn.h>
#include <game/Common.h>

using effectFlagsTable = std::unordered_map<e2dEffectFlags, std::variant<bool, int>>;
using effectPropertiesMap = std::unordered_map<std::string, std::variant<bool, float, std::string, std::vector<float>, effectFlagsTable>>;

/*
[index] = {
    type = "name",
    position = {x,y,z},
    properties = {
        coronaSize = 1,
        shadowName = "shad_exp",
        coronaReflection = false,
        ...
    },
}
*/
using effectDataMap = std::unordered_map<std::string, std::variant<std::string, std::vector<float>, effectPropertiesMap>>;
using effectsMap = std::unordered_map<std::uint32_t, effectDataMap>;

class CLua2DFXDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    // Create/destroy functions
    static bool AddModel2DFX(std::uint32_t model, CVector position, e2dEffectType effectType, effectPropertiesMap effectData);
    static bool RemoveModel2DFX(std::uint32_t model, std::uint32_t index);
    static void RestoreModel2DFX(std::uint32_t model, std::uint32_t index);
    static void ResetModel2DFXEffects(std::uint32_t model);

    // Set functions
    static void SetModel2DFXPosition(std::uint32_t model, std::uint32_t index, CVector position);
    static bool SetModel2DFXProperty(std::uint32_t model, std::uint32_t index, e2dEffectProperty property,
                                     std::variant<bool, float, std::string, std::vector<float>, effectFlagsTable> propertyValue);
    static void ResetModel2DFXProperty(std::uint32_t model, std::uint32_t index, e2dEffectProperty property);
    static void ResetModel2DFXPosition(std::uint32_t model, std::uint32_t index);

    // Get functions
    static std::variant<bool, CLuaMultiReturn<float, float, float>> GetModel2DFXPosition(std::uint32_t model, std::uint32_t index);
    static std::variant<bool, float, std::string, CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>, CLuaMultiReturn<float, float, float>,
                        effectFlagsTable>
    GetModel2DFXProperty(std::uint32_t model, std::uint32_t index, e2dEffectProperty property, std::optional<bool> getFlagsAsTable);
    static std::variant<bool, effectsMap> GetModel2DFXEffects(std::uint32_t model, std::optional<bool> includeCustomEffects);
    static std::uint32_t                  GetModel2DFXCount(std::uint32_t model, std::optional<bool> includeCustomEffects);
    static e2dEffectType                  GetModel2DFXType(std::uint32_t model, std::uint32_t index);
};
