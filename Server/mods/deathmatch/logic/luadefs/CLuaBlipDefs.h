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
    static std::variant<CBlip*, bool> CreateBlip(lua_State* luaVM, CVector vecPosition, std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, std::optional<std::uint8_t> r, std::optional<std::uint8_t> g, std::optional<std::uint8_t> b, std::optional<std::uint8_t> a, std::optional<std::int32_t> ordering, std::optional<std::uint32_t> visibleDistance, std::optional<CElement*> visibleTo);
    static std::variant<CBlip*, bool> CreateBlipAttachedTo(lua_State* luaVM, CElement* entity, std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, std::optional<std::uint8_t> r, std::optional<std::uint8_t> g, std::optional<std::uint8_t> b, std::optional<std::uint8_t> a, std::optional<std::int32_t> ordering, std::optional<std::uint32_t> visibleDistance, std::optional<CElement*> visibleTo);

    // Get functions
    static std::uint8_t                                                            GetBlipIcon(CBlip* radarMarker) noexcept;
    static std::uint8_t                                                            GetBlipSize(CBlip* radarMarker) noexcept;
    static CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t> GetBlipColor(CBlip* radarMarker) noexcept;
    static std::int16_t                                                            GetBlipOrdering(CBlip* radarMarker) noexcept;
    static std::uint16_t                                                           GetBlipVisibleDistance(CBlip* radarMarker) noexcept;

    // Set functions
    static bool SetBlipIcon(CElement* radarMarker, std::uint8_t icon);
    static bool SetBlipSize(lua_State* luaVM, CElement* radarMarker, std::uint8_t size) noexcept;
    static bool SetBlipColor(CElement* radarMarker, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) noexcept;
    static bool SetBlipOrdering(CElement* radarMarker, std::int32_t ordering) noexcept;
    static bool SetBlipVisibleDistance(CElement* radarMarker, std::uint32_t visibleDistance) noexcept;
};
