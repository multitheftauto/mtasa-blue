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

class CLua2DFXDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Create/destroy functions
    static std::variant<bool, CClient2DFX*> AddModel2DFX(lua_State* luaVM, std::uint32_t modelID, CVector position, e2dEffectType effectType, effectDataMap effectData);
    static bool RemoveModel2DFX(std::uint32_t modelID, std::optional<std::uint32_t> index, std::optional<bool> includeDefault);
    static bool ResetModel2DFX(std::uint32_t modelID, std::optional<bool> removeCustomEffects);

    // Set functions
    static bool SetModel2DFXProperties(std::uint32_t modelID, std::uint32_t index, effectDataMap effectData);
    static bool Set2DFXProperties(CClient2DFX* effect, effectDataMap effectData);
    static bool SetModel2DFXProperty(std::uint32_t modelID, std::uint32_t index, e2dEffectProperty property, std::variant<bool, float, std::string> propertyValue);
    static bool Set2DFXProperty(CClient2DFX* effect, e2dEffectProperty property, std::variant<bool, float, std::string> propertyValue);
    static bool SetModel2DFXPosition(std::uint32_t modelID, std::uint32_t index, CVector position);
    static bool Set2DFXPosition(CClient2DFX* effect, CVector position);

    // Get functions
    static std::variant<bool, CLuaMultiReturn<float, float, float>> GetModel2DFXPosition(std::uint32_t modelID, std::uint32_t index);
    static std::variant<bool, CLuaMultiReturn<float, float, float>> Get2DFXPosition(CClient2DFX* effect);
    static std::variant<float, bool, std::string> GetModel2DFXProperty(std::uint32_t modelID, std::uint32_t index, e2dEffectProperty property);
    static std::variant<float, bool, std::string> Get2DFXProperty(CClient2DFX* effect, e2dEffectProperty property);
    static std::variant<bool, effectDataMap> GetModel2DFXProperties(std::uint32_t modelID, std::uint32_t index);
    static std::variant<bool, effectDataMap> Get2DFXProperties(CClient2DFX* effect);
    static std::variant<bool, std::uint32_t> GetModel2DFXCount(std::uint32_t modelID);
};
