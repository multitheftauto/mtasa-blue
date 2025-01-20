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

class CLuaBlipDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

private:
    // Create functions
    static std::variant<CBlip*, bool> CreateBlip(lua_State* luaVM, const CVector vecPosition, std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, std::optional<std::uint8_t> r, std::optional<std::uint8_t> g, std::optional<std::uint8_t> b, std::optional<std::uint8_t> a, std::optional<std::int16_t> ordering, std::optional<std::uint16_t> visibleDistance, std::optional<CElement*> visibleTo);
    static std::variant<CBlip*, bool> CreateBlipAttachedTo(lua_State* luaVM, CElement* entity, std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, std::optional<std::uint8_t> r, std::optional<std::uint8_t> g, std::optional<std::uint8_t> b, std::optional<std::uint8_t> a, std::optional<std::int16_t> ordering, std::optional<std::uint16_t> visibleDistance, std::optional<CElement*> visibleTo);

    // Get functions
    static auto      GetBlipIcon(CBlip* radarMarker) noexcept;
    static auto      GetBlipSize(CBlip* radarMarker) noexcept;
    static auto      GetBlipColor(CBlip* radarMarker) noexcept;
    static auto      GetBlipOrdering(CBlip* radarMarker) noexcept;
    static auto      GetBlipVisibleDistance(CBlip* radarMarker) noexcept;

    // Set functions
    static bool SetBlipIcon(CElement* radarMarker, std::uint8_t icon);
    static bool SetBlipSize(lua_State* luaVM, CElement* radarMarker, std::uint8_t size);
    static bool SetBlipColor(CElement* radarMarker, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a);
    static bool SetBlipOrdering(CElement* radarMarker, std::int16_t ordering);
    static bool SetBlipVisibleDistance(CElement* radarMarker, std::uint16_t visibleDistance);
};
