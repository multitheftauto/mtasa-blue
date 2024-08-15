/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaBlipDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <lua/CLuaMultiReturn.h>

class CLuaBlipDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Create functions
    static std::variant<CBlip*, bool> CreateBlip(lua_State* const luaVM, const CVector vecPosition, const std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, const std::optional<std::uint8_t> r, const std::optional<std::uint8_t> g, const std::optional<std::uint8_t> b, const std::optional<std::uint8_t> a, const std::optional<std::int16_t> ordering, const std::optional<std::uint16_t> visibleDistance, const std::optional<CElement*> visibleTo);
    static std::variant<CBlip*, bool> CreateBlipAttachedTo(lua_State* const luaVM, CElement* const entity, const std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, const std::optional<std::uint8_t> r, const std::optional<std::uint8_t> g, const std::optional<std::uint8_t> b, const std::optional<std::uint8_t> a, const std::optional<std::int16_t> ordering, const std::optional<std::uint16_t> visibleDistance, const std::optional<CElement*> visibleTo);

    // Get functions
    static std::uint8_t                                                            GetBlipIcon(CBlip* const radarMarker) noexcept;
    static std::uint8_t                                                            GetBlipSize(CBlip* const radarMarker) noexcept;
    static CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t> GetBlipColor(CBlip* const radarMarker) noexcept;
    static std::int16_t                                                            GetBlipOrdering(CBlip* const radarMarker) noexcept;
    static std::uint16_t                                                           GetBlipVisibleDistance(CBlip* const radarMarker) noexcept;

    // Set functions
    static bool SetBlipIcon(CElement* const radarMarker, const std::uint8_t icon);
    static bool SetBlipSize(lua_State* const luaVM, CElement* const radarMarker, std::uint8_t size) noexcept;
    static bool SetBlipColor(CElement* const radarMarker, const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a) noexcept;
    static bool SetBlipOrdering(CElement* const radarMarker, const std::int16_t ordering) noexcept;
    static bool SetBlipVisibleDistance(CElement* const radarMarker, const std::uint16_t visibleDistance) noexcept;
};
