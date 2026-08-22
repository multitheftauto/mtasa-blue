/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/luadefs/CLuaEngineDefs.h
 *  PURPOSE:     Lua engine function definitions
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CLuaDefs.h"
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

class CResource;

class CLuaEngineDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    static std::variant<std::uint32_t, bool> RequestModel(lua_State* luaVM, std::string modelType, std::optional<std::uint32_t> parentModelId,
                                                          std::optional<std::uint32_t> requestedId = std::nullopt);
    static bool                              FreeModel(lua_State* luaVM, std::uint32_t modelId);
    static std::variant<std::uint32_t, bool> GetModelParent(std::uint32_t modelId);
    static std::variant<std::string, bool>   GetModelType(std::uint32_t modelId);
    static std::variant<CResource*, bool>    GetModelAllocatingResource(std::uint32_t modelId);
    static std::vector<std::uint32_t>        GetModelsByType(std::string modelType, std::optional<std::uint32_t> minModelId);
    static bool                              IsModelCustom(std::uint32_t modelId);
};
